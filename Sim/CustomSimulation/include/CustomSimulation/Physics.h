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
#include "G4FastSimulationPhysics.hh"

// Gaussino
#include "GiGaMTCoreMessage/IMessage.h"

namespace Gaussino::CustomSimulation {
  using ParticlePIDs   = std::vector<int>;
  using ParticleWorlds = std::vector<std::string>;

  class Physics : public Gsino::Message, public G4FastSimulationPhysics {

  protected:
    ParticlePIDs   m_particlePIDs   = {};
    ParticleWorlds m_particleWorlds = {};

  public:
    using G4FastSimulationPhysics::G4FastSimulationPhysics;
    virtual void ConstructProcess() override;

    void setParticlePIDs( ParticlePIDs );
    void setParticleWorlds( ParticleWorlds );
  };
} // namespace Gaussino::CustomSimulation
