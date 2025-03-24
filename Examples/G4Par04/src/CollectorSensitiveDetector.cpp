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

// Geant4
#include "G4FastHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VFastSimSensitiveDetector.hh"
#include "G4VSensitiveDetector.hh"

// Gaussino
#include "CollectorG4Hit.h"
#include "EventInformation.h"
#include "GiGaMTCoreMessage/IMessage.h"
#include "GiGaMTCoreRun/GaussinoTrackInformation.h"
#include "GiGaMTDetFactories/GiGaMTG4SensDetFactory.h"

namespace Gaussino::G4Par04 {
  struct CollectorSensDet : public G4VSensitiveDetector,
                            public G4VFastSimSensitiveDetector,
                            public virtual Gsino::Message {
    inline CollectorSensDet( const std::string& name )
        : G4VSensitiveDetector( name ), m_hitsMap( std::make_shared<CollectorG4Hit::Map>() ) {
      collectionName.insert( "Hits" );
    }
    void                     Initialize( G4HCofThisEvent* ) override;
    void                     EndOfEvent( G4HCofThisEvent* ) override;
    virtual bool             ProcessHits( G4Step*, G4TouchableHistory* ) override;
    virtual bool             ProcessHits( const G4FastHit*, const G4FastTrack*, G4TouchableHistory* ) override;
    virtual CollectorG4Hit*  RetrieveAndSetupHit( const G4Track* );
    CollectorHitsCollection* m_col;
    std::shared_ptr<CollectorG4Hit::Map> m_hitsMap;
  };

  using BASE_FACTORY = GiGaMTG4SensDetFactory<CollectorSensDet>;
  struct CollectorSensDetFactory : public BASE_FACTORY {
    using BASE_FACTORY::GiGaMTG4SensDetFactory;
    CollectorSensDet* construct() const override {
      auto sensdet = BASE_FACTORY::construct();
      // sensdet->m_onlyStoredTracks = m_onlyStoredTracks.value();
      return sensdet;
    }
  };
} // namespace Gaussino::G4Par04

DECLARE_COMPONENT( Gaussino::G4Par04::CollectorSensDetFactory )

void Gaussino::G4Par04::CollectorSensDet::Initialize( G4HCofThisEvent* aHCE ) {
  m_col    = new CollectorHitsCollection( SensitiveDetectorName, collectionName[0] );
  int HCID = G4SDManager::GetSDMpointer()->GetCollectionID( SensitiveDetectorName + "/" + collectionName[0] );
  aHCE->AddHitsCollection( HCID, m_col );
}

void Gaussino::G4Par04::CollectorSensDet::EndOfEvent( G4HCofThisEvent* ) { m_hitsMap->clear(); }

Gaussino::G4Par04::CollectorG4Hit* Gaussino::G4Par04::CollectorSensDet::RetrieveAndSetupHit( const G4Track* track ) {

  auto baseEventInfo = EventInformation::Get();
  auto eventInfo     = dynamic_cast<EventInformation*>( baseEventInfo );
  if ( !eventInfo ) {
    eventInfo = new EventInformation( std::move( *baseEventInfo ) );
    delete baseEventInfo;
    G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->SetUserInformation( eventInfo );
    eventInfo->SetCollectorHitsMap( m_hitsMap );
  }

  auto trackInfo = GaussinoTrackInformation::Get();
  int  trackID   = track->GetTrackID();
  int  strackID  = track->GetParentID();
  if ( !trackInfo->prelStoreTruth() && !trackInfo->storeTruth() ) { trackID = strackID; }

  if ( auto result = m_hitsMap->find( trackID ); result != m_hitsMap->end() ) { return nullptr; }

  auto newHit = new CollectorG4Hit();
  newHit->SetKineticEnergy( track->GetKineticEnergy() );
  auto particle = track->GetDynamicParticle();
  newHit->SetPrimaryEnergy( particle->GetTotalEnergy() );
  newHit->SetPDG( particle->GetParticleDefinition()->GetPDGEncoding() );
  newHit->SetDirection( track->GetMomentumDirection() );
  newHit->SetPosition( track->GetPosition() );
  newHit->SetTrackID( trackID );
  trackInfo->setCreatedHit( true );
  trackInfo->addHit( newHit );
  ( *m_hitsMap )[trackID] = newHit;
  m_col->insert( newHit );
  return newHit;
}

bool Gaussino::G4Par04::CollectorSensDet::ProcessHits( G4Step* step, G4TouchableHistory* /* history */ ) {
  if ( !step ) return false;
  auto track = step->GetTrack();
  return RetrieveAndSetupHit( track );
}

bool Gaussino::G4Par04::CollectorSensDet::ProcessHits( const G4FastHit* /* fastHit */, const G4FastTrack* fastTrack,
                                                       G4TouchableHistory* /* history */ ) {
  return RetrieveAndSetupHit( fastTrack->GetPrimaryTrack() );
}
