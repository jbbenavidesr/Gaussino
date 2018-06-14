#pragma once

#include "Geant4/G4VUserPhysicsList.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"

/** @class GiGaMTG4PhysicsFactory GiGaMTG4PhysicsFactory.h
 *
 *  Helper class to pull in all the G4 physics list into the
 *  framework by constructing a GaudiTool to create them
 *  which will allow to use Gaudi configurables to dynamically
 *  change them.
 *
 *  @author Dominik Muller
 *  @date   14.6.2018
 */

template <class PHYSLIST>
class GiGaMTG4PhysicsFactory : public GiGaFactoryBase<G4VUserPhysicsList>
{
  static_assert( std::is_base_of<G4VUserPhysicsList, PHYSLIST>::value );
  static_assert( std::is_default_constructible<PHYSLIST>::value );

public:
  // Just get the constructors from the base (which gets them from GaudiTool)
  using GiGaFactoryBase::GiGaFactoryBase;

  PHYSLIST* construct() const override { return new PHYSLIST{}; }
};
