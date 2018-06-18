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
