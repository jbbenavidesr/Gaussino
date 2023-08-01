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
#include "CaloG4Hit.h"

// Geant4
#include "G4AttDef.hh"
#include "G4AttDefStore.hh"
#include "G4AttValue.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
template <class Type>
class G4Allocator;

namespace Gaussino::G4Par04 {

  G4ThreadLocal G4Allocator<CaloG4Hit>* CaloHitAllocator;

  void* CaloG4Hit::operator new( size_t ) {
    if ( !CaloHitAllocator ) CaloHitAllocator = new G4Allocator<CaloG4Hit>;
    return (void*)CaloHitAllocator->MallocSingle();
  }

  void CaloG4Hit::operator delete( void* aHit ) { CaloHitAllocator->FreeSingle( (CaloG4Hit*)aHit ); }

  void CaloG4Hit::Draw() {
    /// Arbitrary size corresponds to the example macros
    G4ThreeVector  meshSize( 2.325 * mm, 2 * CLHEP::pi / 50. * CLHEP::rad, 3.4 * mm );
    G4int          numPhiCells  = CLHEP::pi * 2. / meshSize.y();
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    // Hits can be filtered out in visualisation
    if ( !pVVisManager->FilterHit( *this ) ) return;
    // Do not draw empty hits
    if ( fEdep <= 0 ) return;
    // Do not plot if default values were not changed
    if ( fRhoId == -1 && fZId == -1 && fPhiId == -1 ) return;
    if ( pVVisManager ) {
      G4Transform3D   trans( fRot, fPos );
      G4VisAttributes attribs;
      G4Tubs          solid( "draw", fRhoId * meshSize.x(), ( fRhoId + 1 ) * meshSize.x(), meshSize.z() / 2.,
                    ( -numPhiCells / 2. + fPhiId ) * meshSize.y(), meshSize.y() );
      // Set colours depending on type of hit (full/fast sim)
      G4double colR = fType == 0 ? 0 : 1;
      G4double colG = fType == 0 ? 1 : 0;
      G4double colB = 0;
      // Set transparency depending on the energy
      // Arbitrary formula
      G4double alpha = 2 * std::log10( fEdep + 1 );
      G4cout << "alpha = " << alpha << G4endl;
      G4Colour colour( colR, colG, colB, alpha );
      attribs.SetColour( colour );
      attribs.SetForceSolid( true );
      pVVisManager->Draw( solid, attribs, trans );
    }
  }

  const std::map<G4String, G4AttDef>* CaloG4Hit::GetAttDefs() const {
    G4bool                        isNew;
    std::map<G4String, G4AttDef>* store = G4AttDefStore::GetInstance( "Par04Hit", isNew );
    if ( isNew ) {
      ( *store )["HitType"] = G4AttDef( "HitType", "Hit Type", "Physics", "", "G4String" );
      ( *store )["Energy"]  = G4AttDef( "Energy", "Energy Deposited", "Physics", "G4BestUnit", "G4double" );
      ( *store )["Time"]    = G4AttDef( "Time", "Time", "Physics", "G4BestUnit", "G4double" );
      ( *store )["Pos"]     = G4AttDef( "Pos", "Position", "Physics", "G4BestUnit", "G4ThreeVector" );
    }
    return store;
  }

  std::vector<G4AttValue>* CaloG4Hit::CreateAttValues() const {
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    values->push_back( G4AttValue( "HitType", "HadPar04Hit", "" ) );
    values->push_back( G4AttValue( "Energy", G4BestUnit( fEdep, "Energy" ), "" ) );
    values->push_back( G4AttValue( "Time", G4BestUnit( fTime, "Time" ), "" ) );
    values->push_back( G4AttValue( "Pos", G4BestUnit( fPos, "Length" ), "" ) );
    return values;
  }

  void CaloG4Hit::Print() {
    std::cout << "\tHit " << fEdep / MeV << " MeV at " << fPos / cm << " cm rotation " << fRot << " (R,phi,z)= ("
              << fRhoId << ", " << fPhiId << ", " << fZId << "), " << fTime << " ns" << std::endl;
  }
} // namespace Gaussino::G4Par04
