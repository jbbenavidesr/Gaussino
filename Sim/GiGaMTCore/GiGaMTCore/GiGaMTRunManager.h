#pragma once


#include "GiGaMTCore/IGiGaMessage.h"
// Geant4 includes
#include "Geant4/G4MTRunManager.hh"

//@class GiGaMTRunManager
//@brief Specilization of the G4MTRunManager class for Gaussino
//
//This deactivates all event loop and thread synchronisation features
//present in the normal G4MTRunManager as this is handled by Gaudi.
//Instead it is only used in the setup and to hold the shared objects
//
//The class is a shared singleton and can only be instantiated once.
//
//The corresponding worker thread run manager is GiGaWorkerRunManager.
//
//@author Dominik Muller <dominik.muller@cern.ch>

class GiGaMTRunManager : public G4MTRunManager, public GiGaMessage
{

public:
  // Gets the singleton instance of the GiGaMTRunManager.
  // Returns the same object as G4MTRunManager::GetMasterRunManager()
  static GiGaMTRunManager* GetGiGaMTRunManager();

  // Can only ever be constructed once by calling the single access function
  GiGaMTRunManager(const GiGaMTRunManager &) = delete;
  GiGaMTRunManager(GiGaMTRunManager &&) = delete;

  /// G4 function called at the end of a run
  void RunTermination() override final;

  /// We cram all of the initialization of the run manager stuff in here.
  /// This then includes some of the things that in normal G4 are called
  /// immediately before the event loop.
  void Initialize() override final;

  /// Disable G4's barrier synchronization by implementing these methods
  /// and leaving them empty. Instead, we will do the the synchronisation
  /// of the worker threads manually when initialising the worker payloads
  virtual void ThisWorkerReady() override final{};
  virtual void ThisWorkerEndEventLoop() override final{};

protected:
  /// Initialize the G4 geometry on the master
  void InitializeGeometry() override final;

  // Initialize the physics list on the master
  void InitializePhysics() override final;

  // Disable G4's barrier synchronization by implementing these methods
  // and leaving them empty. These are probably called at some point 
  // internally but we want to do the synchronization ourselves
  virtual void WaitForReadyWorkers() override final{};
  virtual void WaitForEndEventLoopWorkers() override final{};

private:
  /// Pure singleton hence private constructor
  GiGaMTRunManager();
};
