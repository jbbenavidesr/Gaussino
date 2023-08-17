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
#include "GiGaMTCoreRun/G4EventProxy.h"

// Gaudi
#include "GaudiAlg/FunctionalUtilities.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/SystemOfUnits.h"

// LHCb
#include "Event/MCHeader.h"

namespace Gaussino::G4Par04 {
  class GetCaloHitsAlg : public Gaudi::Functional::Transformer<CaloHits( const LHCb::MCHeader&, const G4EventProxies& ),
                                                               Gaudi::Functional::Traits::useLegacyGaudiAlgorithm> {
    Gaudi::Property<std::vector<std::string>> m_colNames{ this, "G4HitsCollectionNames", {} };

  public:
    GetCaloHitsAlg( const std::string& name, ISvcLocator* pSvcLocator )
        : Transformer( name, pSvcLocator,
                       { KeyValue{ "MCHeader", LHCb::MCHeaderLocation::Default },
                         KeyValue{ "G4EventProxies", Gaussino::G4EventsLocation::Default } },
                       KeyValue{ "OutputHitsLocation", "" } ) {}

    CaloHits operator()( const LHCb::MCHeader& header, const G4EventProxies& g4event_proxies ) const override {
      CaloHits hits        = {};
      size_t   tot_entries = 0;
      auto     eventID     = header.evtNumber();
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
              hit->SetTrackID( g4hit->GetTrackID() );
              hit->SetEventID( eventID );
              hits.push_back( std::move( hit ) );
            }
          }
        }
      }
      return hits;
    };
  };
} // namespace Gaussino::G4Par04

DECLARE_COMPONENT( Gaussino::G4Par04::GetCaloHitsAlg )
