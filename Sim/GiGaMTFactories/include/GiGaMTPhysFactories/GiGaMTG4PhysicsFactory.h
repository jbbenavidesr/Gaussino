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

#include "G4VUserPhysicsList.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GaudiAlg/GaudiTool.h"

/** @class GiGaMTG4PhysicsFactory GiGaMTG4PhysicsFactory.h
 *
 *  Helper class to pull in all the G4 physics list into the
 *  framework by constructing a GaudiTool to create them
 *  which will allow to use Gaudi configurables to dynamically
 *  change the used physics list in the Svc.
 *
 *  @author Dominik Muller
 *  @date   14.6.2018
 */

// Only inherit from GaudiTool instead of GiGaTool as we do not care about
// the extended messaging interface here
template <class PHYSLIST>
class GiGaMTG4PhysicsFactory : public extends<GaudiTool, GiGaFactoryBase<G4VUserPhysicsList>>
{
  static_assert( std::is_base_of<G4VUserPhysicsList, PHYSLIST>::value );
  static_assert( std::is_default_constructible<PHYSLIST>::value );

public:
  using extends::extends;

  PHYSLIST* construct() const override { return new PHYSLIST{}; }
};
