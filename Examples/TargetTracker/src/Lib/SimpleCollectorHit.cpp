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
#include "SimpleCollector/SimpleCollectorHit.h"

G4ThreadLocal G4Allocator<SimpleCollector::Hit>* SimpleCollector::HitAllocator;

void* SimpleCollector::Hit::operator new( size_t ) {
  if ( !SimpleCollector::HitAllocator ) { SimpleCollector::HitAllocator = new G4Allocator<SimpleCollector::Hit>; }
  return (void*)SimpleCollector::HitAllocator->MallocSingle();
}

void SimpleCollector::Hit::operator delete( void* hit ) {
  if ( !SimpleCollector::HitAllocator ) { SimpleCollector::HitAllocator = new G4Allocator<SimpleCollector::Hit>; }
  SimpleCollector::HitAllocator->FreeSingle( (SimpleCollector::Hit*)hit );
}

