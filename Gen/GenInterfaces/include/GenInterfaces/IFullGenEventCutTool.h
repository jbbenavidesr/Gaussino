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
#ifndef MCINTERFACES_IFULLGENEVENTCUTTOOL_H
#define MCINTERFACES_IFULLGENEVENTCUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/GenCollision.h"
#include "HepMC3/GenEvent.h"
#include "HepMCUser/typedefs.h"

#include <vector>

/** @class IFullGenEventCutTool IFullGenEventCutTool.h "MCInterfaces/IFullGenEventCutTool.h"
 *
 *  Abstract interface to generator level cut on full event. This type of
 *  cut is applied on a fully generated event: it includes pile-up interactions
 *  and all particles have already been decayed.
 *
 *  @author Patrick Robbe
 *  @date   2005-11-17
 */

class IFullGenEventCutTool : public extend_interfaces<IAlgTool> {
public:
  DeclareInterfaceID( IFullGenEventCutTool, 3, 0 );

  /** Apply the cut on a event.
   *  @param[in] theEvents      Container of all interactions in the event.
   *  @param[in] theCollisions  Container of hard process informations of each
   *                            pile-up interactions of the event.
   *  @return    true  if the full event passes the cut.
   */
  virtual bool studyFullEvent( const HepMC3::GenEventPtrs& theEvents,
                               const LHCb::GenCollisions&  theCollisions ) const = 0;
};
#endif // MCINTERFACES_ICUTTOOL_H
