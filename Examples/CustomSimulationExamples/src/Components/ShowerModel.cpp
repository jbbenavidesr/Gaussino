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
#include "GaudiKernel/PhysicalConstants.h"

// Geant4
#include "Randomize.hh"

// Gaussino
// FIXME: change to G4FastSimHitMaker whenever possible
#include "CustomSimulation/HitMaker.h"
#include "CustomSimulation/ModelFactory.h"

namespace Gaussino::CustomSimulation::ShowerModel {
  class Model : public G4VFastSimulationModel, public GiGaMessage {

    std::unique_ptr<HitMaker> m_hitMaker; // FIXME: change to G4FastSimHitMaker whenever possible

    bool   m_killTrack;
    double m_showerRadius;
    double m_showerShapeParam;
    double m_showerScaleParam;
    int    m_hitsPerShower;

  public:
    Model( G4String, G4Region* );

    // fast simulation methods
    inline G4bool IsApplicable( const G4ParticleDefinition& ) override { return true; };
    inline G4bool ModelTrigger( const G4FastTrack& ) override { return true; };
    void          DoIt( const G4FastTrack&, G4FastStep& ) override;

    // property setters
    inline void SetKillTrack( bool killTrack ) { m_killTrack = killTrack; }
    inline void SetShowerRadius( double showerRadius ) { m_showerRadius = showerRadius; }
    inline void SetShowerShapeParam( double showerShapeParam ) { m_showerShapeParam = showerShapeParam; }
    inline void SetShowerScaleParam( double showerScaleParam ) { m_showerScaleParam = showerScaleParam; }
    inline void SetHitsPerShower( double hitsPerShower ) { m_hitsPerShower = hitsPerShower; }
  };

  class ModelFactory : public CustomSimulation::ModelFactory<Model> {

    Gaudi::Property<bool>   m_killTrack{ this, "KillTrack", true,
                                       "Kill the track if fast simulated, otherwise propagate to the end of volume" };
    Gaudi::Property<double> m_showerRadius{ this, "ShowerRadius", -1,
                                            "Radius of each shower will be sampled from (0.0, ShowerRadius]" };
    Gaudi::Property<double> m_showerShapeParam{ this, "ShowerShapeParameter", -1,
                                                "Shape parameter of the gamma distribution" };
    Gaudi::Property<double> m_showerScaleParam{ this, "ShowerScaleParameter", -1,
                                                "Scale parameter of the gamma distribution" };
    Gaudi::Property<int>    m_hitsPerShower{ this, "HitsPerShower", -1, "No. of hits to be generated for each shower" };

  public:
    using base_class = CustomSimulation::ModelFactory<Model>;
    using base_class::ModelFactory;

    StatusCode     initialize() override;
    virtual Model* construct() const override;
  };
} // namespace Gaussino::CustomSimulation::ShowerModel

DECLARE_COMPONENT_WITH_ID( Gaussino::CustomSimulation::ShowerModel::ModelFactory, "ShowerModel" )

Gaussino::CustomSimulation::ShowerModel::Model::Model( G4String modelName, G4Region* envelope )
    : G4VFastSimulationModel( modelName, envelope ), m_hitMaker( new Gaussino::CustomSimulation::HitMaker ) {}

void Gaussino::CustomSimulation::ShowerModel::Model::DoIt( const G4FastTrack& aFastTrack, G4FastStep& aFastStep ) {

  if ( m_killTrack ) {
    // kill the track so that it will no longer be propagated by G4
    aFastStep.KillPrimaryTrack();
  } else {
    // otherwise propagate the track to the end of the volume
    auto localPosition = aFastTrack.GetPrimaryTrackLocalPosition();
    auto direction     = aFastTrack.GetPrimaryTrackLocalDirection();
    auto distance      = aFastTrack.GetEnvelopeSolid()->DistanceToOut( localPosition, direction );
    aFastStep.ProposePrimaryTrackFinalPosition( localPosition + direction * distance );
  }

  // global position
  auto position = aFastTrack.GetPrimaryTrack()->GetPosition();
  // energy
  double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
  // get the orientation of the track
  auto orientation = G4RotationMatrix();
  orientation.rotateY( position.getTheta() );
  orientation.rotateZ( position.getPhi() );
  // get the radiation length of the material
  auto radlen = aFastTrack.GetPrimaryTrack()->GetMaterial()->GetRadlen();

  // generate random hits
  for ( int i = 0; i < m_hitsPerShower; i++ ) {
    // phi in xy from uniform distribution
    double phi = G4UniformRand() * 2 * Gaudi::Units::pi;
    // radius in xy from normal distribution
    double radius = G4RandGauss::shoot( 0, m_showerRadius );
    // depth in z from gamma distribution
    double depth            = G4RandGamma::shoot( m_showerShapeParam, m_showerScaleParam );
    auto   relativePosition = G4ThreeVector( radius * std::sin( phi ), radius * std::cos( phi ), depth * radlen );
    // creates a fast hit and calls the sensitive detector ProcessHits method
    m_hitMaker->make( G4FastHit( position + orientation * relativePosition, energy / m_hitsPerShower ), aFastTrack );
  }
}

StatusCode Gaussino::CustomSimulation::ShowerModel::ModelFactory::initialize() {
  return base_class::initialize().andThen( [&] {
    if ( m_showerRadius <= 0 ) {
      error() << "ShowerRadius must be positive!" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_showerShapeParam <= 0 ) {
      error() << "ShowerShapeParameter must be positive!" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_showerScaleParam <= 0 ) {
      error() << "ShowerScaleParameter must be positive!" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_hitsPerShower <= 0 ) {
      error() << "HitsPerShower must be positive!" << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  } );
}

Gaussino::CustomSimulation::ShowerModel::Model*
Gaussino::CustomSimulation::ShowerModel::ModelFactory::construct() const {
  auto model = base_class::construct();
  // setters
  model->SetKillTrack( m_killTrack.value() );
  model->SetShowerRadius( m_showerRadius.value() );
  model->SetShowerShapeParam( m_showerShapeParam.value() );
  model->SetShowerScaleParam( m_showerScaleParam.value() );
  model->SetHitsPerShower( m_hitsPerShower.value() );
  return model;
}
