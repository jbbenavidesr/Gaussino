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
#include "CalorimeterCollector/CalorimeterCollectorSensDet.h"


void CalorimeterCollector::SensDet::Initialize( G4HCofThisEvent* HCE ) {

  fHitsCollection = new HitsCollection( SensitiveDetectorName, collectionName[0] );

  int HCID = G4SDManager::GetSDMpointer()->GetCollectionID( SensitiveDetectorName + "/" + collectionName[0] );

  HCE->AddHitsCollection( HCID, fHitsCollection );

  // standard print left as is
  debug( " Initialize(): CollectionName='" + fHitsCollection->GetName() + "' for SensDet='" +
         fHitsCollection->GetSDname() + "'" );

  auto eventId = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  CalorimeterCollector::Hit* hit;

  // Create hits for each layer
  for (G4int i = 0; i < fNofCells; i++) {
    hit = new CalorimeterCollector::Hit();
    hit->SetLayerNumber(i);
    hit->SetEventID(eventId);
    hit->SetDetectorName(SensitiveDetectorName);
    fHitsCollection->insert(hit);
  }

  // Create the hit to store totals
  CalorimeterCollector::Hit* hitTotal = new CalorimeterCollector::Hit();
  hitTotal->SetEventID(eventId);
  hitTotal->SetDetectorName(SensitiveDetectorName);
  fHitsCollection->insert(hitTotal);
}

bool CalorimeterCollector::SensDet::ProcessHits( G4Step* step, G4TouchableHistory* /* history */ ) {
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  // step length
  G4double stepLength = 0.;
  if (step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) {
    stepLength = step->GetStepLength();
  }

  if (edep == 0. && stepLength == 0.) return false;

  auto touchable = (step->GetPreStepPoint()->GetTouchable());

  // Get calorimeter cell id
  auto layerNumber = touchable->GetCopyNumber(1);

  // Get hit accounting data for this cell
  auto hit = (*fHitsCollection)[layerNumber];
  if (!hit) {
    G4ExceptionDescription msg;
    msg << "Cannot access hit " << layerNumber;
    G4Exception("CalorimeterSD::ProcessHits()", "MyCode0004", FatalException, msg);
  }

  // Get hit for total accounting
  auto hitTotal = (*fHitsCollection)[fHitsCollection->entries() - 1];

  // Add values
  hit->Add(edep, stepLength);
  hitTotal->Add(edep, stepLength);

  return true;

}

void CalorimeterCollector::SensDet::EndOfEvent( G4HCofThisEvent* /* HCE */ ) {
  auto nofHits = fHitsCollection->entries();
  G4cout << G4endl << "-------->Hits Collection: in this event they are " << nofHits
    << " hits in the tracker chambers: " << G4endl;
  for (std::size_t i = 0; i < nofHits; ++i)
    (*fHitsCollection)[i]->Print();

}
