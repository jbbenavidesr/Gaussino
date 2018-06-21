#pragma once

#include "GaudiKernel/IService.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"

class G4VUserDetectorConstruction;
class G4VUserPrimaryGeneratorAction;
class G4VUserPhysicsList;
class G4UserRunAction;
class G4UserEventAction;
class G4UserStackingAction;
class G4UserSteppingAction;
class G4UserTrackingAction;
class G4VisManager;
class G4VPhysicalVolume;

/** @class IGiGaMTSetUpSvc IGiGaMTSetUpSvc.h GiGaMT/IGiGaMTSetUpSvc.h
 *
 * Abstract interface to the set-up part. Based on GiGa from Vanya Belyaev.
 *
 *  @author Dominik Muller
 */

class IGiGaMTSetUpSvc : virtual public IService
{
public:
  /// Retrieve interface ID
  DeclareInterfaceID(IGiGaMTSetUpSvc, 1, 0);

  /** initialize method
   *  @return status code
   */
  virtual StatusCode initialize() = 0;

  /** finalize  method
   *  @return status code
   */
  virtual StatusCode finalize() = 0;

public:

protected:
  /// virtual destructor
  virtual ~IGiGaMTSetUpSvc(){};
};
