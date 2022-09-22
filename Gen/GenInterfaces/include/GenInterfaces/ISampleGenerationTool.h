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
// $Id: ISampleGenerationTool.h,v 1.4 2005-12-31 17:30:37 robbep Exp $
#ifndef GENERATORS_ISAMPLEGENERATIONTOOL_H 
#define GENERATORS_ISAMPLEGENERATIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// From Event
#include "Event/GenCollision.h"
#include "HepMC3/GenEvent.h"
#include "HepMCUser/typedefs.h"
#include "NewRnd/RndCommon.h"

namespace CLHEP {
  class HepRandomEngine;
}

/** @class ISampleGenerationTool ISampleGenerationTool.h "GenInterfaces/ISampleGenerationTool.h"
 *  
 *  Abstract interface to generation of event samples. Concrete tools implement
 *  the algorithms used to generate the various event types generated in LHCb
 *  (Minimum bias, inclusive, signal with repeated hadronization, ...)
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_ISampleGenerationTool( "ISampleGenerationTool" , 
                                                    2 , 0 ) ;

class ISampleGenerationTool : virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID() { return IID_ISampleGenerationTool; }

  /** Generate a set of pile-up interactions to form an event
   *  @param[in]  nPileUp        Number of pile-up event to generate for 
   *                             this event.
   *  @param[out] theEvents      Container of the generated pile-up 
   *                             interactions.
   *  @param[out] theCollisions  Container of the hard process information
   *                             for each pile-up interaction.
   */
  virtual bool generate( const unsigned int nPileUp ,
                         HepMC3::GenEventPtrs & theEvents ,
                         LHCb::GenCollisions & theCollisions,
                         HepRandomEnginePtr & engine ) const = 0 ;

  /// Print counters and efficiencies at the end of the job.
  virtual void printCounters( ) const = 0 ;
};
#endif // GENERATORS_ISAMPLEGENERATIONTOOL_H
