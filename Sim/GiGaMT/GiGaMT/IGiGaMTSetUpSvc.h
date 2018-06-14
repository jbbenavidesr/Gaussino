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
  static const InterfaceID& interfaceID();

  /** initialize method
   *  @return status code
   */
  virtual StatusCode initialize() = 0;

  /** finalize  method
   *  @return status code
   */
  virtual StatusCode finalize() = 0;

public:
  /** set detector constructon module
   *
   *  @param  detector pointer to detector construction module
   *  @return self-reference ot IGiGaMTSetUpSvc interface
   */
  virtual StatusCode setConstruction( G4VUserDetectorConstruction* ) = 0;

  /** set new world wolume
   *
   *  @param  world  pointer to  new world volume
   *  @return status code
   */
  virtual StatusCode setDetector( G4VPhysicalVolume* ) = 0;

  /** set new generator
   *
   *  @param  generator  pointer to new generator
   *  @return status code
   */
  virtual StatusCode setGenerator( G4VUserPrimaryGeneratorAction* ) = 0;

  /** set new physics list
   *
   *  @param  physics  pointer to physics list
   *  @return status code
   */
  virtual StatusCode setPhysics( G4VUserPhysicsList* ) = 0;

  /** set new run action
   *
   *  @param  action  pointer to new run action
   *  @return status code
   */
  virtual StatusCode setRunAction( G4UserRunAction* ) = 0;

  /** set new event action
   *
   *  @param  action  pointer to new event action
   *  @return status code
   */
  virtual StatusCode setEvtAction( G4UserEventAction* ) = 0;

  /** set new stacking action
   *
   *  @param  action  pointer to new stacking action
   *  @return status code
   */
  virtual StatusCode setStacking( G4UserStackingAction* ) = 0;

  /** set new tracking  action
   *
   *  @param  action  pointer to new tracking action
   *  @return status code
   */
  virtual StatusCode setTracking( G4UserTrackingAction* ) = 0;

  /** set new stepping  action
   *
   *  @param  action  pointer to new stepping action
   *  @return status code
   */
  virtual StatusCode setStepping( G4UserSteppingAction* ) = 0;

protected:
  /// virtual destructor
  virtual ~IGiGaMTSetUpSvc();
};
