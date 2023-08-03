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

// Gaussino
#include "CustomSimulation/Physics.h"
#include "GiGaMTPhysFactories/GiGaMTG4PhysicsConstrFAC.h"
#include "GiGaMTPhysFactories/GiGaMTG4PhysicsFactory.h"

namespace Gaussino::CustomSimulation {
  template <class TPhysics>
  class PhysicsFactory : public GiGaMTG4PhysicsConstrFAC<TPhysics> {
    static_assert( std::is_base_of<Physics, TPhysics>::value );

  protected:
    Gaudi::Property<ParticlePIDs> m_particlePIDs{
        this, "ParticlePIDs", {}, "List of particle PIDs to be tracked; track all if empty" };
    Gaudi::Property<ParticleWorlds> m_particleWorlds{
        this, "ParticleWorlds", {}, "List of particle parallel worlds; empty means mass world" };

  public:
    using base_class = GiGaMTG4PhysicsConstrFAC<TPhysics>;
    using base_class::GiGaMTG4PhysicsConstrFAC;

    StatusCode initialize() override;
    TPhysics*  construct() const override;
  };

  using DefaultPhysicsFactory = PhysicsFactory<Physics>;

} // namespace Gaussino::CustomSimulation

#include "PhysicsFactory.icpp"
