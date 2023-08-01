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


// GiGaMT
#include "Defaults/Locations.h"
#include "GiGaMTCoreRun/G4EventProxy.h"
#include "GiGaMTCoreRun/MCTruthConverter.h"
#include "MCTruthToEDM/LinkedParticleMCParticleLink.h"

// local
#include "MCCollector/MCCollectorHit.h"

// LHCb
#include "Event/MCExtendedHit.h"
#include "Event/MCHit.h"

namespace MCCollector {
  class HitsAlg : public Gaudi::Functional::Transformer<LHCb::MCHits( const G4EventProxies&,
                                                                      const LinkedParticleMCParticleLinks& ),
                                                                    Gaudi::Functional::Traits::useLegacyGaudiAlgorithm> {
  public:
    HitsAlg( const std::string& name, ISvcLocator* pSvcLocator )
        : Transformer(
              name, pSvcLocator,
              {KeyValue{"Input", Gaussino::G4EventsLocation::Default},
               KeyValue{"LinkedParticleMCParticleLinks", Gaussino::LinkedParticleMCParticleLinksLocation::Default}},
              KeyValue{"MCHitsLocation", ""} ) {}

    virtual LHCb::MCHits operator()( const G4EventProxies&, const LinkedParticleMCParticleLinks& ) const override;

  protected:
    Gaudi::Property<std::string> m_colName{this, "CollectionName", ""};
  };
} // namespace MCCollector

LHCb::MCHits MCCollector::HitsAlg::operator()( const G4EventProxies&                evtprxs,
                                               const LinkedParticleMCParticleLinks& mclinks ) const {

  LHCb::MCHits hits;
  for ( auto& evtprx : evtprxs ) {
    auto hitCollection = evtprx->GetHitCollection<HitsCollection>( m_colName.value() );

    if ( !hitCollection ) {
      warning() << "The hit collection='" + m_colName + "' is not found!" << endmsg;
      continue;
    }

    int numOfHits = hitCollection->entries();
    for ( int iG4Hit = 0; iG4Hit < numOfHits; ++iG4Hit ) {
      LHCb::MCExtendedHit* newHit = new LHCb::MCExtendedHit();
      auto                 g4Hit  = ( *hitCollection )[iG4Hit];
      Gaudi::XYZPoint      entry( g4Hit->GetEntryPos() );
      Gaudi::XYZVector     mom( g4Hit->GetMomentum() );
      newHit->setMomentum( mom );
      newHit->setEntry( entry );
      newHit->setEnergy( g4Hit->GetEdep() );
      newHit->setP( g4Hit->GetMomentum().mag() );
      int trackID = g4Hit->GetTrackID();
      if ( auto lp = evtprx->truth()->GetParticleFromTrackID( trackID ); lp ) {
        if ( auto it = mclinks.find( lp ); it != std::end( mclinks ) ) {
          newHit->setMCParticle( it->second );
        } else {
          warning() << "No pointer to MCParticle for MCHit associated to G4 trackID: " << trackID << endmsg;
        }
      } else {
        warning() << "No LinkedParticle found. Something went seriously wrong. trackID: " << trackID << endmsg;
      }
      hits.add( newHit );
    }
  }

  return hits;
}

DECLARE_COMPONENT_WITH_ID( MCCollector::HitsAlg, "GetMCCollectorHitsAlg" )
