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

#include "ParallelGeometry/World.h"

ParallelGeometry::World::World( std::string name ) : G4VUserParallelWorld( name ) {}

void ParallelGeometry::World::Construct() { m_world_constructor( GetWorld() ); }

void ParallelGeometry::World::ConstructSD() { m_sd_constructor(); }

void ParallelGeometry::World::setWorldConstructor( ParallelGeometry::WorldConstructor constr ) {
  m_world_constructor = constr;
};

void ParallelGeometry::World::setSDConstructor( ParallelGeometry::SDConstructor constr ) { m_sd_constructor = constr; };
