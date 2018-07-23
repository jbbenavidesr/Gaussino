#pragma once

#include "Geant4/G4VUserPhysicsList.hh"
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
