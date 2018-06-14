#pragma once

#include "GaudiKernel/IService.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
// GiGa

// Forward declaration from G4
class G4Event;
class G4PrimaryVertex;
class G4HCofThisEvent;
class G4TrajectoryContainer;

/** @class IGiGaMTSvc IGiGaMTSvc.h GiGaMT/IGiGaMTSvc.h
 *
 * Definition of the abstract interface to the Geant4 MT implementenation.
 * Based on GiGa from Vanya Belyaev.
 *
 *  @author Dominik Muller
 */

class IGiGaMTSvc : virtual public IService
{
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  /**  initialize
   *   @return status code
   */
  virtual StatusCode initialize() = 0;

  /**  initialize
   *   @return status code
   */
  virtual StatusCode finalize() = 0;

public:
  /** add  primary vertex into GiGaMT/G4
   *
   *  @param  vertex pointer to primary vertex
   *  @return status code
   */
  virtual StatusCode addPrimaryKinematics( G4PrimaryVertex* vertex ) = 0;

  /** get the whole event  object from GiGaMT/G4
   *
   *  @param  event pointer to whole event
   *  @return status code
   */
  virtual StatusCode retrieveEvent( const G4Event*& ) = 0;

  /** get the all hit collections from GiGaMT/G4
   *                  implementation of IGiGaMTSvc abstract interface
   *
   *  @param   collections  pointer to all hit collections
   *  @return  status code
   */
  virtual StatusCode retrieveHitCollections( G4HCofThisEvent*& ) = 0;

  /** get the concrete hit collection from GiGaMT/G4
   *                  implementation of IGiGaMTSvc abstract interface
   *
   *  @param   collection  reference to collection pair
   *  @return  status code
   */
  // virtual StatusCode retrieveHitCollection( GiGaMTHitsByID& ) = 0;

  /** get the concrete hit collection from GiGaMT/G4
   *                  implementation of IGiGaMTSvc abstract interface
   *
   *  @param   collection  reference to collection pair
   *  @return  status code
   */
  // virtual StatusCode retrieveHitCollection( GiGaMTHitsByName& ) = 0;

  /** get all trajectories(trajectory container) from GiGaMT/G4
   *                  implementation of IGiGaMTSvc abstract interface
   *
   *  NB: errors are reported throw exception
   *
   *  @param   trajectories  pointer to trajectory conatiner
   *  @return  self-reference ot IGiGaMTSvc interface
   */
  virtual StatusCode retrieveTrajectories( G4TrajectoryContainer*& ) = 0;

protected:
  /// virtual destructor
  virtual ~IGiGaMTSvc();
};
