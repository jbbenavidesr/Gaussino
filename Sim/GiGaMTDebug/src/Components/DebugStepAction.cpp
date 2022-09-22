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
#include "G4UserSteppingAction.hh"
#include "globals.hh"

namespace Gaussino
{
  class DebugStepAction : public G4UserSteppingAction
  {
  public:
    DebugStepAction() = default;
    virtual ~DebugStepAction() = default;

    // method from the base class
    virtual void UserSteppingAction( const G4Step* ) override;
  };
}

#include "G4Event.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "HepMC3/FourVector.h"

template <typename T>
T& operator<<( T& ostr, const HepMC3::FourVector& fv )
{
  ostr << "[" << fv.x() << ", " << fv.y() << ", " << fv.z() << ", " << fv.t() << "]";
  return ostr;
}
void Gaussino::DebugStepAction::UserSteppingAction( const G4Step* step )
{
  G4cout << "#### BEGIN STEP ####\n";
  auto deltafourmomentum = HepMC3::FourVector( step->GetDeltaMomentum().x(), step->GetDeltaMomentum().y(),
                                               step->GetDeltaMomentum().z(), step->GetDeltaEnergy() );
  auto presteppos =
      HepMC3::FourVector( step->GetPreStepPoint()->GetPosition().x(), step->GetPreStepPoint()->GetPosition().y(),
                          step->GetPreStepPoint()->GetPosition().z(), step->GetPreStepPoint()->GetGlobalTime() );
  auto poststeppos =
      HepMC3::FourVector( step->GetPostStepPoint()->GetPosition().x(), step->GetPostStepPoint()->GetPosition().y(),
                          step->GetPostStepPoint()->GetPosition().z(), step->GetPostStepPoint()->GetGlobalTime() );
  auto track = step->GetTrack();
  auto fourmomentum = HepMC3::FourVector( track->GetMomentum().x(), track->GetMomentum().y(), track->GetMomentum().z(),
                                     track->GetTotalEnergy() );

  // get volume of the current step
  G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  G4cout << "Volume = " << volume->GetName() << "\n";
  G4cout << "Delta Momentum = " << deltafourmomentum << "\n";
  G4cout << "PreStepPoint = " << presteppos << "\n";
  G4cout << "PostStepPoint = " << poststeppos << "\n";
  G4cout << "Track momentum = " << fourmomentum << "\n";
  if(auto proc=step->GetPostStepPoint()->GetProcessDefinedStep(); proc)
    G4cout << "PostStepPoint Process = " << proc->GetProcessName() << "\n";

  if(auto proc=step->GetPreStepPoint()->GetProcessDefinedStep(); proc)
    G4cout << "PreStepPoint Process = " << proc->GetProcessName() << "\n";
  G4cout << "Track momentum = " << fourmomentum << "\n";

  G4cout << "Is first in volume: " <<step->IsFirstStepInVolume() << "\n";
  G4cout << "Is last in volume: " <<step->IsLastStepInVolume() << "\n";
  G4cout << "#### END STEP ####" << std::endl;
}

#include "GaudiAlg/GaudiTool.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"

#include "G4UserTrackingAction.hh"

class DebugStepActionFAC : public extends<GaudiTool, GiGaFactoryBase<G4UserSteppingAction>>
{
  using extends::extends;
  virtual G4UserSteppingAction* construct() const override {
    return new Gaussino::DebugStepAction{};
  }
};

DECLARE_COMPONENT_WITH_ID( DebugStepActionFAC, "DebugStepAction" )
