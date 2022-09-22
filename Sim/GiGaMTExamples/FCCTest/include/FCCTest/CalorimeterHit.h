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

#include "G4Allocator.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4VHit.hh"

class G4AttDef;
class G4AttValue;

/// Hadron Calorimeter hit
///
/// It records:
/// - the cell x ID and y ID and z ID
/// - the energy deposit
/// - the cell position and rotation

/** @class CalorimeterHit.h TestGeometry/TestGeometry/CalorimeterHit.h CalorimeterHit.h
 *
 *  Implementation of the hit for the calorimeter.
 *  Based on B5HadCalorimeterHit from examples/basic/B5.
 *
 *  @author Anna Zaborowska
 *
 *  Modification for MT Gaussino
 *
 *  @author Dominik Muller
 */

namespace FCCTest
{
  class CalorimeterHit : public G4VHit
  {
  public:
    CalorimeterHit();
    CalorimeterHit( G4int iX, G4int iY, G4int iZ );
    CalorimeterHit( const CalorimeterHit& right );
    virtual ~CalorimeterHit();

    const CalorimeterHit& operator=( const CalorimeterHit& right );
    int operator==( const CalorimeterHit& right ) const;

    inline void* operator new( size_t );
    inline void operator delete( void* aHit );
    inline static G4Allocator<CalorimeterHit>* CalorimeterHitAllocator();

    virtual void Draw() override;
    virtual const std::map<G4String, G4AttDef>* GetAttDefs() const override;
    virtual std::vector<G4AttValue>* CreateAttValues() const override;
    virtual void Print() override;

    void SetXid( G4int z ) { fxID = z; }
    G4int GetXid() const { return fxID; }

    void SetYid( G4int z ) { fyID = z; }
    G4int GetYid() const { return fyID; }

    void SetZid( G4int z ) { fzID = z; }
    G4int GetZid() const { return fzID; }

    void SetEdep( G4double de ) { fEdep = de; }
    void AddEdep( G4double de ) { fEdep += de; }
    G4double GetEdep() const { return fEdep; }

    void SetPos( G4ThreeVector xyz ) { fPos = xyz; }
    G4ThreeVector GetPos() const { return fPos; }

    void SetRot( G4RotationMatrix rmat ) { fRot = rmat; }
    G4RotationMatrix GetRot() const { return fRot; }

  private:
    G4int fxID;
    G4int fyID;
    G4int fzID;
    G4double fEdep;
    G4ThreeVector fPos;
    G4RotationMatrix fRot;
  };

  typedef G4THitsCollection<CalorimeterHit> CalorimeterHitsCollection;

  inline G4Allocator<CalorimeterHit>* CalorimeterHit::CalorimeterHitAllocator()
  {
    thread_local auto hitAllocator = new G4Allocator<CalorimeterHit>{};
    return hitAllocator;
  }

  inline void* CalorimeterHit::operator new( size_t ) { return (void*)CalorimeterHitAllocator()->MallocSingle(); }

  inline void CalorimeterHit::operator delete( void* aHit )
  {
    CalorimeterHitAllocator()->FreeSingle( (CalorimeterHit*)aHit );
  }
}
