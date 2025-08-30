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
#include "CalorimeterCollector/CalorimeterCollectorHit.h"

// G4
#include "G4UnitsTable.hh"

G4ThreadLocal G4Allocator<CalorimeterCollector::Hit>* CalorimeterCollector::HitAllocator;

void* CalorimeterCollector::Hit::operator new( size_t ) {
  if ( !CalorimeterCollector::HitAllocator )
  { 
    CalorimeterCollector::HitAllocator = new G4Allocator<CalorimeterCollector::Hit>; 
  }
  return (void*)CalorimeterCollector::HitAllocator->MallocSingle();
}

void CalorimeterCollector::Hit::operator delete( void* hit ) {
  if ( !CalorimeterCollector::HitAllocator ) 
  { 
    CalorimeterCollector::HitAllocator = new G4Allocator<CalorimeterCollector::Hit>; 
  }
  CalorimeterCollector::HitAllocator->FreeSingle( (CalorimeterCollector::Hit*)hit );
}

void CalorimeterCollector::Hit::Add(G4double de, G4double dl)
{
  fEdep += de;
  fTrackLength += dl;
}

void CalorimeterCollector::Hit::Print()
{
  G4cout << "Edep: " << std::setw(7) << G4BestUnit(fEdep, "Energy")
         << " track length: " << std::setw(7) << G4BestUnit(fTrackLength, "Length") 
         << " sensitive detector: " << std::setw(7) << fSensitiveDetector
         << " layer number: " << std::setw(7) << fLayerNumber
         << " eventID: " << std::setw(7) << fEventID
         << G4endl;
}


