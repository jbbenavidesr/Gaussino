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
#pragma once

#include "GiGaMTCoreMessage/UIMessage.h"
// Geant4 includes
#include "G4MTRunManager.hh"

//@class GiGaMTRunManager
//@brief Specilization of the G4MTRunManager class for Gaussino
//
// This deactivates all event loop and thread synchronisation features
// present in the normal G4MTRunManager as this is handled by Gaudi.
// Instead it is only used in the setup and to hold the shared objects
//
// The class is a shared singleton and can only be instantiated once.
//
// The corresponding worker thread run manager is GiGaWorkerRunManager.
//
//@author Dominik Muller <dominik.muller@cern.ch>

class GiGaMTRunManager : public G4MTRunManager, public Gsino::UIMessage {

public:
  // Gets the singleton instance of the GiGaMTRunManager.
  // Returns the same object as G4MTRunManager::GetMasterRunManager()
  static GiGaMTRunManager* GetGiGaMTRunManager();

  // Can only ever be constructed once by calling the single access function
  GiGaMTRunManager( const GiGaMTRunManager& ) = delete;
  GiGaMTRunManager( GiGaMTRunManager&& )      = delete;

  /// G4 function called at end of run. Should not be called. G4 can call
  /// it internally, but it does not handle the event loop. Sould result
  /// in an error. For GiGa use SafeRunTermination.
  void RunTermination() override final;

  /// RunTermination called by GiGaMT to make sure that G4 did not
  /// trigger the termination of the run internally.
  void SafeRunTermination();

  /// We cram all of the initialization of the run manager stuff in here.
  /// This then includes some of the things that in normal G4 are called
  /// immediately before the event loop.
  void Initialize() override final;

  /// Disable G4's barrier synchronization by implementing these methods
  /// and leaving them empty. Instead, we will do the the synchronisation
  /// of the worker threads manually when initialising the worker payloads
  virtual void ThisWorkerReady() override final{};
  virtual void ThisWorkerEndEventLoop() override final{};

  void setInitCommands( std::vector<std::string> initCommands ) { m_initCommands = initCommands; }

protected:
  /// Initialize the G4 geometry on the master
  // void InitializeGeometry() override final;

  // Initialize the physics list on the master
  // void InitializePhysics() override final;

  // Disable G4's barrier synchronization by implementing these methods
  // and leaving them empty. These are probably called at some point
  // internally but we want to do the synchronization ourselves
  virtual void WaitForReadyWorkers() override final{};
  virtual void WaitForEndEventLoopWorkers() override final{};
  virtual void CreateAndStartWorkers() override final{};

private:
  /// Pure singleton hence private constructor
  GiGaMTRunManager();

  std::vector<std::string> m_initCommands = {};
};
