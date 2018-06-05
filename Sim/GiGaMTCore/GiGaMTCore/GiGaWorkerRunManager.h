#pragma once

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "Geant4/G4WorkerRunManager.hh"

#include "GiGaMTCore/IGiGaMessage.h"

class GiGaWorkerRunManager : public G4WorkerRunManager
{

public:
  // Get the (pure) singleton instance
  // This grabs the thread local G4RunManager and casts it to
  // a GiGaWorkerRunManager. This should be fine as long as
  // GiGaWorkerRunManagers exist in separate threads without contact to
  // any other run manager
  static GiGaWorkerRunManager* GetGiGaWorkerRunManager();

  /// We cram all of the necessary worker run manager initialization here.
  /// In G4 some of it is called instead under BeamOn
  void Initialize() override final;

  /// Does the work actual work and is called per event
  bool ProcessEvent( G4Event* event );

  /// G4 function called at end of run
  void RunTermination() override final;

protected:
  /// Initialize the geometry on the worker
  void InitializeGeometry() override final;

  /// Initialize the physics on the worker
  void InitializePhysics() override final;

private:
  /// Pure singleton private constructor
  GiGaWorkerRunManager();
};
