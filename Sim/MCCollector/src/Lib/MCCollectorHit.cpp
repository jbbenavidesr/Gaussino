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
#include "MCCollector/MCCollectorHit.h"

G4ThreadLocal G4Allocator<MCCollector::Hit>* MCCollector::HitAllocator;

void* MCCollector::Hit::operator new( size_t ) {
  if ( !MCCollector::HitAllocator ) { MCCollector::HitAllocator = new G4Allocator<MCCollector::Hit>; }
  return (void*)MCCollector::HitAllocator->MallocSingle();
}

void MCCollector::Hit::operator delete( void* hit ) {
  if ( !MCCollector::HitAllocator ) { MCCollector::HitAllocator = new G4Allocator<MCCollector::Hit>; }
  MCCollector::HitAllocator->FreeSingle( (MCCollector::Hit*)hit );
}
