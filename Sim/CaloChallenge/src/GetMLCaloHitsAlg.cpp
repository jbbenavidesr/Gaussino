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

// CLHEP
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Vector/Rotation.h"

// Gaudi
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/VectorsAsProperty.h"

// Gaussino
#include "CaloHit.h"
#include "CollectorHit.h"
#include "Defaults/Locations.h"
#include "EDM/CaloHit.h"
#include "IMLModel.h"
#include "NewRnd/RndAlgSeeder.h"
#include "NewRnd/RndGlobal.h"

namespace Gsino::CaloChallenge {
  /**
   * @brief Algorithm to retrieve ML-based calorimeter hits from Geant4 event proxies.
   *
   * This algorithm processes input collector hits and generates output calorimeter hits
   * by running a machine learning model inference on the collector hits.
   *
   * @author Michał Mazurek
   * @date 2023
   */

  class GetMLCaloHitsAlg : public Gaudi::Functional::Transformer<EDM::CaloHits( const EDM::Hits& ),
                                                                 Gaudi::Functional::Traits::BaseClass_t<RndAlgSeeder>> {

    Gaudi::Property<Gaudi::XYZVector> m_meshSize{
        this, "MeshSize", { 2.325 * Gaudi::Units::mm, 1, 3.4 * Gaudi::Units::mm } };
    Gaudi::Property<Gaudi::XYZVector> m_meshNumber{ this, "MeshNumber", { 18, 50, 45 } };

    ToolHandle<IMLModel> m_model{ this, "ModelName", "" };

  public:
    GetMLCaloHitsAlg( const std::string& name, ISvcLocator* pSvcLocator )
        : Transformer( name, pSvcLocator, { KeyValue{ "CollectorHitsLocation", "" } },
                       KeyValue{ "OutputHitsLocation", "" } ) {}

    virtual StatusCode initialize() override {
      return Transformer::initialize().andThen( [&]() {
        m_model->setMeshNumber( m_meshNumber.value() );
        return StatusCode::SUCCESS;
      } );
    }

    EDM::CaloHits operator()( const EDM::Hits& collHits ) const override {
      // reuse the same random engine for all pseudo-random numbers
      // in ML model inference
      auto                     engine = createRndmEngine();
      ThreadLocalEngine::Guard guard( engine );
      EDM::CaloHits            hits = {};

      // initialize the vector of shower energies to 0
      std::vector<float> energies( m_meshNumber.value().x() * m_meshNumber.value().y() * m_meshNumber.value().z(),
                                   0.0 );

      // loop over the collector hits, run inference, fill the shower energies
      // and create the calo hits
      for ( const auto& tcollHit : collHits ) {
        auto collHit = std::dynamic_pointer_cast<CollectorHit>( tcollHit );
        // run ML inference
        m_model->fillShower( energies, engine.get(), collHit->GetKineticEnergy(), collHit->GetPDG(),
                             collHit->GetTheta(), collHit->GetPhi() );
        // Calculate rotation matrix along the particle momentum direction
        // It will rotate the shower axes to match the incoming particle direction
        auto   rotMatrix     = CLHEP::HepRotation();
        auto   direction     = collHit->GetDirection();
        auto   tpos0         = collHit->GetPosition();
        auto   pos0          = CLHEP::Hep3Vector( tpos0.x(), tpos0.y(), tpos0.z() );
        double particleTheta = direction.theta();
        double particlePhi   = direction.phi();
        rotMatrix.rotateZ( -particlePhi );
        rotMatrix.rotateY( -particleTheta );
        auto rotMatrixInv = CLHEP::inverseOf( rotMatrix );

        int cpt     = 0;
        int trackID = collHit->GetTrackID();
        int eventID = collHit->GetEventID();
        for ( size_t iCellR = 0; iCellR < m_meshNumber.value().x(); iCellR++ ) {
          for ( size_t iCellPhi = 0; iCellPhi < m_meshNumber.value().y(); iCellPhi++ ) {
            for ( size_t iCellZ = 0; iCellZ < m_meshNumber.value().z(); iCellZ++ ) {
              auto energy = energies[cpt];
              if ( energy > 0.0 ) {
                auto rhoComp  = ( iCellR + 0.5 ) * m_meshSize.value().x();
                auto phiComp  = ( iCellPhi + 0.5 ) * 2 * Gaudi::Units::pi / m_meshNumber.value().y() - Gaudi::Units::pi;
                auto zComp    = ( iCellZ + 0.5 ) * m_meshSize.value().z();
                auto position = pos0 + rotMatrixInv * CLHEP::Hep3Vector( rhoComp * std::cos( phiComp ),
                                                                         rhoComp * std::sin( phiComp ), zComp );
                CaloHitPtr hit( new CaloHit );
                hit->SetEdep( energy );
                hit->SetZId( iCellZ );
                hit->SetRhoId( iCellR );
                hit->SetPhiId( iCellPhi );
                hit->SetType( true );
                hit->SetTrackID( trackID );
                hit->SetEventID( eventID );
                hit->SetPosition( Gaudi::XYZPoint( position ) );
                hit->SetTime( collHit->GetTime() );
                hit->SetMCParticle( collHit->GetMCParticle() );
                hits.push_back( std::move( hit ) );
              }
              cpt++;
            }
          }
        }
        std::fill( energies.begin(), energies.end(), 0.0 );
      }
      return hits;
    };
  };
} // namespace Gsino::CaloChallenge

DECLARE_COMPONENT( Gsino::CaloChallenge::GetMLCaloHitsAlg )
