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
#include "FCCTest/CalorimeterSD.h"
#include "FCCTest/CalorimeterHit.h"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"

namespace FCCTest
{
  CalorimeterSD::CalorimeterSD( G4String name )
      : G4VSensitiveDetector( name ), fHitsCollection( 0 ), fHCID( -1 ), fCellNo( 1 )
  {
    collectionName.insert( "ECalorimeterColl" );
  }

  CalorimeterSD::CalorimeterSD( G4String name, G4int aCellNoInAxis )
      : G4VSensitiveDetector( name ), fHitsCollection( 0 ), fHCID( -1 ), fCellNo( aCellNoInAxis )
  {
    collectionName.insert( "ECalorimeterColl" );
  }

  CalorimeterSD::~CalorimeterSD() {}

  void CalorimeterSD::Initialize( G4HCofThisEvent* hce )
  {
    fHitsCollection = new CalorimeterHitsCollection( SensitiveDetectorName, collectionName[0] );
    if ( fHCID < 0 ) {
      fHCID = G4SDManager::GetSDMpointer()->GetCollectionID( fHitsCollection );
    }
    hce->AddHitsCollection( fHCID, fHitsCollection );

    // fill calorimeter hits with zero energy deposition
    // FIXME: This requires stupid amounts of memory for a lot of cells. Being smarter now.
    //for ( G4int ix = 0; ix < fCellNo; ix++ )
      //for ( G4int iy = 0; iy < fCellNo; iy++ )
        //for ( G4int iz = 0; iz < fCellNo; iz++ ) {
          //CalorimeterHit* hit = new CalorimeterHit();
          //fHitsCollection->insert( hit );
        //}
  }

  G4bool CalorimeterSD::ProcessHits( G4Step* step, G4TouchableHistory* )
  {
    G4double edep = step->GetTotalEnergyDeposit();
    if ( edep == 0. ) return true;

    G4TouchableHistory* touchable = (G4TouchableHistory*)( step->GetPreStepPoint()->GetTouchable() );

    G4int yNo = touchable->GetCopyNumber( 1 );
    G4int xNo = touchable->GetCopyNumber( 2 );
    G4int zNo = touchable->GetCopyNumber( 0 );

    G4int hitID         = fCellNo * fCellNo * xNo + fCellNo * yNo + zNo;
    CalorimeterHit* hit = nullptr;
    if(hitmap.find(hitID) != hitmap.end()){
      hit = hitmap[hitID];
    } else {
      hit = new CalorimeterHit{};
      fHitsCollection->insert(hit);
      hitmap[hitID] = hit;
    }

    if ( hit->GetXid() < 0 ) {
      hit->SetXid( xNo );
      hit->SetYid( yNo );
      hit->SetZid( zNo );
      G4int depth                 = touchable->GetHistory()->GetDepth();
      G4AffineTransform transform = touchable->GetHistory()->GetTransform( depth );
      transform.Invert();
      hit->SetRot( transform.NetRotation() );
      hit->SetPos( transform.NetTranslation() );
    }
    hit->AddEdep( edep );
    return true;
  }
}
