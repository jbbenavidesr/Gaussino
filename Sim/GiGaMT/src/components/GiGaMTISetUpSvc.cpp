/// STD & ATL
#include <string>
#include <typeinfo>
/// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/System.h"
/// G4
#include "Geant4/G4UserEventAction.hh"
#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4UserStackingAction.hh"
#include "Geant4/G4UserSteppingAction.hh"
#include "Geant4/G4UserTrackingAction.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/G4VUserPhysicsList.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"

/// GiGa
#include "GiGaMT/GiGaException.h"
#include "GiGaMTCore/GiGaMTRunManager.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
//#include "GiGa/GiGaUtil.h"
/// local
#include "GiGaMT.h"
#include "GiGaMT/GiGaActionInitializer.h"
#include "GiGaMTCore/GiGaWorkerPilot.h"

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
StatusCode GiGaMT::InitializeMainThread() const
{
  // Start by creating the worker thread by using the corresponding factory.
  // This will instantiate call the singleton for the run manager and apply
  // any configuration given to the factory (none at the moment!).
  auto main_mgr = m_mTRunManagerFactory->construct();
  main_mgr->SetUserInitialization( m_physListFactory->construct() );
  main_mgr->SetUserInitialization( m_ActionInitializerFactory->construct() );
  main_mgr->G4RunManager::SetUserInitialization( m_detConstFactory->construct() );

  main_mgr->Initialize();

  return StatusCode::SUCCESS;
}

StatusCode GiGaMT::InitializeWorkerThreads() const
{

  debug() << "Beginning worker thread creation" << endmsg;
  // Barrier to synchronise the initialization of the threads to only
  // continue and exit the current function successfully when all threads
  // are ready.
  auto& initBarrier = GiGaWorkerPilot::GetInitBarrier( m_nWorkerThreads + 1 );

  for ( size_t iThread = 0; iThread < m_nWorkerThreads; iThread++ ) {
    auto pilot = m_workerPilotFactory->construct();
    pilot->SetInputQueue( &m_payloadQueue );
    // FIXME: Add call-back for converter to workerpilot
    //pilot->SetConverter(
        //[&]( const std::vector<const HepMC3::GenEvent*>& evts ) { return m_conversionTool->g4Event( evts ); } );
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
