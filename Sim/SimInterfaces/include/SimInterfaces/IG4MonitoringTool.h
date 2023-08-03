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
// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant
class G4Event;

/** @class IG4MonitoringTool
 *
 * Basic interface to a G4 monitoring algorithm. It is intended to be applied after the simulation by the simulation
 * service.
 *
 * @author Dominik Muller
 */

class IG4MonitoringTool : virtual public IAlgTool {
public:
  DeclareInterfaceID( IG4MonitoringTool, 1, 0 );

  /**  Save the data output.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode monitor( const G4Event& aEvent ) = 0;
};
