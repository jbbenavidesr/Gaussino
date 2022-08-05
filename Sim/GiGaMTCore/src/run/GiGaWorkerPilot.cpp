/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#include "GiGaMTCoreRun/GiGaWorkerPilot.h"
#include "GiGaMTCoreRun/GiGaMTRunManager.h"
#include "GiGaMTCoreRun/GiGaWorkerRunManager.h"
#include "GaudiKernel/GaudiException.h"

#include "GiGaMTCoreTruth/GaussinoEventInformation.h"

#include <string>

#include "G4AutoDelete.hh"
#include "G4Event.hh"
#include "G4UImanager.hh"
#include "G4UserWorkerThreadInitialization.hh"
#include "G4VUserActionInitialization.hh"
#include "G4WorkerThread.hh"
#include "G4Run.hh"

GiGaWorkerPilot::GiGaWorkerPilot( GiGaWorkerPilot&& right ) : GiGaMessage( std::move( right ) )
{
  m_input_queue       = right.m_input_queue;
  right.m_input_queue = nullptr;

  m_postprocessing = right.m_postprocessing;

  m_context       = right.m_context;
  right.m_context = nullptr;

  iWorker       = right.iWorker;
  nWorkers      = right.nWorkers;
  nDeleted      = right.nDeleted;
  nCreated      = right.nCreated;
  nKept         = right.nKept;
  m_track_eventstructure = right.m_track_eventstructure;
  m_for_cleanup = std::move( right.m_for_cleanup );
}

void GiGaWorkerPilot::InitializeWorker()
{
  debug( "Initializing the worker for thread " + std::to_string( iWorker ) );
  GiGaMessage::NameTag = "Worker #" + std::to_string(iWorker);
  // Following code is modelled based on the code in
  // G4UserWorkerThreadInitialization::CreateAndStartWorker and
  // G4MTRunManagerKernel::StartThread with slight modifications.
  G4Threading::WorkerThreadJoinsPool(); // FIXME: necessary?
  auto master_mgr = GiGaMTRunManager::GetGiGaMTRunManager();
  auto worker_mgr = GiGaWorkerRunManager::GetGiGaWorkerRunManager();
  m_context       = new G4WorkerThread;
  m_context->SetThreadId( iWorker );
  // userWorkerThreadInitialization->CreateAndStartWorker( context );
  G4Threading::G4SetThreadId( iWorker );
  G4UImanager::GetUIpointer()->SetUpForAThread( iWorker );

  //============================
  // Step-2: Initialize worker thread
  //============================
  if ( master_mgr->GetUserActionInitialization() ) {
    G4VSteppingVerbose* sv = master_mgr->GetUserActionInitialization()->InitializeSteppingVerbose();
    if ( sv ) {
      G4VSteppingVerbose::SetInstance( sv );
    }
  }

  m_context->BuildGeometryAndPhysicsVector();
  worker_mgr->SetWorkerThread( m_context );

  //================================
  // Step-3: Setup worker run manager
  //================================
  // Set the detector and physics list to the worker thread. Share with master
  const G4VUserDetectorConstruction* detector = master_mgr->GetUserDetectorConstruction();
  worker_mgr->G4RunManager::SetUserInitialization( const_cast<G4VUserDetectorConstruction*>( detector ) );
  const G4VUserPhysicsList* physicslist = master_mgr->GetUserPhysicsList();
  worker_mgr->SetUserInitialization( const_cast<G4VUserPhysicsList*>( physicslist ) );

  //================================
  // Step-4: Initialize worker run manager
  //================================
  if ( master_mgr->GetUserActionInitialization() ) {
    master_mgr->GetNonConstUserActionInitialization()->Build();
  }
  worker_mgr->Initialize();
}

