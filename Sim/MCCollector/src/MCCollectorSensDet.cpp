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
// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// from Gaudi
#include "GaudiKernel/MsgStream.h"

// from Geant4
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4TouchableHistory.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4ios.hh"

// local
#include "MCCollectorSensDet.h"


void MCCollector::SensDet::Initialize( G4HCofThisEvent* HCE ) {

  m_col = new HitsCollection( SensitiveDetectorName, collectionName[0] );

  int HCID = G4SDManager::GetSDMpointer()->GetCollectionID( SensitiveDetectorName + "/" + collectionName[0] );

  HCE->AddHitsCollection( HCID, m_col );

  // standard print left as is
  Print( " Initialize(): CollectionName='" + m_col->GetName() + "' for SensDet='" + m_col->GetSDname() + "'",
         StatusCode::SUCCESS, MSG::VERBOSE )
      .ignore();
}

bool MCCollector::SensDet::ProcessHits( G4Step* step, G4TouchableHistory* /* history */ ) {
  if ( 0 == step ) return false;

  auto track = step->GetTrack();

  // If required to have energy deposition check
  double edep = step->GetTotalEnergyDeposit();
  if ( m_requireEDep.value() && edep <= 0.0 ) return false;
  if ( step->GetStepLength() == 0. ) return false;

  auto preStep = step->GetPreStepPoint();

  // do not store hits created by particles moving backwards
  auto premom = preStep->GetMomentum();
  if ( m_onlyForward.value() && premom.z() < 0. ) return false;

  // do not store hits created not at the boundary
  if ( m_onlyAtBoundary.value() && preStep->GetStepStatus() != 1 ) return false;

  auto prepos = preStep->GetPosition();
  auto newHit = new MCCollector::Hit();
  newHit->SetEdep( edep );
  newHit->SetEntryPos( prepos );
  newHit->SetMomentum( premom );
  // track info
  int trid = track->GetTrackID();
  newHit->SetTrackID( trid );
  auto ui = track->GetUserInformation();
  auto gi = (GaussTrackInformation*)ui;
  gi->setCreatedHit( true );
  gi->setToBeStored( true );
  gi->addHit( newHit );

  m_col->insert( newHit );
  return true;
}

DECLARE_COMPONENT_WITH_ID( MCCollector::SensDet, "MCCollectorSensDet" )
