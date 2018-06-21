#include "GiGaMT/GiGaActionInitializer.h"

// Using the Multi-action lists in Geant4 since 10.3
// Even though they publically inherit from std::vector
#include "Geant4/G4MultiEventAction.hh"
#include "Geant4/G4MultiRunAction.hh"
#include "Geant4/G4MultiSteppingAction.hh"
#include "Geant4/G4MultiTrackingAction.hh"

DECLARE_COMPONENT( GiGaActionInitializer )

StatusCode GiGaActionInitializer::initialize()
{
  auto sc = GaudiTool::initialize();
  // Now get all those factories and stuff them into the vector
  for ( auto& name : m_UserRunAction ) {
    auto t = tool<RunActionFactory>( name, this );
    if ( !t ) {
      return Error( "Could not get " + name );
    }
    m_UserRunActionsFactories.push_back( t );
  }

  for ( auto& name : m_UserEventAction ) {
    auto t = tool<EventActionFactory>( name, this );
    if ( !t ) {
      return Error( "Could not get " + name );
    }
    m_UserEventActionFactories.push_back( t );
  }

  if ( m_UserStackingAction != "" ) {
    m_UserStackingActionFactory = tool<StackingActionFactory>( m_UserStackingAction, this );
    if ( !m_UserStackingActionFactory ) {
      return Error( "Could not get " + m_UserStackingAction );
    }
  }

  for ( auto& name : m_UserTrackingAction ) {
    auto t = tool<TrackingActionFactory>( name, this );
    if ( !t ) {
      return Error( "Could not get " + name );
    }
    m_UserTrackingActionFactories.push_back( t );
  }

  for ( auto& name : m_UserSteppingAction ) {
    auto t = tool<SteppingActionFactory>( name, this );
    if ( !t ) {
      return Error( "Could not get " + name );
    }
    m_UserSteppingActionFactories.push_back( t );
  }

  return sc;
}

StatusCode GiGaActionInitializer::finalize()
{
  release_tools( m_UserRunActionsFactories );
  release_tools( m_UserEventActionFactories );
  release_tools( m_UserSteppingActionFactories );
  release_tools( m_UserTrackingActionFactories );
  m_UserStackingActionFactory->release();

  return GaudiTool::finalize();
}

void GiGaActionInitializer::BuildForMaster() const
{
  auto runseq = new G4MultiRunAction{};
  for ( auto& fac : m_UserRunActionsFactories ) {
    runseq->push_back( std::unique_ptr<G4UserRunAction>( fac->construct() ) );
  }
  SetUserAction( runseq );
}

void GiGaActionInitializer::Build() const
{
  { // Sequence of UserRunActions
    auto runseq = new G4MultiRunAction{};
    for ( auto& fac : m_UserRunActionsFactories ) {
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

  { // Sequence of UserStackingAction
    SetUserAction( m_UserStackingActionFactory->construct() );
  }

  { // Sequence of UserTrackingAction
    auto trackseq = new G4MultiTrackingAction{};
    for ( auto& fac : m_UserTrackingActionFactories ) {
      trackseq->push_back( std::unique_ptr<G4UserTrackingAction>( fac->construct() ) );
    }
    SetUserAction( trackseq );
  }
}

G4VUserActionInitialization* GiGaActionInitializer::construct() const
{
  class dummy : public G4VUserActionInitialization
  {
  public:
    dummy( const G4VUserActionInitialization* concrete ) : m_concrete( concrete ) {}

    void Build() const override { m_concrete->Build(); }
    void BuildForMaster() const override { m_concrete->BuildForMaster(); }

  private:
    const G4VUserActionInitialization* m_concrete = nullptr;
  };

  return new dummy( this );
}
