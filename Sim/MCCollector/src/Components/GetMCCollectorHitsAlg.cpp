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

// Gaudi
#include "GaudiAlg/FunctionalUtilities.h"
#include "GaudiAlg/Transformer.h"

// Gaussino
#include "Defaults/Locations.h"
#include "EDM/Hit.h"
#include "GiGaMTCoreRun/G4EventProxy.h"
#include "GiGaMTCoreRun/MCTruthConverter.h"
#include "MCTruthToEDM/LinkedParticleMCParticleLink.h"

// local
#include "MCCollector/MCCollectorHit.h"

namespace MCCollector {
  class HitsAlg : public Gaudi::Functional::Transformer<Gsino::EDM::Hits( const G4EventProxies&,
                                                                          const LinkedParticleMCParticleLinks& ),
                                                        Gaudi::Functional::Traits::useLegacyGaudiAlgorithm> {
  public:
    HitsAlg( const std::string& name, ISvcLocator* pSvcLocator )
        : Transformer(
              name, pSvcLocator,
              { KeyValue{ "Input", Gaussino::G4EventsLocation::Default },
                KeyValue{ "LinkedParticleMCParticleLinks", Gaussino::LinkedParticleMCParticleLinksLocation::Default } },
              KeyValue{ "MCHitsLocation", "" } ) {}

    virtual Gsino::EDM::Hits operator()( const G4EventProxies&, const LinkedParticleMCParticleLinks& ) const override;

  protected:
    Gaudi::Property<std::string> m_colName{ this, "CollectionName", "" };
  };
} // namespace MCCollector

Gsino::EDM::Hits MCCollector::HitsAlg::operator()( const G4EventProxies&                evtprxs,
                                                   const LinkedParticleMCParticleLinks& mclinks ) const {
  Gsino::EDM::Hits hits;
  for ( auto& evtprx : evtprxs ) {
    auto hitCollection = evtprx->GetHitCollection<HitsCollection>( m_colName.value() );

    if ( !hitCollection ) {
      warning() << "The hit collection='" + m_colName + "' is not found!" << endmsg;
      continue;
    }

    int numOfHits = hitCollection->entries();
    for ( int iG4Hit = 0; iG4Hit < numOfHits; ++iG4Hit ) {
      auto             newHit = Gsino::EDM::HitPtr( new Gsino::EDM::Hit() );
      auto             g4Hit  = ( *hitCollection )[iG4Hit];
      Gaudi::XYZPoint  entry( g4Hit->GetEntryPos() );
      Gaudi::XYZVector mom( g4Hit->GetMomentum() );
      newHit->SetMomentum( mom );
      newHit->SetPosition( entry );
      newHit->SetEnergy( g4Hit->GetEdep() );
      newHit->SetTime( g4Hit->GetTimeOfFlight() );
      int trackID = g4Hit->GetTrackID();
      if ( auto lp = evtprx->truth()->GetParticleFromTrackID( trackID ); lp ) {
        if ( auto it = mclinks.find( lp ); it != std::end( mclinks ) ) {
          newHit->SetMCParticle( it->second );
        } else {
          warning() << "No pointer to MCParticle for MCHit associated to G4 trackID: " << trackID << endmsg;
        }
      } else {
        warning() << "No LinkedParticle found. Something went seriously wrong. trackID: " << trackID << endmsg;
      }
      hits.emplace_back( newHit );
    }
  }

  return hits;
}

DECLARE_COMPONENT_WITH_ID( MCCollector::HitsAlg, "GetMCCollectorHitsAlg" )
