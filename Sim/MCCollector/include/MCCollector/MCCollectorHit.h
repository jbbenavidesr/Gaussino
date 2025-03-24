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

namespace MCCollector {
  class Hit : public Gaussino::HitBase {

  public:
    void* operator new( size_t );
    void operator delete( void* aHit );

    inline void     SetEdep( G4double de ) { m_edep = de; }
    inline G4double GetEdep() { return m_edep; }

    inline void          SetEntryPos( G4ThreeVector xyz ) { m_entryPos = xyz; }
    inline G4ThreeVector GetEntryPos() { return m_entryPos; }

    inline void          SetExitPos( G4ThreeVector xyz ) { m_exitPos = xyz; }
    inline G4ThreeVector GetExitPos() { return m_exitPos; }

    inline void          SetMomentum( G4ThreeVector p ) { m_momentum = p; }
    inline G4ThreeVector GetMomentum() { return m_momentum; }

    inline void     SetTimeOfFlight( G4double time ) { m_hitTime = time; }
    inline G4double GetTimeOfFlight() { return m_hitTime; }

  private:
    G4ThreeVector m_entryPos;
    G4ThreeVector m_exitPos;
    G4double      m_edep;
    G4double      m_hitTime;
    G4ThreeVector m_momentum;
  };

  using HitsCollection = G4THitsCollection<Hit>;

  extern G4ThreadLocal G4Allocator<Hit>* HitAllocator;
} // namespace MCCollector
