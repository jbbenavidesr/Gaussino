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
#include "Geant4/G4ParallelWorldPhysics.hh"
// local
#include "GiGaMTFactories/GiGaMTG4PhysicsConstrFAC.h"
#include "GiGaMTFactories/GiGaMTG4PhysicsFactory.h"
#include "ParallelGeometry/Physics.h"

namespace ParallelGeometry {

  template <class ParallelPhysics>
  class PhysicsFactory : public extends<GiGaMTPhysConstr, GiGaFactoryBase<G4ParallelWorldPhysics>> {
    static_assert( std::is_base_of<Physics, ParallelPhysics>::value );

  protected:
    // Layered Mass Geometry: if on, then you can define materials in the parallel world and they will overwrite
    // those in the mass geometry. If more than one parallel world is defined, then those later defined will be
    // on the top of the hierarchy.  
    Gaudi::Property<bool> m_layeredMass {this, "LayeredMass", false, "Parallel world on top of the mass geometry"};
    Gaudi::Property<std::string> m_worldName {this, "WorldName", "", "Corresponding name of the parallel world"};
    // optional
    Gaudi::Property<bool> m_standardProcess {this, "StandardProcess", true, "Run the standard implementation of the ConstructProcess() method or not"};

    inline virtual void additionalParticleConstructor() const {};
    inline virtual void additionalProcessConstructor() const {};

  public:
    using extends::extends;

    ParallelPhysics* construct() const override {
      debug() << "Constructing fast simulation physics: " << name() << endmsg;
      auto physics = new Physics{m_worldName.value(), m_layeredMass.value(), m_standardProcess.value()};
      
      physics->SetPhysicsName( name() );
      physics->SetVerboseLevel( verbosity() );

      physics->setParticleConstructor([&]() {
        debug() << "Constructing a particle constructor for " << name() << " parallel physics" << endmsg;
        // additional implementation
        additionalParticleConstructor();
      } );

      physics->setProcessConstructor([&]() {
        debug() << "Constructing a process constructor for " << name() << " parallel physics" << endmsg;
        // additional implementation
        additionalProcessConstructor();
      } );

      debug() << "Constructed fast simulation physics:" << name() << endmsg;
      return physics;
    }
  };

} // namespace ParallelGeometry
