#include "GiGaMTCore/GiGaWorkerPilot.h"
#include "GiGaMTCore/GiGaMTRunManager.h"
#include "GiGaMTCore/GiGaWorkerRunManager.h"

#include "Geant4/G4AutoDelete.hh"
#include "Geant4/G4UImanager.hh"
#include "Geant4/G4UserWorkerThreadInitialization.hh"
#include "Geant4/G4VUserActionInitialization.hh"
#include "Geant4/G4WorkerThread.hh"

GiGaWorkerPilot::GiGaWorkerPilot( GiGaWorkerPilot&& right ) : GiGaMessage( std::move( right ) )
{
  m_input_queue       = right.m_input_queue;
  right.m_input_queue = nullptr;

  m_context       = right.m_context;
  right.m_context = nullptr;

  iWorker  = right.iWorker;
  nWorkers = right.nWorkers;
}

void GiGaWorkerPilot::InitializeWorker()
{
  debug( "Initializing the worker for thread " + std::to_string( iWorker ) );
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
  G4Threading::WorkerThreadLeavesPool(); // FIXME: necessary?
  delete GiGaWorkerRunManager::GetGiGaWorkerRunManager();

  //===============================
  // Step-7: Cleanup split classes
  //===============================
  m_context->DestroyGeometryAndPhysicsVector();

  G4Threading::WorkerThreadLeavesPool();
  delete m_context;
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
  GiGaWorkerPayload payload;
  while ( true ) {
    m_input_queue->wait_dequeue( payload );
    auto & [ evt, engine, ret_promise ] = payload;
    // We treat the case of the G4Event* pointer being a nullptr
    // as the sentinel value and break the loop.
    if ( !evt ) {
      debug( "Sentinel detected, ending loop" );

      // We put the payload back into the queue to trigger a cascading
      // shut down of all threads if one sentinel was pushed into the queue
      m_input_queue->enqueue( payload );
      break;
    }

    // Reset the random number engine of this worker thread
    G4Random::setTheEngine( engine );

    mgr->ProcessEvent( evt );
  }

  FinalizeWorker();
}
