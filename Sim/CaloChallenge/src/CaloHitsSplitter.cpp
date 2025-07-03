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

#include "CLHEP/Vector/Rotation.h"

// Gaussino
#include "CaloHit.h"
#include "CollectorHit.h"

// Gaudi
#include "GaudiAlg/FunctionalUtilities.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/PhysicalConstants.h"

namespace Gsino::CaloChallenge {
  /**
   * @brief A transformer algorithm that splits calorimeter hits into
   *        subcells based on specified configurations.
   *
   * This algorithm processes input calorimeter hits and collector hits, and generates output calorimeter hits
   * by dividing the input hits into smaller subcells. The division is based on configurable properties
   * such as the size and number of subcells in the rho, phi, and z directions
   * - Input hits are split into subcells using the specified number of subcells in each direction.
   * - The position, energy deposition, time, and associated Monte Carlo particle information are calculated
   *   for each subcell and stored in the output hits.
   * - Collector hits are used to determine the entrance direction and position for transforming the subcell positions.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  class CaloHitsSplitter
      : public Gaudi::Functional::Transformer<EDM::CaloHits( const EDM::CaloHits&, const EDM::Hits& collHits ),
                                              Gaudi::Functional::Traits::useLegacyGaudiAlgorithm> {

    Gaudi::Property<double> m_sizeOfRhoCells{ this, "SizeOfRhoCells", 2.325 * Gaudi::Units::mm };
    Gaudi::Property<double> m_sizeOfZCells{ this, "SizeOfZCells", 3.4 * Gaudi::Units::mm };
    Gaudi::Property<size_t> m_nbOfPhiCells{ this, "NbOfPhiCells", 50 };
    // Subcells
    Gaudi::Property<size_t> m_nbOfRhoSubCells{ this, "NbOfRhoSubCells", 1 };
    Gaudi::Property<size_t> m_nbOfPhiSubCells{ this, "NbOfPhiSubCells", 1 };
    Gaudi::Property<size_t> m_nbOfZSubCells{ this, "NbOfZSubCells", 1 };

  public:
    CaloHitsSplitter( const std::string& name, ISvcLocator* pSvcLocator )
        : Transformer( name, pSvcLocator,
                       { KeyValue{ "InputCaloHitsLocation", "" }, KeyValue{ "InputCollectorHitsLocation", "" } },
                       KeyValue{ "OutputCaloHitsLocation", "" } ) {}

    EDM::CaloHits operator()( const EDM::CaloHits& inputHits, const EDM::Hits& collHits ) const override {
      EDM::CaloHits outputHits     = {};
      auto          sizeOfPhiCells = 2. * Gaudi::Units::pi / m_nbOfPhiCells.value();

      std::map<int, CollectorHit*> collMap = {};
      for ( const auto& tcollHit : collHits ) {
        auto collHit                   = std::dynamic_pointer_cast<CollectorHit>( tcollHit );
        collMap[collHit->GetTrackID()] = collHit.get();
      }

      for ( const auto& tinputHit : inputHits ) {
        auto inputHit          = std::dynamic_pointer_cast<CaloHit>( tinputHit );
        auto collHit           = collMap[inputHit->GetTrackID()];
        auto entranceDirection = collHit->GetDirection();
        auto tentrancePosition = collHit->GetPosition();
        auto entrancePosition =
            CLHEP::Hep3Vector( tentrancePosition.x(), tentrancePosition.y(), tentrancePosition.z() );
        auto rotMatrix = CLHEP::HepRotation();
        rotMatrix.rotateZ( -entranceDirection.phi() );
        rotMatrix.rotateY( -entranceDirection.theta() );
        auto rotMatrixInv = CLHEP::inverseOf( rotMatrix );
        auto rhoNo        = inputHit->GetRhoId();
        auto phiNo        = inputHit->GetPhiId();
        auto zNo          = inputHit->GetZId();
        auto subEdep      = inputHit->GetEdep() / ( m_nbOfRhoSubCells * m_nbOfPhiSubCells * m_nbOfZSubCells );
        auto time         = inputHit->GetTime();
        auto mcparticle   = inputHit->GetMCParticle();
        for ( size_t rhoSubID = 0; rhoSubID < m_nbOfRhoSubCells; ++rhoSubID ) {
          for ( size_t phiSubID = 0; phiSubID < m_nbOfPhiSubCells; ++phiSubID ) {
            for ( size_t zSubID = 0; zSubID < m_nbOfZSubCells; ++zSubID ) {
              EDM::CaloHitPtr hit( new EDM::CaloHit );
              auto            rhoComp = ( rhoNo + ( rhoSubID + 0.5 ) / m_nbOfRhoSubCells ) * m_sizeOfRhoCells;
              auto phiComp = ( phiNo + ( phiSubID + 0.5 ) / m_nbOfPhiSubCells ) * sizeOfPhiCells - Gaudi::Units::pi;
              auto zComp   = ( zNo + ( zSubID + 0.5 ) / m_nbOfZSubCells ) * m_sizeOfZCells;
              auto subPos  = entrancePosition + rotMatrixInv * CLHEP::Hep3Vector( rhoComp * std::cos( phiComp ),
                                                                                  rhoComp * std::sin( phiComp ), zComp );
              hit->SetPosition( Gaudi::XYZPoint( subPos ) );
              hit->SetEdep( subEdep );
              hit->SetTime( time );
              hit->SetMCParticle( mcparticle );
              outputHits.push_back( hit );
            }
          }
        }
      }
      return outputHits;
    };
  };
} // namespace Gsino::CaloChallenge

DECLARE_COMPONENT( Gsino::CaloChallenge::CaloHitsSplitter )
