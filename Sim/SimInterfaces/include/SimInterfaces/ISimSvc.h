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

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IService.h"
#include "HepMCUser/typedefs.h"
#include <vector>

// Forward declaration
namespace HepMC {
  class GenEvent;
}

namespace CLHEP {
  class HepRandomEngine;
}

/** @class ISimSvc ISimSvc.h "GenInterfaces/ISimSvc.h"
 *
 *  Basic interface for simulation services
 *
 *  @author Dominik Muller
 *  @date   2018-05-24
 */

static const InterfaceID IID_ISimSvc( "ISimSvc", 2, 0 );

class ISimSvc : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_ISimSvc; }

  /// Simulate the HepMC events
  virtual StatusCode simulate( const HepMC3::GenEventPtrs& theEvents, CLHEP::HepRandomEngine& engine ) = 0;
};
