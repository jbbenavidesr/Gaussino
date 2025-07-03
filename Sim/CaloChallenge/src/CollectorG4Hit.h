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

namespace Gsino::CaloChallenge {
  /**
   * @brief Represents a collector hit (Geant4)
   *
   * This hit is part of Geant4 and represents a hit in the
   * calorimeter simulation left by an incident particle in the CaloChallenge setup.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  struct CollectorG4Hit : public Gaussino::HitBase {
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

    inline void          SetMomentum( const G4ThreeVector& aP ) { m_P = aP; }
    inline G4ThreeVector GetMomentum() const { return m_P; }

    inline void SetPDG( const int aPDG ) { m_PDG = aPDG; }
    inline int  GetPDG() const { return m_PDG; }

    inline void     SetTime( G4double aTime ) { m_Time = aTime; }
    inline G4double GetTime() const { return m_Ep; }

  private:
    int           m_PDG  = 0;
    double        m_Ek   = 0;
    double        m_Ep   = 0;
    double        m_Time = 0;
    G4ThreeVector m_P    = {};
    G4ThreeVector m_Pos  = {};
    G4ThreeVector m_Dir  = {};
  };

  using CollectorHitsCollection = G4THitsCollection<CollectorG4Hit>;

  extern G4ThreadLocal G4Allocator<CollectorG4Hit>* CollectorHitAllocator;

} // namespace Gsino::CaloChallenge
