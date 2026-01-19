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
#include "CalorimeterCollector/CalorimeterCollectorHit.h"
#include "Defaults/Locations.h"
#include "GiGaMTCoreRun/G4EventProxy.h"

// Gaudi
#include "Gaudi/Accumulators/Histogram.h"
#include "Gaudi/FSMCallbackHolder.h"
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/FixTESPath.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace Acc = Gaudi::Accumulators;

namespace CalorimeterCollector {
  using BaseClass_t = Gaudi::Functional::Traits::BaseClass_t<Gaudi::FSMCallbackHolder<FixTESPath<Gaudi::Algorithm>>>;

  class Monitoring : public Gaudi::Functional::Consumer<void( const G4EventProxies& ), BaseClass_t> {
    mutable std::mutex m_lock;

    Gaudi::Property<std::string> m_absorberHC{ this, "AbsorberCollectionName", "AbsorberHits" };
    Gaudi::Property<std::string> m_gapHC{ this, "GapCollectionName", "GapHits" };

    // histogram properties
    Gaudi::Property<double> m_maxGapEnergy{ this, "MaxGapEnergy", 1. * Gaudi::Units::GeV };
    Gaudi::Property<double> m_maxGapLength{ this, "MaxGapLength", 100. * Gaudi::Units::cm };
    Gaudi::Property<double> m_maxAbsorberEnergy{ this, "MaxAbsorberEnergy", 1. * Gaudi::Units::GeV };
    Gaudi::Property<double> m_maxAbsorberLength{ this, "MaxAbsorberLength", 100. * Gaudi::Units::cm };

    // Histograms
    mutable Acc::Histogram<1> m_hGapEnergy{ this, "gapEnergy", "Energy Distribution in the Gaps" };
    mutable Acc::Histogram<1> m_hGapLength{ this, "gapLength", "Total Track Length in the Gaps" };
    mutable Acc::Histogram<1> m_hAbsorberEnergy{ this, "absorberEnergy", "Energy Distribution in Absorbers" };
    mutable Acc::Histogram<1> m_hAbsorberLength{ this, "absorberLength", "Total Track Length in Absorbers" };

  public:
    Monitoring( const std::string& name, ISvcLocator* pSvcLocator )
        : Consumer( name, pSvcLocator, { KeyValue{ "G4EventProxies", Gaussino::G4EventsLocation::Default } } ) {}

    StatusCode initialize() override {
      return Consumer::initialize().andThen( [&]() -> StatusCode {
        m_hGapEnergy.setAxis<0>( { 1024, 0, 1.1 * m_maxGapEnergy / Gaudi::Units::GeV } );
        m_hGapLength.setAxis<0>( { 1024, 0, 1.1 * m_maxGapLength / Gaudi::Units::GeV } );
        m_hAbsorberEnergy.setAxis<0>( { 1024, 0, 1.1 * m_maxAbsorberEnergy / Gaudi::Units::GeV } );
        m_hAbsorberLength.setAxis<0>( { 1024, 0, 1.1 * m_maxAbsorberLength / Gaudi::Units::GeV } );

        return StatusCode::SUCCESS;
      } );
    }

    void operator()( const G4EventProxies& g4event_proxies ) const override {
      std::lock_guard<std::mutex> guard_lock( m_lock );

      for ( const auto& g4event_proxy : g4event_proxies ) {

        auto absoHC = g4event_proxy->GetHitCollection<HitsCollection>( m_absorberHC.value() );
        if ( !absoHC ) {
          throw GaudiException( "Hit collection not found: " + m_absorberHC.value(), name(), StatusCode::FAILURE );
        }

        auto gapHC = g4event_proxy->GetHitCollection<HitsCollection>( m_gapHC.value() );
        if ( !gapHC ) {
          throw GaudiException( "Hit collection not found: " + m_gapHC.value(), name(), StatusCode::FAILURE );
        }

        // Get hit with total values
        auto absoHit = ( *absoHC )[absoHC->entries() - 1];
        auto gapHit  = ( *gapHC )[gapHC->entries() - 1];

        ++m_hAbsorberEnergy[absoHit->GetEdep() / Gaudi::Units::GeV];
        ++m_hAbsorberLength[absoHit->GetTrackLength() / Gaudi::Units::m];
        ++m_hGapEnergy[gapHit->GetEdep() / Gaudi::Units::GeV];
        ++m_hGapLength[gapHit->GetTrackLength() / Gaudi::Units::m];
      }
    };
  };

} // namespace CalorimeterCollector

DECLARE_COMPONENT_WITH_ID( CalorimeterCollector::Monitoring, "CalorimeterMonitoring" )
