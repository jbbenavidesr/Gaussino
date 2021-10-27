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

#include "ParallelGeometry/Physics.h"

ParallelGeometry::Physics::Physics( std::string parallelWorld, bool layeredMass, bool runStandardProcess )
    : G4ParallelWorldPhysics( parallelWorld, layeredMass ), m_runStandardProcess( runStandardProcess ) {}

void ParallelGeometry::Physics::ConstructParticle() { m_particle_constructor(); }

void ParallelGeometry::Physics::ConstructProcess() {
  if ( m_runStandardProcess ) { G4ParallelWorldPhysics::ConstructProcess(); }
  m_process_constructor();
}

void ParallelGeometry::Physics::setParticleConstructor( ParallelGeometry::ParticleConstructor constr ) {
  m_particle_constructor = constr;
}

void ParallelGeometry::Physics::setProcessConstructor( ParallelGeometry::ProcessConstructor constr ) {
  m_process_constructor = constr;
}
