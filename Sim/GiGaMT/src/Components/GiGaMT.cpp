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
// from STD & STL
#include <algorithm>
#include <future>
#include <list>
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Stat.h"

// from G4
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"
#include "G4ParticlePropertyTable.hh"
#include "G4ParticleTable.hh"
#include "G4UIsession.hh"
#include "G4UserRunAction.hh"
#include "G4VUserActionInitialization.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VisManager.hh"

// from GiGaMT
#include "CLHEP/Random/RandomEngine.h"
#include "GiGaMT/GiGaActionInitializer.h"
#include "GiGaMTCoreRun/GiGaMTRunManager.h"
#include "GiGaMTCoreRun/GiGaWorkerPayload.h"
#include "GiGaMTCoreRun/GiGaWorkerPilot.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "SimInterfaces/IG4MonitoringTool.h"

#include "HepMC3/GenEvent.h"

// local
#include "GiGaMT.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"

//-----------------------------------------------------------------------------
// Implementation of the general components of the GiGaMT service.
// Set-up related information is given in GiGaMTISetUpSvc.cpp
//
// 20.6.2018 Dominik Muller
//
//-----------------------------------------------------------------------------

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_COMPONENT( GiGaMT )

//=============================================================================
// query interface
//=============================================================================
StatusCode GiGaMT::queryInterface( const InterfaceID& id, void** ppI ) {
  if ( 0 == ppI ) {
    return StatusCode::FAILURE; //  RETURN !!!
  } else if ( IGiGaMTSvc::interfaceID() == id ) {
    *ppI = static_cast<IGiGaMTSvc*>( this );
  } else if ( IGiGaMTSetUpSvc::interfaceID() == id ) {
    *ppI = static_cast<IGiGaMTSetUpSvc*>( this );
  } else {
    return Service::queryInterface( id, ppI ); //  RETURN !!!
  }

  addRef();

  return StatusCode::SUCCESS;
}

//=============================================================================
// service initialization
//=============================================================================
StatusCode GiGaMT::initialize() {
  // initialize the base class
  StatusCode sc = Service::initialize();
  if ( sc.isFailure() ) {
    error() << "Unable to initialize the base class Service " << endmsg;
    return StatusCode::FAILURE;
  }

  if ( m_nWorkerThreads.value() == 0 ) {
    warning() << "No number of threads set. Setting to " << std::thread::hardware_concurrency() << endmsg;
    m_nWorkerThreads.set( std::thread::hardware_concurrency() );
  }

  /// Dump all particles known to Geant4
  if ( m_printMaterials.value() ) {
    G4cout << *G4Material::GetMaterialTable();
    G4cout << "Nist Materials\n";
    G4NistManager::Instance()->ListMaterials( "all" );
  }

  // Main initialization of the run managers using the tools above
  sc = InitializeMainThread();
  if ( sc.isFailure() ) {
    error() << "Unable to initialize main G4 thread" << endmsg;
    return StatusCode::FAILURE;
  }

  sc = InitializeWorkerThreads();
  if ( sc.isFailure() ) {
    error() << "Unable to initialize G4 worker threads" << endmsg;
    return StatusCode::FAILURE;
  }

  /// Dump all particles known to Geant4
  if ( m_printParticles.value() ) {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    particleTable->DumpTable( "all" );
  }

  return StatusCode::SUCCESS;
}

bool GiGaMT::particleKnownToGeant4( int pdg_id ) const {
  return (bool)G4ParticleTable::GetParticleTable()->FindParticle( pdg_id );
}

