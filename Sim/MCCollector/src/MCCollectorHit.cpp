/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

// local
#include "MCCollectorHit.h"

G4ThreadLocal G4Allocator<MCCollector::Hit> MCCollector::HitAllocator;

void* MCCollector::Hit::operator new( size_t ) {
  void* aHitAllocator;
  aHitAllocator = (void*)MCCollector::HitAllocator.MallocSingle();
  return aHitAllocator;
}

void MCCollector::Hit::operator delete( void* aHitAllocator ) {
  MCCollector::HitAllocator.FreeSingle( (MCCollector::Hit*)aHitAllocator );
}
