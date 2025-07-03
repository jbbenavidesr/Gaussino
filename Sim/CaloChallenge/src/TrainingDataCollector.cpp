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

// Gaudi
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/GaudiTupleAlg.h"

// Gaussino
#include "CaloHit.h"
#include "CollectorHit.h"

namespace Gsino::CaloChallenge {
  using BaseClass_t = Gaudi::Functional::Traits::BaseClass_t<GaudiTupleAlg>;

  /**
   * @brief Algorithm to collect training data for calorimeter hits.
   *
   * This algorithm collects calorimeter hits and collector hits, storing them in
   * NTuples for further analysis or training of machine learning models.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  class TrainingDataCollector
      : public Gaudi::Functional::Consumer<void( const EDM::Hits&, const EDM::CaloHits& ), BaseClass_t> {
    mutable std::mutex m_lock;

    Gaudi::Property<std::string> m_caloHitsTupleName{ this, "CaloHitsTupleName", "CaloHits" };
    Gaudi::Property<std::string> m_collectorHitsTupleName{ this, "CollectorHitsTupleName", "CollectorHits" };

  public:
    TrainingDataCollector( const std::string& name, ISvcLocator* pSvcLocator )
        : Consumer( name, pSvcLocator,
                    { KeyValue{ "CollectorHitsLocation", "" }, KeyValue{ "CaloHitsLocation", "" } } ) {}
    void operator()( const EDM::Hits& collhits, const EDM::CaloHits& caloHits ) const override {
      std::lock_guard<std::mutex> guard_lock( m_lock );

      Tuple collTuple = this->nTuple( m_collectorHitsTupleName.value() );
      for ( const auto& thit : collhits ) {
        auto hit = std::dynamic_pointer_cast<CollectorHit>( thit );
        collTuple->column( "KineticEnergy", hit->GetKineticEnergy() ).ignore();
        collTuple->column( "PrimaryEnergy", hit->GetPrimaryEnergy() ).ignore();
        collTuple->column( "Theta", hit->GetTheta() ).ignore();
        collTuple->column( "Phi", hit->GetPhi() ).ignore();
        collTuple->column( "PDG", hit->GetPDG() ).ignore();
        collTuple->column( "TrackID", hit->GetTrackID() ).ignore();
        collTuple->column( "EventID", hit->GetEventID() ).ignore();
        auto sc = collTuple->write();
        if ( sc.isFailure() ) { throw GaudiException( "NTuple not wrtitten!", this->name(), StatusCode::FAILURE ); }
      }

      Tuple caloTuple = this->nTuple( m_caloHitsTupleName.value() );
      for ( const auto& thit : caloHits ) {
        auto hit = std::dynamic_pointer_cast<CaloHit>( thit );
        caloTuple->column( "RhoID", hit->GetRhoId() ).ignore();
        caloTuple->column( "ZID", hit->GetZId() ).ignore();
        caloTuple->column( "PhiID", hit->GetPhiId() ).ignore();
        caloTuple->column( "EDep", hit->GetEdep() ).ignore();
        caloTuple->column( "TrackID", hit->GetTrackID() ).ignore();
        caloTuple->column( "EventID", hit->GetEventID() ).ignore();
        auto pos = hit->GetPosition();
        caloTuple->column( "X", pos.x() ).ignore();
        caloTuple->column( "Y", pos.y() ).ignore();
        caloTuple->column( "Z", pos.z() ).ignore();
        auto sc = caloTuple->write();
        if ( sc.isFailure() ) { throw GaudiException( "NTuple not wrtitten!", this->name(), StatusCode::FAILURE ); }
      }
    };
  };
} // namespace Gsino::CaloChallenge

DECLARE_COMPONENT( Gsino::CaloChallenge::TrainingDataCollector )
