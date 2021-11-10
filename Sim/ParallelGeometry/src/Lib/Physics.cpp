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

// local
#include "ParallelGeometry/Physics.h"

// Geant4
#include "Geant4/G4ParallelWorldPhysics.hh"
#include "Geant4/G4ParallelWorldProcess.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4TransportationManager.hh"

ParallelGeometry::Physics::Physics( std::string parallelWorld, bool layeredMass )
    : G4ParallelWorldPhysics( parallelWorld, layeredMass ), m_layeredMass( layeredMass ) {}

void ParallelGeometry::Physics::ConstructProcess() {
  if ( !m_process_constructor() ) { return; }

  if ( m_particlePIDs.empty() ) {
    debug( "Constructing the standard G4 process constructor" );
    G4ParallelWorldPhysics::ConstructProcess();
  } else {
    debug( "Constructing the process constructor for the selected particles." );
    auto parallelProcess = new G4ParallelWorldProcess( namePhysics );
    parallelProcess->SetParallelWorld( namePhysics );
    parallelProcess->SetLayeredMaterialFlag( m_layeredMass );
    auto particleTable = G4ParticleTable::GetParticleTable();

    for ( auto& particlePID : m_particlePIDs ) {
      auto particle = particleTable->FindParticle( particlePID );
      auto pmanager = particle->GetProcessManager();
      pmanager->AddProcess( parallelProcess );
      if ( parallelProcess->IsAtRestRequired( particle ) ) {
        pmanager->SetProcessOrdering( parallelProcess, idxAtRest, 9900 );
      }
      pmanager->SetProcessOrderingToSecond( parallelProcess, idxAlongStep );
      pmanager->SetProcessOrdering( parallelProcess, idxPostStep, 9900 );
    }
  }
}

void ParallelGeometry::Physics::setProcessConstructor( ParallelGeometry::ProcessConstructor constr ) {
  m_process_constructor = constr;
}

void ParallelGeometry::Physics::setParticlePIDs( std::vector<int> particlePIDs ) { m_particlePIDs = particlePIDs; }
