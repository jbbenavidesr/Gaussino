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

#include <functional>

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
#include "GiGaMTCoreRun/ZMaxPlane.h"
#include "GiGaMTDetFactories/GiGaMTG4SensDetFactory.h"

namespace Gsino::CaloChallenge {

  using MomentumTrigger = std::function<bool( const Gaudi::XYZVector& momentum )>;

  /**
   * @brief Sensitive detector for collecting calorimeter hits in the Gaussino CaloChallenge.
   *
   * @author Michał Mazurek
   * @date 2023
   *
   */
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

    // triggering plane
    std::unique_ptr<ZMaxPlane> m_plane;
    bool                       m_customTriggerPlane = false;

    // extra triggering conditions / cuts
    std::unique_ptr<MomentumTrigger> m_momentumTrigger;
  };

  using BASE_FACTORY = GiGaMTG4SensDetFactory<CollectorSensDet>;

  /**
   * @brief Factory for constructing CollectorSensDet instances with configurable properties.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  struct CollectorSensDetFactory : public BASE_FACTORY {
    using BASE_FACTORY::GiGaMTG4SensDetFactory;
    CollectorSensDet* construct() const override {
      auto sensdet = BASE_FACTORY::construct();
      if ( m_customTriggerPlane.value() ) {
        sensdet->m_plane = std::make_unique<ZMaxPlane>();
        sensdet->m_plane->prepare( m_triggerPlaneZ.value(), m_triggerPlaneTilt.value(), m_triggerPlaneYShift.value() );
        sensdet->m_customTriggerPlane = m_customTriggerPlane.value();
      }

      MomentumTrigger momentumTrigger = []( const Gaudi::XYZVector& ) { return true; };
      if ( m_useTotalMomentumCuts ) {
        momentumTrigger = [&, momentumTrigger]( const Gaudi::XYZVector& momentum ) {
          auto mag = std::sqrt( momentum.Mag2() );
          return momentumTrigger( momentum ) && mag > m_minTotalMomentum && mag < m_maxTotalMomentum;
        };
      }
      if ( m_useMomentumXCuts ) {
        momentumTrigger = [&, momentumTrigger]( const Gaudi::XYZVector& momentum ) {
          return momentumTrigger( momentum ) && momentum.x() > m_minMomentumX && momentum.x() < m_maxMomentumX;
        };
      }
      if ( m_useMomentumYCuts ) {
        momentumTrigger = [&, momentumTrigger]( const Gaudi::XYZVector& momentum ) {
          return momentumTrigger( momentum ) && momentum.y() > m_minMomentumY && momentum.y() < m_maxMomentumY;
        };
      }
      if ( m_useMomentumZCuts ) {
        momentumTrigger = [&, momentumTrigger]( const Gaudi::XYZVector& momentum ) {
          return momentumTrigger( momentum ) && momentum.z() > m_minMomentumZ && momentum.z() < m_maxMomentumZ;
        };
      }
      if ( m_useTotalMomentumCuts || m_useMomentumXCuts || m_useMomentumYCuts || m_useMomentumZCuts ) {
        sensdet->m_momentumTrigger = std::make_unique<MomentumTrigger>( std::move( momentumTrigger ) );
      }
      return sensdet;
    }

  private:
    // optional properties of the triggering plane
    // (if the trigger plane is different from the collector plane)
    // warning: works only with planar calorimeters
    Gaudi::Property<bool>   m_customTriggerPlane{ this, "UseCustomTriggerPlane", false };
    Gaudi::Property<double> m_triggerPlaneZ{ this, "TriggerPlaneZ", 0.0 * Gaudi::Units::mm };
    Gaudi::Property<double> m_triggerPlaneTilt{ this, "TriggerPlaneTilt", 0.0 * Gaudi::Units::deg };
    Gaudi::Property<double> m_triggerPlaneYShift{ this, "TriggerPlaneYShift", 0.0 * Gaudi::Units::mm };
    // optional conditions of the particles to trigger on
    // (PIDs should be steered from the parallel world's physics)
    Gaudi::Property<bool>   m_useTotalMomentumCuts{ this, "UseTotalMomentumCuts", false };
    Gaudi::Property<double> m_minTotalMomentum{ this, "MinTotalMomentum", 0.0 * Gaudi::Units::MeV };
    Gaudi::Property<double> m_maxTotalMomentum{ this, "MaxTotalMomentum", 0.0 * Gaudi::Units::MeV };
    Gaudi::Property<bool>   m_useMomentumXCuts{ this, "UseMomentumXCuts", false };
    Gaudi::Property<double> m_minMomentumX{ this, "MinMomentumX", 0.0 * Gaudi::Units::MeV };
    Gaudi::Property<double> m_maxMomentumX{ this, "MaxMomentumX", 0.0 * Gaudi::Units::MeV };
    Gaudi::Property<bool>   m_useMomentumYCuts{ this, "UseMomentumYCuts", false };
    Gaudi::Property<double> m_minMomentumY{ this, "MinMomentumY", 0.0 * Gaudi::Units::MeV };
    Gaudi::Property<double> m_maxMomentumY{ this, "MaxMomentumY", 0.0 * Gaudi::Units::MeV };
    Gaudi::Property<bool>   m_useMomentumZCuts{ this, "UseMomentumZCuts", false };
    Gaudi::Property<double> m_minMomentumZ{ this, "MinMomentumZ", 0.0 * Gaudi::Units::MeV };
    Gaudi::Property<double> m_maxMomentumZ{ this, "MaxMomentumZ", 0.0 * Gaudi::Units::MeV };
  };
} // namespace Gsino::CaloChallenge

DECLARE_COMPONENT( Gsino::CaloChallenge::CollectorSensDetFactory )

void Gsino::CaloChallenge::CollectorSensDet::Initialize( G4HCofThisEvent* aHCE ) {
  m_col    = new CollectorHitsCollection( SensitiveDetectorName, collectionName[0] );
  int HCID = G4SDManager::GetSDMpointer()->GetCollectionID( SensitiveDetectorName + "/" + collectionName[0] );
  aHCE->AddHitsCollection( HCID, m_col );
}

void Gsino::CaloChallenge::CollectorSensDet::EndOfEvent( G4HCofThisEvent* ) { m_hitsMap->clear(); }

Gsino::CaloChallenge::CollectorG4Hit*
Gsino::CaloChallenge::CollectorSensDet::RetrieveAndSetupHit( const G4Track* track ) {
  auto momentum = track->GetMomentum();

  if ( m_momentumTrigger ) {
    if ( !m_momentumTrigger->operator()( Gaudi::XYZVector( momentum ) ) ) { return nullptr; }
  }

  auto baseEventInfo = EventInformation::Get();
  auto eventInfo     = dynamic_cast<EventInformation*>( baseEventInfo );
  if ( !eventInfo ) {
    eventInfo = new EventInformation( std::move( *baseEventInfo ) );
    delete baseEventInfo;
    G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->SetUserInformation( eventInfo );
    eventInfo->SetCollectorHitsMap( m_hitsMap );
  }

  auto trackInfo = GaussinoTrackInformation::Get( track );
  int  trackID   = track->GetTrackID();
  int  strackID  = track->GetParentID();
  if ( !trackInfo->prelStoreTruth() && !trackInfo->storeTruth() ) { trackID = strackID; }

  if ( auto result = m_hitsMap->find( trackID ); result != m_hitsMap->end() ) { return nullptr; }

  auto newHit = new CollectorG4Hit();
  newHit->SetKineticEnergy( track->GetKineticEnergy() );
  auto particle = track->GetDynamicParticle();
  newHit->SetPrimaryEnergy( particle->GetTotalEnergy() );
  newHit->SetPDG( particle->GetParticleDefinition()->GetPDGEncoding() );
  auto direction = track->GetMomentumDirection();
  newHit->SetDirection( direction );
  newHit->SetMomentum( momentum );
  auto position = track->GetPosition();
  if ( m_customTriggerPlane ) {
    auto dist = m_plane->DistanceAlongDirection( Gaudi::XYZPoint( position ), Gaudi::XYZVector( direction ) );
    position  = position - direction * dist;
  }
  newHit->SetPosition( position );
  newHit->SetTrackID( trackID );
  newHit->SetTime( track->GetGlobalTime() );
  trackInfo->setCreatedHit( true );
  trackInfo->addHit( newHit );
  ( *m_hitsMap )[trackID] = newHit;
  m_col->insert( newHit );
  return newHit;
}

bool Gsino::CaloChallenge::CollectorSensDet::ProcessHits( G4Step* step, G4TouchableHistory* /* history */ ) {
  if ( !step ) return false;
  auto track = step->GetTrack();
  return RetrieveAndSetupHit( track );
}

bool Gsino::CaloChallenge::CollectorSensDet::ProcessHits( const G4FastHit* /* fastHit */, const G4FastTrack* fastTrack,
                                                          G4TouchableHistory* /* history */ ) {
  return RetrieveAndSetupHit( fastTrack->GetPrimaryTrack() );
}