//=============================================================================
// service finalization
//=============================================================================
StatusCode GiGaMT::finalize() {
  // Trigger the termination of the worker threads which are blocking
  // on an empty queue right now by pushing the sentinel the worker threads
  m_payloadQueue.enqueue( std::nullopt );

  auto main_mgr = GiGaMTRunManager::GetGiGaMTRunManager();
  if ( !m_visMgrFactory.name().empty() ) {
    // additional 2 barriers are added here in case G4 will apply some postprocessing
    // (in Gaussino it is used for now only in the case when we use G4VisManager)
    // the reason for this is that postprocessing can be done on a separate thread and we have
    // to make sure that the master and worker threads will wait with the deletion
    auto& postProcBarrier = GiGaWorkerPilot::GetPostProcessingBarrier( m_nWorkerThreads + 1 );
    auto& finalBarrier    = GiGaWorkerPilot::GetFinalBarrier( m_nWorkerThreads + 1 );
    postProcBarrier.wait();
    // now we call EndOfRunAction a bit eariler (normally it would be done in SafeRunTermination()
    // the idea is that we do not want to delete G4 objects and end the event loop before
    // any additional postprocessing and the use of kept events is done by G4
    auto run_actions = const_cast<G4UserRunAction*>( main_mgr->GetUserRunAction() );
    if ( run_actions ) {
      auto run = main_mgr->GetNonConstCurrentRun();
      if ( !run ) {
        error() << "G4Run not available before terminating the event loop!" << endmsg;
        return StatusCode::FAILURE;
      }
      run_actions->EndOfRunAction( run );
      delete run_actions;
      run_actions = nullptr;
      main_mgr->SetUserAction( run_actions );
    }
    finalBarrier.wait();
  }

  //
  // Wait for the worker threads that will finalize now automatically
  for ( auto& t : m_workerThreads ) { t.join(); }
  always() << "Finalized all G4 worker threads" << endmsg;
  main_mgr->SafeRunTermination();
  delete main_mgr;

  if ( !m_visMgrFactory.name().empty() ) {
    auto vis_mgr = G4VisManager::GetInstance();
    if ( vis_mgr ) {
      debug() << "Deleting G4VisManager: " << m_visMgrFactory.name() << endmsg;
      delete vis_mgr;
    }
  }

  // error printout
  if ( 0 != m_errors.size() || 0 != m_warnings.size() || 0 != m_exceptions.size() ) {
    MsgStream log( msgSvc(), name() );
    // format printout
    log << MSG::ALWAYS << " Exceptions/Errors/Warnings statistics:  " << m_exceptions.size() << "/" << m_errors.size()
        << "/" << m_warnings.size() << endmsg;
    // print exceptions counter
    for ( Counter::const_iterator excp = m_exceptions.begin(); excp != m_exceptions.end(); ++excp ) {
      log << MSG::ALWAYS << " #EXCEPTIONS= " << excp->second << " Message='" << excp->first << "'" << endmsg;
    }
    // print errors counter
    for ( Counter::const_iterator error = m_errors.begin(); error != m_errors.end(); ++error ) {
      log << MSG::ALWAYS << " #ERRORS    = " << error->second << " Message='" << error->first << "'" << endmsg;
    }
    // print warnings
    for ( Counter::const_iterator warning = m_warnings.begin(); warning != m_warnings.end(); ++warning ) {
      log << MSG::ALWAYS << " #WARNINGS  = " << warning->second << " Message='" << warning->first << "'" << endmsg;
    }
  }
  m_errors.clear();
  m_warnings.clear();
  m_exceptions.clear();

  ///  finalize the base class
  return Service::finalize();
}

std::tuple<G4EventProxies, Gaussino::MCTruthPtrs>
GiGaMT::simulate( int eventNumber, Gaussino::MCTruthConverterPtrs&& _in, HepRandomEnginePtr& engine ) const {
  auto                                             start_time = Clock::now();
  std::list<std::promise<Gaussino::GiGaSimReturn>> promises;
  std::list<std::future<Gaussino::GiGaSimReturn>>  futures;
  if ( m_splitPileUp.value() ) {
    // Submit every HepMC event separarely to the queue
    for ( auto& conv : _in ) {
      auto& prom = promises.emplace_back();
      futures.emplace_back( prom.get_future() );
      m_payloadQueue.enqueue(
          GiGaWorkerPayload{ std::move( conv ), engine.createSubRndmEngine(), &prom, eventNumber } );
    }
  } else {
    auto& prom = promises.emplace_back();
    futures.emplace_back( prom.get_future() );
    Gaussino::MCTruthConverterPtr conv = Gaussino::MergeConverters( std::begin( _in ), std::end( _in ) );
    // Merge the individual pileup converters into one
    m_payloadQueue.enqueue( GiGaWorkerPayload{ std::move( conv ), engine, &prom, eventNumber } );
  }
  G4EventProxies        return_events;
  Gaussino::MCTruthPtrs return_truths;
  return_events.reserve( promises.size() );
  return_truths.reserve( promises.size() );
  for ( auto& fut : futures ) {
    auto [evt, tru] = fut.get(); // Copy illision
    return_events.emplace_back( std::move( evt ) );
    return_truths.emplace_back( std::move( tru ) );
  }
  for ( auto& truth : return_truths ) {
    for ( auto& cevt : truth->GetContainedProxies() ) { return_events.push_back( cevt ); }
  }
  if ( auto it = std::unique( std::begin( return_events ), std::end( return_events ) );
       it != std::end( return_events ) ) {
    warning() << "Had to remove non-unique G4EventProxy. Something is wrong!" << endmsg;
  }

  auto end_time = Clock::now();
  debug() << "Simulation complete after " << std::setprecision( 2 )
          << std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - start_time ).count() / 1e9 << " seconds."
          << endmsg;
  for ( auto monitool : m_MoniTools ) {
    for ( auto& g4eventproxy : return_events ) { monitool->monitor( *g4eventproxy->event() ).ignore(); }
  }

  return std::make_tuple<G4EventProxies, Gaussino::MCTruthPtrs>( std::move( return_events ),
                                                                 std::move( return_truths ) );
}

std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> GiGaMT::simulate( int eventNumber, const HepMC3::GenEventPtrs& _in,
                                                                    HepRandomEnginePtr& engine ) const {
  return simulate( eventNumber, m_converterTool->BuildConverter( _in ), engine );
}

std::tuple<G4EventProxyPtr, Gaussino::MCTruthPtr>
GiGaMT::simulateDecay( int eventNumber, const HepMC3::GenParticlePtr& _in, HepRandomEnginePtr& engine ) const {

  auto [proxies, truths] = simulate( eventNumber, { m_converterTool->BuildConverter( _in ) }, engine );
  return { *std::begin( proxies ), *std::begin( truths ) };
}
