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
#include "GiGaMTCoreRun/GiGaWorkerRunManager.h"

#include "G4Event.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4MTRunManager.hh"
#include "G4StateManager.hh"
#include "G4TransportationManager.hh"
#include "G4UImanager.hh"
#include "G4VUserDetectorConstruction.hh"

#include <mutex>

static std::mutex workerInitMutex;

GiGaWorkerRunManager::GiGaWorkerRunManager()
    : G4WorkerRunManager()
// TODO: what if we need to make these configurable?
{}

GiGaWorkerRunManager* GiGaWorkerRunManager::GetGiGaWorkerRunManager() {
  // Grab thread-local pointer from base class
  auto* wrm = G4RunManager::GetRunManager();
  if ( wrm ) {
    return static_cast<GiGaWorkerRunManager*>( wrm );
  } else {
    return new GiGaWorkerRunManager;
  }
}

void GiGaWorkerRunManager::Initialize() {
  // Locking this initialization to protect currently thread-unsafe services
  std::lock_guard<std::mutex> lock( workerInitMutex );

  // Setup geometry and physics via the base class
  G4RunManager::Initialize();

  /*
  ** The following fragment of code applies all UI commangs from the master command stack.
  ** It has been moved over here from G4InitTool::initThread() because in its previous
  ** location the code had no effect (invalid application state).
  **
  ** Having this code here is OK, but placing it here we are changing some assumptions
  ** implemented in the design of Geant4. This has to do with the handling of multiple
  ** (G4)Runs in the same job. If a second run is executed and no physics or geometry
  ** changes happened between the two runs [Worker]::Initialize() is not called. But UI
  ** commands need to be called anyway, which is not going to happen with our implementation.
  **
  ** If ATLAS ever decides to run multiple G4 runs in the same job, all the MT initialization
  ** will have to be thoroughly reviewed.
  */
  G4MTRunManager*       masterRM = G4MTRunManager::GetMasterRunManager();
  std::vector<G4String> cmds     = masterRM->GetCommandStack();
  G4UImanager*          uimgr    = G4UImanager::GetUIpointer();
  for ( const auto& it : cmds ) {
    int retVal = uimgr->ApplyCommand( it );
    if ( retVal != fCommandSucceeded ) {
      std::string errMsg{ "Failed to apply command <" };
      errMsg += ( it + ">. Return value " + std::to_string( retVal ) );
      error( errMsg );
      // throw GaudiException( errMsg, "GiGaWorkerRunManager::Initialize", StatusCode::FAILURE );
    }
  }

  // Does some extra setup that we need.
  ConstructScoringWorlds();
  // Run initialization in G4RunManager.
  // Normally done in BeamOn.
  RunInitialization();
}

void GiGaWorkerRunManager::InitializeGeometry() {
  const std::string methodName = "GiGaWorkerRunManager::InitializeGeometry";

  // I don't think this does anything
  if ( fGeometryHasBeenDestroyed ) { G4TransportationManager::GetTransportationManager()->ClearParallelWorlds(); }

  // Get the world volume and give it to the kernel
  G4RunManagerKernel* masterKernel = G4MTRunManager::GetMasterRunManagerKernel();
  G4VPhysicalVolume*  worldVol     = masterKernel->GetCurrentWorld();
  kernel->WorkerDefineWorldVolume( worldVol, false );
  // We don't currently use parallel worlds in ATLAS, but someday we might
  kernel->SetNumberOfParallelWorld( masterKernel->GetNumberOfParallelWorld() );

  // These currently do nothing because we don't override
  userDetector->ConstructSDandField();
  userDetector->ConstructParallelSD();

  // Mark g4 geo as initialized
  geometryInitialized = true;
}

void GiGaWorkerRunManager::InitializePhysics() {
  const std::string methodName = "GiGaWorkerRunManager::InitializePhysics";

  // Call the base class
  G4RunManager::InitializePhysics();
}

bool GiGaWorkerRunManager::ProcessEvent( G4Event* event ) {

  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState( G4State_GeomClosed );

  currentEvent = event;

  eventManager->SetVerboseLevel( 3 );
  // eventManager->GetTrackingManager()->SetVerboseLevel(3);
  eventManager->ProcessOneEvent( currentEvent );
  if ( currentEvent->IsAborted() ) {
    warning( "GiGaWorkerRunManager::SimulateFADSEvent: "
             "Event Aborted at Detector Simulation level" );
    currentEvent = nullptr;
    return true;
  }

  this->AnalyzeEvent( currentEvent );
  if ( currentEvent->IsAborted() ) {
    warning( "GiGaWorkerRunManager::SimulateFADSEvent: "
             "Event Aborted at Analysis level" );
    currentEvent = nullptr;
    return true;
  }

  // this->StackPreviousEvent( currentEvent );
  bool abort   = currentEvent->IsAborted();
  currentEvent = nullptr;

  return abort;
}

void GiGaWorkerRunManager::RunTermination() {
  // Not sure what I should put here...
  // Maybe I can just use the base class?
  G4WorkerRunManager::RunTermination();
}
