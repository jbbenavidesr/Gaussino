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

// from Geant4
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

// from Gaussino
#include "SimpleCollector/SimpleCollectorSensDet.h"

void SimpleCollector::SensDet::Initialize( G4HCofThisEvent* HCE ) {

  fHitsCollection = new HitsCollection( SensitiveDetectorName, collectionName[0] );

  int HCID = G4SDManager::GetSDMpointer()->GetCollectionID( SensitiveDetectorName + "/" + collectionName[0] );

  HCE->AddHitsCollection( HCID, fHitsCollection );

  // standard print left as is
  debug( " Initialize(): CollectionName='" + fHitsCollection->GetName() + "' for SensDet='" +
         fHitsCollection->GetSDname() + "'" );
}

bool SimpleCollector::SensDet::ProcessHits( G4Step* step, G4TouchableHistory* /* history */ ) {
  // energy deposit
  G4double edep = step->GetTotalEnergyDeposit();

  if ( edep == 0. ) return false;

  SimpleCollector::Hit* newHit = new SimpleCollector::Hit();

  newHit->SetTrackID( step->GetTrack()->GetTrackID() );
  newHit->SetChamberNb( step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber() );
  newHit->SetEdep( edep );
  newHit->SetPos( step->GetPostStepPoint()->GetPosition() );

  fHitsCollection->insert( newHit );

  return true;
}

void SimpleCollector::SensDet::EndOfEvent( G4HCofThisEvent* /* HCE */ ) {
  int    hits_no = 0;
  double energy  = 0.;

  std::set<int> unique_particles;

  std::vector<SimpleCollector::Hit*>* hits = fHitsCollection->GetVector();
  for ( auto& hit : *hits ) {
    hits_no++;
    energy += hit->GetEdep();
    unique_particles.insert( hit->GetTrackID() );
  }

  auto eventId = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

  always( boost::str( boost::format( "#Hits=%5d Energy=%8.3g[GeV] #Particles=%5d in %s for event with id: %5d" ) % hits_no %
                      ( energy / Gaudi::Units::GeV ) % unique_particles.size() % fHitsCollection->GetSDname() % eventId) );
}
