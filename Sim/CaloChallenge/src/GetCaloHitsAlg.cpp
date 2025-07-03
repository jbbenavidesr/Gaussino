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
#include "CaloHit.h"
#include "Defaults/Locations.h"
#include "EDM/CaloHit.h"
#include "GiGaMTCoreRun/G4EventProxy.h"
#include "GiGaMTCoreRun/MCTruthConverter.h"
#include "MCTruthToEDM/LinkedParticleMCParticleLink.h"

// Gaudi
#include "GaudiAlg/FunctionalUtilities.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/SystemOfUnits.h"

// LHCb
#include "Event/MCHeader.h"

namespace Gsino::CaloChallenge {
  /**
   * @brief Algorithm to retrieve calorimeter hits from Geant4 event proxies.
   *
   * @author Michał Mazurek
   * @date 2023
   *
   */

  class GetCaloHitsAlg
      : public Gaudi::Functional::Transformer<EDM::CaloHits( const LHCb::MCHeader&, const G4EventProxies&,
                                                             const LinkedParticleMCParticleLinks& ),
                                              Gaudi::Functional::Traits::useLegacyGaudiAlgorithm> {
    Gaudi::Property<std::vector<std::string>> m_colNames{ this, "G4HitsCollectionNames", {} };

  public:
    GetCaloHitsAlg( const std::string& name, ISvcLocator* pSvcLocator )
        : Transformer(
              name, pSvcLocator,
              { KeyValue{ "MCHeader", LHCb::MCHeaderLocation::Default },
                KeyValue{ "G4EventProxies", Gaussino::G4EventsLocation::Default },
                KeyValue{ "LinkedParticleMCParticleLinks", Gaussino::LinkedParticleMCParticleLinksLocation::Default } },
              KeyValue{ "OutputHitsLocation", "" } ) {}

    EDM::CaloHits operator()( const LHCb::MCHeader& header, const G4EventProxies& g4event_proxies,
                              const LinkedParticleMCParticleLinks& mclinks ) const override {
      EDM::CaloHits hits        = {};
      size_t        tot_entries = 0;
      auto          eventID     = header.evtNumber();
      for ( const auto& g4event_proxy : g4event_proxies ) {
        for ( const auto& coll : m_colNames.value() ) {
          auto hitColl = g4event_proxy->GetHitCollection<CaloHitsCollection>( coll );
          if ( !hitColl ) { throw GaudiException( "Hit collection not found", name(), StatusCode::FAILURE ); }
          tot_entries += hitColl->entries();
        }
      }
      hits.reserve( tot_entries );
      for ( const auto& g4event_proxy : g4event_proxies ) {
        for ( const auto& coll : m_colNames.value() ) {
          auto hitColl = g4event_proxy->GetHitCollection<CaloHitsCollection>( coll );
          auto entries = hitColl->entries();
          for ( size_t i = 0; i < entries; ++i ) {
            const auto* g4hit = ( *hitColl )[i];
            auto        eDep  = g4hit->GetEdep();
            if ( eDep > 0 ) {
              CaloHitPtr hit( new CaloHit );
              hit->SetEdep( eDep );
              hit->SetZId( g4hit->GetZId() );
              hit->SetRhoId( g4hit->GetRhoId() );
              hit->SetPhiId( g4hit->GetPhiId() );
              hit->SetType( g4hit->GetType() );
              auto trackID = g4hit->GetTrackID();
              hit->SetTrackID( trackID );
              hit->SetEventID( eventID );
              hit->SetTime( g4hit->GetTime() );
              hit->SetPosition( Gaudi::XYZPoint( g4hit->GetPosAbs() ) );
              if ( auto lp = g4event_proxy->truth()->GetParticleFromTrackID( trackID ); lp ) {
                if ( auto it = mclinks.find( lp ); it != std::end( mclinks ) ) {
                  hit->SetMCParticle( it->second );
                } else {
                  warning() << "No pointer to MCParticle for MCHit associated to G4 trackID: " << trackID << endmsg;
                }
              } else {
                warning() << "No LinkedParticle found. Something went seriously wrong. trackID: " << trackID << endmsg;
              }
              hits.push_back( std::move( hit ) );
            }
          }
        }
      }
      return hits;
    };
  };
} // namespace Gsino::CaloChallenge

DECLARE_COMPONENT( Gsino::CaloChallenge::GetCaloHitsAlg )
