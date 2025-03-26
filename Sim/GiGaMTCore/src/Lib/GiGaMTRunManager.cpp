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
#include "GiGaMTCoreRun/GiGaMTRunManager.h"

#include "G4GeometryManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4MTRunManagerKernel.hh"
#include "G4Run.hh"
#include "G4StateManager.hh"
#include "G4UserRunAction.hh"

#include "G4CascadeInterface.hh"
#include "G4IonTable.hh"
#include "G4ParticleTable.hh"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

GiGaMTRunManager::GiGaMTRunManager() : G4MTRunManager() {}

GiGaMTRunManager* GiGaMTRunManager::GetGiGaMTRunManager() {
  // Grab thread-local pointer from base class
  auto* wrm = G4MTRunManager::GetMasterRunManager();
  if ( wrm ) {
    return static_cast<GiGaMTRunManager*>( wrm );
  } else {
    return new GiGaMTRunManager;
  }
}

void GiGaMTRunManager::Initialize() {
  // Set up geometry and physics in base class.
  // Why doesn't this call G4MTRunManager?
  // Probably because G4MTRunManager seems to call BeamOn(0) which
  // will trigger an automatic creation of the worker threads. We
  // don't want that!
  G4RunManager::Initialize();
  // Construct scoring worlds
  ConstructScoringWorlds();
  // Run all the commands that are needed to work before
  // the run is initilized (this simulates commands that are
  // added in a main function of Geant4)
  for ( auto& cmd : m_initCommands ) {
    info( "InitCommand(): execute '" + cmd + "'" );
    applyUIcommand( cmd );
  }
  // Run initialization in G4RunManager.
  // Normally done in BeamOn.
  RunInitialization();
  // Setup physics decay channels.
  // Normally done in InitializeEventLoop, from DoEventLoop, from BeamOn.
  GetMTMasterRunManagerKernel()->SetUpDecayChannels();
  // Setup UI commands
  PrepareCommandsStack();
}

void GiGaMTRunManager::RunTermination() {
  std::string msg = "Geant4 terminated the run internally. This should not have happened!";
  throw std::runtime_error( msg );
}

void GiGaMTRunManager::SafeRunTermination() {
  debug( "Geant4 Run terminated." );
  // WaitForEndEventLoopWorkers();
  // -> disabled as it won't do anything, as G4 barrier is disabled
  // G4RunManager::TerminateEventLoop()
  // -> disabled as the event loop is controlled by Gaussino
  G4RunManager::RunTermination();
  // -> this will call kernel->RunTermination() and any EndOfRunAction
}
