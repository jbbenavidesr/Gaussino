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
  DeclareInterfaceID(IGiGaMTSvc, 1, 0);

  /**  initialize
   *   @return status code
   */
  virtual StatusCode initialize() = 0;

  /**  initialize
   *   @return status code
   */
  virtual StatusCode finalize() = 0;

public:

protected:
  /// virtual destructor
  virtual ~IGiGaMTSvc(){};
};
