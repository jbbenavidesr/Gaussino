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
/// STD & ATL
#include <string>
#include <typeinfo>
/// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/System.h"
/// G4
#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserStackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VisManager.hh"

/// GiGa
#include "GiGaMT/GiGaException.h"
#include "GiGaMTCoreRun/GiGaMTRunManager.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"
// #include "GiGa/GiGaUtil.h"
/// local
#include "GiGaMT.h"
#include "GiGaMT/GiGaActionInitializer.h"
#include "GiGaMTCoreRun/GiGaWorkerPilot.h"

// ============================================================================
/**  @file
 *
 *   Implementation of class GiGa
 *   all methods from abstract interface IGiGaMTSetUpSvc
 *
 *   @author: Dominik Muller
 */
// ============================================================================

// This function initaliases the main thread.
// It's implementation is based on commong Geant4 examples and the documentation.
// Note however that not much will happen here as GiGaMTRunManager deactivated a lot
// of the internal workings.
StatusCode GiGaMT::InitializeMainThread() const {
  // Start by creating the worker thread by using the corresponding factory.
  // This will instantiate call the singleton for the run manager and apply
  // any configuration given to the factory (none at the moment!).
  auto main_mgr = m_mTRunManagerFactory->construct();
  main_mgr->SetNumberOfEventsToBeProcessed( m_nWorkerThreads );
  main_mgr->SetUserInitialization( m_physListFactory->construct() );
  main_mgr->SetUserInitialization( m_ActionInitializerFactory->construct() );
  main_mgr->G4RunManager::SetUserInitialization( m_detConstFactory->construct() );

  // G4 Visualization
  if ( !m_visMgrFactory.name().empty() ) {
    debug() << "Constructing G4VisManager: " << m_visMgrFactory.name() << endmsg;
    m_visMgrFactory->construct();
  }

  main_mgr->Initialize();

  // additional checks for G4 visualziation to ensure
  // that the special thread G4VIS was created correctly
  // must be checked after main_mgr->Initialize()
  if ( !m_visMgrFactory.name().empty() ) {
    auto vis_mgr = G4VisManager::GetInstance();
    if ( !vis_mgr ) {
      error() << "Global instance of G4VisManager was not created!" << endmsg;
      return StatusCode::FAILURE;
    }
    auto currentSceneHandler = vis_mgr->GetCurrentSceneHandler();
    if ( !currentSceneHandler ) {
      error() << "Must create an instance of G4VSceneHandler for the visualization thread. "
              << "Add it in the initCommands." << endmsg;
      return StatusCode::FAILURE;
    }
    if ( !currentSceneHandler->GetScene() ) {
      error() << "Must create an instance of G4Scene for the visualization thread. "
              << "Add '/vis/scene/create' and '/vis/sceneHandler/attach' "
              << "or a compound command to the initCommands." << endmsg;
      return StatusCode::FAILURE;
    }
    if ( !vis_mgr->GetCurrentViewer() ) {
      error() << "Must create an instance of G4VViewer for the visualization thread. "
              << "Add it in the initCommands." << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode GiGaMT::InitializeWorkerThreads() const {

  debug() << "Beginning worker thread creation" << endmsg;
  // Barrier to synchronise the initialization of the threads to only
  // continue and exit the current function successfully when all threads
  // are ready.
  auto& initBarrier = GiGaWorkerPilot::GetInitBarrier( m_nWorkerThreads + 1 );

  for ( size_t iThread = 0; iThread < m_nWorkerThreads; iThread++ ) {
    auto pilot = m_workerPilotFactory->construct();
    pilot->SetInputQueue( &m_payloadQueue );

    if ( !m_visMgrFactory.name().empty() ) {
      // activate postprocessing of events if G4 visualization is on
      pilot->setPostProcessing( true );
    }

    // FIXME: Add call-back for converter to workerpilot
    // pilot->SetConverter(
    //[&]( const std::vector<const HepMC3::GenEventPtr>& evts ) { return m_conversionTool->g4Event( evts ); } );
    m_workerThreads.emplace_back( std::move( *pilot ) );
    delete pilot;
  }

  // This barrier is just for safety so that nothing continues beyond this point
  // until the worker threads are initialized in their threads. Might not really be
  // necessary, I don't know. But at least while using no more threads than physical
  // cores all threads should roughly finish initializing at the same time anyway.
  initBarrier.wait();
  info() << "Initialized all G4 worker threads" << endmsg;
  return StatusCode::SUCCESS;
}
