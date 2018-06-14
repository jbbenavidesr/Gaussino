#include "GiGaMTCore/GiGaMTRunManager.h"

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
  // Using a Meyer's singleton pattern using a static function local
  static GiGaMTRunManager thisManager{};
  return &thisManager;
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


void GiGaMTRunManager::InitializeGeometry()
{
  //FIXME: This needs to do something ...


}

void GiGaMTRunManager::InitializePhysics()
{
  kernel->InitializePhysics();

  G4CascadeInterface::Initialize();
  physicsInitialized = true;

}


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
