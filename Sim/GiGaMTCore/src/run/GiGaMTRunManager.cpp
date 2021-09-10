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

#include "Geant4/G4StateManager.hh"
#include "Geant4/G4GeometryManager.hh"
#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4Run.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4MTRunManagerKernel.hh"

#include "Geant4/G4IonTable.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4CascadeInterface.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"


GiGaMTRunManager::GiGaMTRunManager()
  : G4MTRunManager()
{}


GiGaMTRunManager* GiGaMTRunManager::GetGiGaMTRunManager()
{
  // Grab thread-local pointer from base class
  auto* wrm = G4MTRunManager::GetMasterRunManager();
  if ( wrm ) {
    return static_cast<GiGaMTRunManager*>( wrm );
  } else {
    return new GiGaMTRunManager;
  }
}


void GiGaMTRunManager::Initialize()
{
  // Set up geometry and physics in base class.
  // Why doesn't this call G4MTRunManager?
  // Probably because G4MTRunManager seems to call BeamOn(0) which
  // will trigger an automatic creation of the worker threads. We
  // don't want that!
  G4RunManager::Initialize();
  // Construct scoring worlds
  ConstructScoringWorlds();
  // Run initialization in G4RunManager.
  // Normally done in BeamOn.
  RunInitialization();
  // Setup physics decay channels.
  // Normally done in InitializeEventLoop, from DoEventLoop, from BeamOn.
  GetMTMasterRunManagerKernel()->SetUpDecayChannels();
  // Setup UI commands
  PrepareCommandsStack();
}


//void GiGaMTRunManager::InitializeGeometry()
//{
  ////FIXME: This needs to do something ...


//}

//void GiGaMTRunManager::InitializePhysics()
//{
  //kernel->InitializePhysics();

  //G4CascadeInterface::Initialize();
  //physicsInitialized = true;

//}


// I suspect a lot of this could just be delegated to the base class.
// I wonder if something there breaks in Athena..
void GiGaMTRunManager::RunTermination()
{
  // vanilla G4 calls a different method... why?
  CleanUpPreviousEvents();
  previousEvents->clear();

  if(userRunAction) { userRunAction->EndOfRunAction(currentRun); }

  delete currentRun;
  currentRun = nullptr;
  runIDCounter++;

  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_Idle);

  G4GeometryManager::GetInstance()->OpenGeometry();

  kernel->RunTermination();

  userRunAction = nullptr;
  userEventAction = nullptr;
  userSteppingAction = nullptr;
  userStackingAction = nullptr;
  userTrackingAction = nullptr;
  // physicsList = nullptr;
  userDetector = nullptr;
  userPrimaryGeneratorAction = nullptr;
}