void GiGaWorkerPilot::FinalizeWorker()
{
  debug( "Finalizing the worker for thread " + std::to_string( iWorker ) );
  CleanUp();                             // Delete any remaining events handled by this worker thread.
  debug( "Finished clean-up for thread " + std::to_string( iWorker ) );
  G4Threading::WorkerThreadLeavesPool(); // FIXME: necessary?
  delete GiGaWorkerRunManager::GetGiGaWorkerRunManager();
  debug( "Deleted run manager " + std::to_string( iWorker ) );

  //===============================
  // Step-7: Cleanup split classes
  //===============================
  m_context->DestroyGeometryAndPhysicsVector();

  G4Threading::WorkerThreadLeavesPool();
  delete m_context;
  if (m_postprocessing && nKept > 0) {
    std::stringstream strea;
    strea << "Asked to keep some events by Geant4. "
          << "Created: " << nCreated << ", Kept: " << nKept << ", Deleted: " << nDeleted;
    debug( strea.str() );
  } else if ( nCreated > nDeleted ) {
    std::stringstream strea;
    strea << "Didn't delete all G4 events " << nDeleted << "/" << nCreated;
    warning( strea.str() );
  } else if ( nCreated < nDeleted ) {
    error( "Deleted more (!!!!) G4 events than created." );
  } else {
    debug( "Number of created and deleted events matches." );
  }
}

void GiGaWorkerPilot::RunTermination() {}

void GiGaWorkerPilot::operator()()
{
  debug( "Starting up thread ..." );

  // Create and initialize the worker run manager
  InitializeWorker();

  debug( "Finished initialization, awaiting something to simulate!" );
  // Get the pointer to the GiGaWorkerRunManager
  auto mgr = GiGaWorkerRunManager::GetGiGaWorkerRunManager();
  GetInitBarrier().wait();

  // Begin the main loop by
  GiGaWorkerPayloadOpt payload;
  while ( true ) {
    m_input_queue->wait_dequeue( payload );

    m_before_sim = "";
    m_after_sim = "";
    m_after_cleanup = "";
    debug( "Queue length " + std::to_string( m_input_queue->size_approx() ) );
    if ( !payload ) {
      debug( "Sentinel detected, ending loop" );
      // now introducing a potentially dangerous loop that should wait
      // until all the worker threads finished the postprocessing steps in G4
      // note: this should be working, but we might want to be extra careful
      // when running productions. Here, I added an additional flag that will 
      // activate this extra checkk when we actually know that the
      // postprocessing takes place
      if ( m_postprocessing ) {
        while ( nCreated > nDeleted && nKept < nCreated - nDeleted ) {
          CleanUp();
          std::this_thread::sleep_for(std::chrono::milliseconds( 500 ));
        }
      }

      // We put the payload back into the queue to trigger a cascading
      // shut down of all threads if one sentinel was pushed into the queue
      m_input_queue->enqueue( std::move( payload ) );
      break;
    }
    auto & [ truth_converter, engine, ret_promise ] = *payload;
    CleanUp();
    // We now have a list of MCTruthConverter. Each needs to be triggered to link their contents and fill the
    // Geant4 event to be simulated.
    auto evt = new G4Event{};
    Gaussino::MCTruthTrackerPtr tracker =
        std::make_unique<Gaussino::MCTruthTracker>( std::move( *truth_converter.get() ), evt );
    if(m_track_eventstructure){
      std::stringstream sstr;
      sstr << "\nBefore simulation\n";
      tracker->DumpToStream( sstr );
      m_before_sim = sstr.str();
      if ( printDebug() ) {
        debug( m_before_sim );
      }
    }
    auto code = tracker->VerifyStructure();
    if(code == 1){
      error(m_before_sim);
      throw GaudiException{"Not all particles reachable from root particles", "MCTruth", StatusCode::FAILURE};
    } else if (code == 2){
      error(m_before_sim);
      throw GaudiException{"Not all particles reachable from final state particles", "MCTruth", StatusCode::FAILURE};
    }
    evt->SetUserInformation( new GaussinoEventInformation( tracker.get() ) );
    debug( "Dequeued event with " + std::to_string( evt->GetNumberOfPrimaryVertex() ) + " vertices." );

    // Reset the random number engine of this worker thread
    G4Random::setTheEngine( engine.get() );

    mgr->ProcessEvent( evt );

    if (m_postprocessing) {
      if( evt->ToBeKept() ) {
        debug("Asked to keep this event by Geant4");
        GiGaMTRunManager::GetGiGaMTRunManager()->GetNonConstCurrentRun()->StoreEvent(evt);
        nKept++;
      }
    }

    if(m_track_eventstructure){
      std::stringstream sstr;
      sstr << "\nAfter simulation\n";
      tracker->DumpToStream( sstr );
      m_after_sim = sstr.str();
      if ( printDebug() ) {
        debug( m_after_sim );
      }
    }

    code = tracker->VerifyStructure();
    if(code == 1){
      error(m_before_sim);
      error(m_after_sim);
      throw GaudiException{"Not all particles reachable from root particles", "MCTruth", StatusCode::FAILURE};
    } else if (code == 2){
      error(m_before_sim);
      error(m_after_sim);
      throw GaudiException{"Not all particles reachable from final state particles", "MCTruth", StatusCode::FAILURE};
    }
    Gaussino::MCTruthPtr mctruth = std::make_unique<Gaussino::MCTruth>( std::move( *tracker.get() ) );

    if(m_track_eventstructure){
      std::stringstream sstr;
      sstr << "\nAfter cleanup\n";
      mctruth->DumpToStream( sstr );
      m_after_cleanup = sstr.str();
      if ( printDebug() ) {
        debug( m_after_cleanup );
      }
    }

    code = mctruth->VerifyStructure();
    if(code == 1){
      error(m_before_sim);
      error(m_after_sim);
      error(m_after_cleanup);
      throw GaudiException{"Not all particles reachable from root particles", "MCTruth", StatusCode::FAILURE};
    } else if (code == 2){
      error(m_before_sim);
      error(m_after_sim);
      error(m_after_cleanup);
      throw GaudiException{"Not all particles reachable from final state particles", "MCTruth", StatusCode::FAILURE};
    }
    debug( "Geant4 finished processing the event." );
    G4EventProxyPtr proxy = std::make_shared<G4EventProxy>(evt, mctruth.get(), this);
    ret_promise->set_value( std::make_tuple( std::move( proxy ), std::move( mctruth ) ) );
    nCreated++;
  }

  FinalizeWorker();
}

