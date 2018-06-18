/// STD & ATL 
#include <string>
#include <typeinfo> 
/// GaudiKernel 
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/System.h" 
#include  "GaudiKernel/MsgStream.h" 
/// G4 
#include  "Geant4/G4VUserDetectorConstruction.hh"
#include  "Geant4/G4VPhysicalVolume.hh"
#include  "Geant4/G4VUserPrimaryGeneratorAction.hh"
#include  "Geant4/G4VUserPhysicsList.hh"
#include  "Geant4/G4UserRunAction.hh"
#include  "Geant4/G4UserEventAction.hh"
#include  "Geant4/G4UserStackingAction.hh"
#include  "Geant4/G4UserTrackingAction.hh"
#include  "Geant4/G4UserSteppingAction.hh"

/// GiGa
#include "GiGaMTCore/GiGaMTRunManager.h"
#include "GiGaMT/GiGaException.h"
//#include "GiGa/GiGaUtil.h"
/// local 
#include "GiGaMT.h"


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

  
  return StatusCode::SUCCESS;
}

StatusCode GiGaMT::InitializeWorkerThreads() const {

  return StatusCode::SUCCESS;
}
