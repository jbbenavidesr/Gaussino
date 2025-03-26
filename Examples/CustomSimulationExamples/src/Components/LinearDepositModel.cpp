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
#include "GaudiKernel/SystemOfUnits.h"

// Gaussino
// FIXME: change to G4FastSimHitMaker whenever possible
#include "CustomSimulation/HitMaker.h"
#include "CustomSimulation/ModelFactory.h"

namespace Gaussino::CustomSimulation::LinearDeposit {

  class Model : public G4VFastSimulationModel, public Gsino::Message {

    std::unique_ptr<HitMaker> m_hitMaker;
    double                    m_eDepFrac{ 1.0 };
    double                    m_eKillThr{ 50.0 * Gaudi::Units::MeV };
    double                    m_stepLength{ 0. * Gaudi::Units::mm };

  public:
    Model( G4String modelName, G4Region* envelope );

    inline G4bool IsApplicable( const G4ParticleDefinition& ) override { return true; };
    inline G4bool ModelTrigger( const G4FastTrack& ) override { return true; };
    void          DoIt( const G4FastTrack& aFastTrack, G4FastStep& aFastStep ) override;

    inline void SetEDepFrac( double eDepFrac ) { m_eDepFrac = eDepFrac; };
    inline void SetEKillThr( double eKillThr ) { m_eKillThr = eKillThr; };
    inline void SetStepLength( double stepLength ) { m_stepLength = stepLength; };
  };

  class ModelFactory : public CustomSimulation::ModelFactory<Model> {

    Gaudi::Property<double> m_eDepFrac{ this, "EnergyDepositFraction", 1.0 };
    Gaudi::Property<double> m_eKillThr{ this, "KillEnergyThreshold", 50. * Gaudi::Units::MeV };
    Gaudi::Property<double> m_stepLength{ this, "FakeStepLength", 0. * Gaudi::Units::mm };

  public:
    using base_class = CustomSimulation::ModelFactory<Model>;
    using base_class::ModelFactory;

    virtual Model* construct() const override {
      auto model = base_class::construct();
      model->SetEDepFrac( m_eDepFrac );
      model->SetEKillThr( m_eKillThr );
      model->SetStepLength( m_stepLength );
      return model;
    };
  };
} // namespace Gaussino::CustomSimulation::LinearDeposit

DECLARE_COMPONENT_WITH_ID( Gaussino::CustomSimulation::LinearDeposit::ModelFactory, "LinearDepositModel" )

Gaussino::CustomSimulation::LinearDeposit::Model::Model( G4String modelName, G4Region* envelope )
    : G4VFastSimulationModel( modelName, envelope ), m_hitMaker( new Gaussino::CustomSimulation::HitMaker ) {}

void Gaussino::CustomSimulation::LinearDeposit::Model::DoIt( const G4FastTrack& aFastTrack, G4FastStep& aFastStep ) {
  // kill the track so that it will no longer be propagated by G4
  aFastStep.KillPrimaryTrack();
  double initialEnergy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
  auto   position      = aFastTrack.GetPrimaryTrack()->GetPosition();
  auto   rot           = G4RotationMatrix();
  rot.rotateY( position.getTheta() );

  if ( !( std::fabs( position.x() ) < 1e-3 && std::fabs( position.y() ) < 1e-3 ) ) { rot.rotateZ( position.getPhi() ); }

  if ( m_eDepFrac <= 1.0 ) {
    double tmpEnergyDeposit = initialEnergy * m_eDepFrac;
    auto   tmpPosition      = position;
    while ( tmpEnergyDeposit > m_eKillThr ) {
      m_hitMaker->make( G4FastHit( tmpPosition, tmpEnergyDeposit ), aFastTrack );
      if ( m_eDepFrac == 1.0 ) break;
      initialEnergy -= tmpEnergyDeposit;
      tmpEnergyDeposit = initialEnergy * m_eDepFrac;
      tmpPosition      = tmpPosition + rot * G4ThreeVector( 0, 0, m_stepLength );
    }
  }
}
