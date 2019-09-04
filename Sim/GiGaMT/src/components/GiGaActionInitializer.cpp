#include "GiGaMT/GiGaActionInitializer.h"

// Using the Multi-action lists in Geant4 since 10.3
// Even though they publically inherit from std::vector
#include "Geant4/G4MultiEventAction.hh"
#include "Geant4/G4MultiRunAction.hh"
#include "Geant4/G4MultiSteppingAction.hh"
#include "Geant4/G4MultiTrackingAction.hh"

DECLARE_COMPONENT( GiGaActionInitializer )

StatusCode GiGaActionInitializer::initialize() {
  for ( auto& fac : m_UserRunActionFactories ) { fac.retrieve(); }
  for ( auto& fac : m_UserEventActionFactories ) { fac.retrieve(); }
  for ( auto& fac : m_UserTrackingActionFactories ) { fac.retrieve(); }
  for ( auto& fac : m_UserSteppingActionFactories ) { fac.retrieve(); }
  if ( !m_UserStackingActionFactory.empty() ) { m_UserStackingActionFactory.retrieve(); }
  return StatusCode::SUCCESS;
}
void GiGaActionInitializer::BuildForMaster() const {
  auto runseq = new G4MultiRunAction{};
  for ( auto& fac : m_UserRunActionFactories ) {
    runseq->push_back( std::unique_ptr<G4UserRunAction>( fac->construct() ) );
  }
  SetUserAction( runseq );
}

void GiGaActionInitializer::Build() const {
  { // Sequence of UserRunActions
    auto runseq = new G4MultiRunAction{};
    for ( auto& fac : m_UserRunActionFactories ) {
      runseq->push_back( std::unique_ptr<G4UserRunAction>( fac->construct() ) );
    }
    SetUserAction( runseq );
  }

  { // Sequence of UserEventActions
    auto evtseq = new G4MultiEventAction{};
    for ( auto& fac : m_UserEventActionFactories ) {
      evtseq->push_back( std::unique_ptr<G4UserEventAction>( fac->construct() ) );
    }
    SetUserAction( evtseq );
  }

  if ( !m_UserStackingActionFactory.empty() ) { SetUserAction( m_UserStackingActionFactory->construct() ); }

  { // Sequence of UserTrackingAction
    auto trackseq = new G4MultiTrackingAction{};
    for ( auto& fac : m_UserTrackingActionFactories ) {
      trackseq->push_back( std::unique_ptr<G4UserTrackingAction>( fac->construct() ) );
    }
    SetUserAction( trackseq );
  }

  { // Sequence of UserTrackingAction
    auto stepseq = new G4MultiSteppingAction{};
    for ( auto& fac : m_UserSteppingActionFactories ) {
      stepseq->push_back( std::unique_ptr<G4UserSteppingAction>( fac->construct() ) );
    }
    SetUserAction( stepseq );
  }
}

G4VUserActionInitialization* GiGaActionInitializer::construct() const {
  class dummy : public G4VUserActionInitialization {
  public:
    dummy( const G4VUserActionInitialization* concrete ) : m_concrete( concrete ) {}

    void Build() const override { m_concrete->Build(); }
    void BuildForMaster() const override { m_concrete->BuildForMaster(); }

  private:
    const G4VUserActionInitialization* m_concrete = nullptr;
  };

  return new dummy( this );
}
