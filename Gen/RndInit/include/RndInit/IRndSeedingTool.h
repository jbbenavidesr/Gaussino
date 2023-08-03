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

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class IRndSeedingTool IRndSeedingTool.h "RndInit/IRndSeedingTool.h"
 *
 *  Abstract interface to random enging seeding tool
 *  Placeholder tool as this will get nice and entertaining when multithreading
 *
 *  @author Dominik Muller
 *  @date   2018-01-30
 */

static const InterfaceID IID_IRndSeedingTool( "IRndSeedingTool", 2, 0 );

class IRndSeedingTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IRndSeedingTool; }

  /** seed the gaudi random generator
   *  Last seed is hash including seed1, seed2 and name()
   *  @param[in] seed1 First  seed (typically run number)
   *  @param[in] seed2 Second seed (typically event number)
   *  @param[in] Optional to allow obtaining the used seeds
   *  @return StatusCode
   */
  virtual StatusCode seed( unsigned int seed1, unsigned long long seed2,
                           std::shared_ptr<std::vector<long int>> seeds = nullptr ) = 0;
};
