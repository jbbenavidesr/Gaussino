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

// Geant4
#include "G4ParticleTable.hh"

// Boost
#include <boost/range/combine.hpp>

// Gaussino
#include "CustomSimulation/Physics.h"

void Gaussino::CustomSimulation::Physics::ConstructProcess() {
  auto particleTable = G4ParticleTable::GetParticleTable();
  for ( auto particleProps : boost::combine( m_particlePIDs, m_particleWorlds ) ) {
    int         particlePID;
    std::string particleWorld;

    boost::tie( particlePID, particleWorld ) = particleProps;
    auto particle                            = particleTable->FindParticle( particlePID );

    if ( !particle ) { error( "Cannot find particle with a PID: " + std::to_string( particlePID ) ); }

    auto worldName = particleWorld;
    if ( worldName.empty() ) worldName = "MassWorld";
    debug( "Activating fast simulation " + particle->GetParticleName() + " in " + worldName );
    ActivateFastSimulation( particle->GetParticleName(), particleWorld );
  }
  G4FastSimulationPhysics::ConstructProcess();
}

void Gaussino::CustomSimulation::Physics::setParticlePIDs( Gaussino::CustomSimulation::ParticlePIDs pids ) {
  m_particlePIDs = pids;
}

void Gaussino::CustomSimulation::Physics::setParticleWorlds( Gaussino::CustomSimulation::ParticleWorlds worlds ) {
  m_particleWorlds = worlds;
}
