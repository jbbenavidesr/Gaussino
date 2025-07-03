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

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Vector3DTypes.h"

// Gaussino
// FIXME: change to G4FastSimHitMaker whenever possible
#include "CustomSimulation/HitMaker.h"
#include "CustomSimulation/ModelFactory.h"

namespace Gsino::CaloChallenge::ImmediateDepositWithConditions {
  using MomentumTrigger = std::function<bool( const Gaudi::XYZVector& momentum )>;
  /**
   * @brief Model for immediate deposit of energy in the calorimeter with momentum-based conditions.
   *
   * This model is used to immediately deposit energy in the calorimeter based on the momentum of the particle.
   * It allows for setting conditions on the momentum components to trigger the deposition.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  struct Model : public G4VFastSimulationModel, public Gsino::Message {
    // FIXME: change to G4FastSimHitMaker whenever possible
    std::unique_ptr<Gaussino::CustomSimulation::HitMaker> m_hitMaker;
    std::unique_ptr<MomentumTrigger>                      m_momentumTrigger;

    Model( G4String modelName, G4Region* envelope )
        : G4VFastSimulationModel( modelName, envelope ), m_hitMaker( new Gaussino::CustomSimulation::HitMaker ){};

    inline G4bool IsApplicable( const G4ParticleDefinition& ) override { return true; };
    inline G4bool ModelTrigger( const G4FastTrack& track ) override {
      auto p = track.GetPrimaryTrack()->GetMomentum();
      if ( m_momentumTrigger ) { return m_momentumTrigger->operator()( Gaudi::XYZVector( p ) ); }
      return true;
    };
    void DoIt( const G4FastTrack& aFastTrack, G4FastStep& aFastStep ) override {
      // kill the track so that it will no longer be propagated by G4
      aFastStep.KillPrimaryTrack();
      double edep = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
      auto   pos  = aFastTrack.GetPrimaryTrackLocalPosition();
      // FIXME: change to G4FastSimHitMaker whenever possible
      m_hitMaker->make( G4FastHit( pos, edep ), aFastTrack );
    }
  };

  /**
   * @brief Factory for constructing Model instances with configurable properties.
   *
   * This factory allows for setting conditions on the momentum components to trigger the deposition.
   * It constructs Model instances with the specified momentum conditions.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  class ModelFactory : public Gaussino::CustomSimulation::ModelFactory<Model> {

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

  public:
    using base_class = Gaussino::CustomSimulation::ModelFactory<Model>;
    using base_class::ModelFactory;

    virtual Model* construct() const override {
      auto            model           = base_class::construct();
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
        model->m_momentumTrigger = std::make_unique<MomentumTrigger>( std::move( momentumTrigger ) );
      }
      return model;
    };
  };
} // namespace Gsino::CaloChallenge::ImmediateDepositWithConditions

DECLARE_COMPONENT_WITH_ID( Gsino::CaloChallenge::ImmediateDepositWithConditions::ModelFactory,
                           "ImmediateDepositWithConditionsModel" )
