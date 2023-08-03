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
#pragma once

// Geant4
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"

// Gaussino
#include "GiGaMTCoreDet/GaussHitBase.h"

class G4LogicalVolume;
class G4AttDef;

namespace Gaussino::G4Par04 {
  struct CollectorG4Hit : public HitBase {
    using Map = std::map<int, CollectorG4Hit*>;

    void* operator new( size_t );
    void operator delete( void* aHit );

    inline void   SetKineticEnergy( double aEk ) { m_Ek = aEk; }
    inline double GetKineticEnergy() const { return m_Ek; }

    inline void   SetPrimaryEnergy( double aEp ) { m_Ep = aEp; }
    inline double GetPrimaryEnergy() const { return m_Ep; }

    inline void          SetPosition( const G4ThreeVector& aPos ) { m_Pos = aPos; }
    inline G4ThreeVector GetPosition() const { return m_Pos; }

    inline void          SetDirection( const G4ThreeVector& aDir ) { m_Dir = aDir; }
    inline G4ThreeVector GetDirection() const { return m_Dir; }

    inline void SetPDG( const int aPDG ) { m_PDG = aPDG; }
    inline int  GetPDG() const { return m_PDG; }

  private:
    int           m_PDG = 0;
    double        m_Ek  = 0;
    double        m_Ep  = 0;
    G4ThreeVector m_Pos = {};
    G4ThreeVector m_Dir = {};
  };

  using CollectorHitsCollection = G4THitsCollection<CollectorG4Hit>;

  extern G4ThreadLocal G4Allocator<CollectorG4Hit>* CollectorHitAllocator;

} // namespace Gaussino::G4Par04
