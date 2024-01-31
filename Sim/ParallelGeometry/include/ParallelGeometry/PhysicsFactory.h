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
    Gaudi::Property<bool> m_forceNoPhysics{ this, "ForceNoPhysics", false, "Parallel world without physics" };
    Gaudi::Property<std::vector<int>> m_particlePIDs{
        this, "ParticlePIDs", {}, "List of particle PIDs to be tracked; track all if empty" };

    virtual bool additionalProcessConstructor() const { return true; };

  public:
    using extends::extends;
    virtual StatusCode initialize() override;
    ParallelPhysics*   construct() const override;
  };

} // namespace ParallelGeometry

template <class ParallelPhysics>
ParallelPhysics* ParallelGeometry::PhysicsFactory<ParallelPhysics>::construct() const {
  debug() << "Constructing parallel physics: " << name() << endmsg;
  auto physics = new Physics{ m_worldName.value(), m_layeredMass.value() };

  physics->SetVerboseLevel( verbosity() );
  physics->SetMessageInterface( message_interface() );
  physics->setParticlePIDs( m_particlePIDs.value() );
  physics->setForceNoPhysics( m_forceNoPhysics.value() );
  physics->setProcessConstructor( [&]() {
    debug() << "Constructing a process constructor for " << name() << " parallel physics" << endmsg;

    // additional implementation
    return additionalProcessConstructor();
  } );

  debug() << "Constructed fast simulation physics:" << name() << endmsg;
  return physics;
}

template <class ParallelPhysics>
StatusCode ParallelGeometry::PhysicsFactory<ParallelPhysics>::initialize() {
  return extends::initialize().andThen( [&]() {
    if ( m_forceNoPhysics && !m_particlePIDs.value().empty() ) {
      error() << "You cannot force no physics and select particles at the same time!" << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  } );
}
