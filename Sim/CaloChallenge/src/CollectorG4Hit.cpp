/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

// Gaussino
#include "CollectorG4Hit.h"
namespace Gsino::CaloChallenge {

  G4ThreadLocal G4Allocator<CollectorG4Hit>* CollectorHitAllocator;

  void* CollectorG4Hit::operator new( size_t ) {
    if ( !CollectorHitAllocator ) CollectorHitAllocator = new G4Allocator<CollectorG4Hit>;
    return (void*)CollectorHitAllocator->MallocSingle();
  }

  void CollectorG4Hit::operator delete( void* aHit ) { CollectorHitAllocator->FreeSingle( (CollectorG4Hit*)aHit ); }

} // namespace Gsino::CaloChallenge
