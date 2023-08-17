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

// Gaussino
// FIXME: change to G4FastSimHitMaker whenever possible
#include "CustomSimulation/HitMaker.h"
#include "CustomSimulation/ModelFactory.h"

namespace Gaussino::CustomSimulation::ImmediateDeposit {

  class Model : public G4VFastSimulationModel, public Gsino::Message {

    // FIXME: change to G4FastSimHitMaker whenever possible
    std::unique_ptr<HitMaker> m_hitMaker;

  public:
    Model( G4String modelName, G4Region* envelope );

    inline G4bool IsApplicable( const G4ParticleDefinition& ) override { return true; };
    inline G4bool ModelTrigger( const G4FastTrack& ) override { return true; };
    void          DoIt( const G4FastTrack& aFastTrack, G4FastStep& aFastStep ) override;
  };

  using ModelFactory = CustomSimulation::ModelFactory<Model>;
} // namespace Gaussino::CustomSimulation::ImmediateDeposit

Gaussino::CustomSimulation::ImmediateDeposit::Model::Model( G4String modelName, G4Region* envelope )
    : G4VFastSimulationModel( modelName, envelope ), m_hitMaker( new Gaussino::CustomSimulation::HitMaker ) {}

void Gaussino::CustomSimulation::ImmediateDeposit::Model::DoIt( const G4FastTrack& aFastTrack, G4FastStep& aFastStep ) {
  // kill the track so that it will no longer be propagated by G4
  aFastStep.KillPrimaryTrack();
  double edep = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
  auto   pos  = aFastTrack.GetPrimaryTrackLocalPosition();
  // FIXME: change to G4FastSimHitMaker whenever possible
  m_hitMaker->make( G4FastHit( pos, edep ), aFastTrack );
}

DECLARE_COMPONENT_WITH_ID( Gaussino::CustomSimulation::ImmediateDeposit::ModelFactory, "ImmediateDepositModel" )
