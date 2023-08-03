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
// Geant4
#include "G4ParallelWorldPhysics.hh"
// local
#include "GiGaMTPhysFactories/GiGaMTG4PhysicsConstrFAC.h"
#include "GiGaMTPhysFactories/GiGaMTG4PhysicsFactory.h"
#include "ParallelGeometry/Physics.h"

namespace ParallelGeometry {

  template <class ParallelPhysics>
  class PhysicsFactory : public extends<GiGaMTPhysConstr, GiGaFactoryBase<G4VPhysicsConstructor>> {
    static_assert( std::is_base_of<Physics, ParallelPhysics>::value );

  protected:
    // Layered Mass Geometry: if on, then you can define materials in the parallel world and they will overwrite
    // those in the mass geometry. If more than one parallel world is defined, then those later defined will be
    // on the top of the hierarchy.
    Gaudi::Property<bool> m_layeredMass{ this, "LayeredMass", false, "Parallel world on top of the mass geometry" };
    Gaudi::Property<std::string> m_worldName{ this, "WorldName", "", "Corresponding name of the parallel world" };
    // optional
    Gaudi::Property<std::vector<int>> m_particlePIDs{
        this, "ParticlePIDs", {}, "List of particle PIDs to be tracked; track all if empty" };

    virtual bool additionalProcessConstructor() const { return true; };

  public:
    using extends::extends;
    ParallelPhysics* construct() const override;
  };

} // namespace ParallelGeometry

#include "PhysicsFactory.icpp"
