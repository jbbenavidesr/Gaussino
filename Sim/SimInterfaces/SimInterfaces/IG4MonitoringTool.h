#pragma once
// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant
class G4Event;


/** @class IG4MonitoringTool
 *
 * Basic interface to a G4 monitoring algorithm. It is intended to be applied after the simulation by the simulation service.
 *
 * @author Dominik Muller
 */

class IG4MonitoringTool: virtual public IAlgTool {
public:
  DeclareInterfaceID(IG4MonitoringTool,1,0);

  /**  Save the data output.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode monitor(const G4Event& aEvent) = 0;
};
