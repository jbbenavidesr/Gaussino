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

// GiGa
#include "GiGaMTCoreDet/GaussHitBase.h"

// G4
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"

namespace SimpleCollector {
  class Hit : public Gaussino::HitBase {

  public:
    void* operator new( size_t );
    void operator delete( void* aHit );

    // Set methods
    inline void SetTrackID( G4int track ) { fTrackID = track; };
    inline void SetChamberNb( G4int chamb ) { fChamberNb = chamb; };
    inline void SetEdep( G4double de ) { fEdep = de; };
    inline void SetPos( G4ThreeVector xyz ) { fPos = xyz; };

    // Get methods
    inline G4int         GetTrackID() const { return fTrackID; };
    inline G4int         GetChamberNb() const { return fChamberNb; };
    inline G4double      GetEdep() const { return fEdep; };
    inline G4ThreeVector GetPos() const { return fPos; };

  private:
    G4int         fTrackID;
    G4int         fChamberNb;
    G4double      fEdep;
    G4ThreeVector fPos;
  };

  using HitsCollection = G4THitsCollection<Hit>;

  extern G4ThreadLocal G4Allocator<Hit>* HitAllocator;
} // namespace SimpleCollector