void GiGaWorkerPilot::RegisterForCleanUp( G4Event* evt )
{
  // Need to lock access as multiple Gaudi TES destruction
  // could potentially add here in parallel
  std::lock_guard<std::mutex> guard{m_cleanup_lock};
  m_for_cleanup.push_back( evt );
}

void GiGaWorkerPilot::CleanUp()
{
  // Might be incorrect but avoids taking the lock. As this function
  // is also called during finalisation, no events can get lost.
  if ( m_for_cleanup.size() == 0 ) return;
  // Need to lock access to prevent additional events being
  // pushed into the vector during cleanup
  std::lock_guard<std::mutex> guard{m_cleanup_lock};
  auto evs_to_remove = std::remove_if(m_for_cleanup.begin(), m_for_cleanup.end(), [&](G4Event* evt) -> bool {
    if (m_postprocessing) {
      auto postActions = evt->GetNumberOfGrips();
      if (postActions > 0) {
        debug( "Not deleting G4Event yet. No. of postprocessing actions remaining: " + std::to_string(postActions) );
        return false;
      } else if ( evt->ToBeKept() ) {
        debug( "Not deleting G4Event. Gaussino was asked to keep the event." );
        return true;
      }
    }
    debug( "Deleting G4Event" );
    nDeleted++;
    delete evt;
    return true;
  } );
  m_for_cleanup.erase(evs_to_remove, m_for_cleanup.end());
}
