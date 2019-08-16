#include "Geant4/G4UserSteppingAction.hh"
#include "Geant4/globals.hh"

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

#include "Geant4/G4Event.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4Step.hh"
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

  G4cout << "Is first in volume: " <<step->IsFirstStepInVolume() << "\n";
  G4cout << "Is last in volume: " <<step->IsLastStepInVolume() << "\n";
  G4cout << "#### END STEP ####" << std::endl;
}

#include "GaudiAlg/GaudiTool.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"

#include "Geant4/G4UserTrackingAction.hh"

class DebugStepActionFAC : public extends<GaudiTool, GiGaFactoryBase<G4UserSteppingAction>>
{
  using extends::extends;
  virtual G4UserSteppingAction* construct() const override {
    return new Gaussino::DebugStepAction{};
  }
};

DECLARE_COMPONENT_WITH_ID( DebugStepActionFAC, "DebugStepAction" )
