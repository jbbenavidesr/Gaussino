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

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// from Geant4
#ifdef CUSTOMSIM
#  include "G4FastHit.hh"
#endif
#include "G4HCofThisEvent.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"

// from Gaussino
#include "MCCollector/MCCollectorSensDet.h"

void MCCollector::SensDet::Initialize( G4HCofThisEvent* HCE ) {

  m_col = new HitsCollection( SensitiveDetectorName, collectionName[0] );

  int HCID = G4SDManager::GetSDMpointer()->GetCollectionID( SensitiveDetectorName + "/" + collectionName[0] );

  HCE->AddHitsCollection( HCID, m_col );

  // standard print left as is
  debug( " Initialize(): CollectionName='" + m_col->GetName() + "' for SensDet='" + m_col->GetSDname() + "'" );
}

MCCollector::Hit* MCCollector::SensDet::RetrieveAndSetupHit( const G4Track* track, G4TouchableHistory* /* history */ ) {
  auto newHit = new MCCollector::Hit();
  // track info
  int trid = track->GetTrackID();
  newHit->SetTrackID( trid );
  auto ui = track->GetUserInformation();
  auto gi = (GaussinoTrackInformation*)ui;
  gi->setCreatedHit( true );
  if ( m_forceStoreTruth ) { gi->setToStoreTruth( true ); }
  gi->addHit( newHit );
  m_col->insert( newHit );
  return newHit;
}

bool MCCollector::SensDet::ProcessHits( G4Step* step, G4TouchableHistory* /* history */ ) {
  if ( 0 == step ) return false;

  auto track = step->GetTrack();

  // If required to have energy deposition check
  double edep = step->GetTotalEnergyDeposit();
  if ( m_requireEDep && edep <= 0.0 ) return false;
  if ( step->GetStepLength() == 0. ) return false;

  auto preStep = step->GetPreStepPoint();

  // do not store hits created by particles moving backwards
  auto premom = preStep->GetMomentum();
  if ( m_onlyForward && premom.z() < 0. ) return false;

  // do not store hits created not at the boundary
  if ( m_onlyAtBoundary && preStep->GetStepStatus() != 1 ) return false;

  auto prepos   = preStep->GetPosition();
  auto histTemp = (G4TouchableHistory*)preStep->GetTouchable();
  auto newHit   = RetrieveAndSetupHit( track, histTemp );
  newHit->SetEdep( edep );
  newHit->SetEntryPos( prepos );
  newHit->SetMomentum( premom );
  newHit->SetExitPos( step->GetPostStepPoint()->GetPosition() );

  double timeof = preStep->GetGlobalTime();

  newHit->SetTimeOfFlight( timeof );
  return true;
}

#ifdef CUSTOMSIM
bool MCCollector::SensDet::ProcessHits( const G4FastHit* fastHit, const G4FastTrack* fastTrack,
                                        G4TouchableHistory* history ) {
  auto edep = fastHit->GetEnergy();
  // If required to have energy deposition check
  if ( m_requireEDep && edep <= 0.0 ) return false;

  auto track = fastTrack->GetPrimaryTrack();
  // do not store hits created by particles moving backwards
  auto premom = track->GetMomentum();
  if ( m_onlyForward && premom.z() < 0. ) return false;

  const HepGeom::Point3D<double>& prepos = fastHit->GetPosition();

  auto newHit = RetrieveAndSetupHit( track, history );
  newHit->SetEdep( edep );
  newHit->SetEntryPos( prepos );
  newHit->SetMomentum( premom );

  return true;
}
#endif

void MCCollector::SensDet::EndOfEvent( G4HCofThisEvent* /* HCE */ ) {
  if ( m_printStats ) {
    int    hits_no = 0;
    double energy  = 0.;

    std::set<int> unique_particles;

    std::vector<MCCollector::Hit*>* hits = m_col->GetVector();
    for ( auto& hit : *hits ) {
      hits_no++;
      energy += hit->GetEdep();
      unique_particles.insert( hit->GetTrackID() );
    }

    always( boost::str( boost::format( "#Hits=%5d Energy=%8.3g[GeV] #Particles=%5d in %s" ) % hits_no %
                        ( energy / Gaudi::Units::GeV ) % unique_particles.size() % m_col->GetSDname() ) );
  }
}
