#include "Geant4/G4UserEventAction.hh"
#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4UserStackingAction.hh"
#include "Geant4/G4UserSteppingAction.hh"
#include "Geant4/G4UserTrackingAction.hh"
#include <vector>

/* Lots of little wrappers that allow to use more than one
 * action of each type by storing a sequence of actions
 * and broadcasting each call to the contained actions.
 *
 * they are internally stored as std::vectors accessible via
 * the member 'sequence'.
 *
 * IMPORTANT:
 * The sequencer own the contained actions and will take them with
 * it once deleted by geant4 at the end of the processing.
 *
 * TODO: This is tedious code copy&paste. Use cool templates maybe?
 * */

template <typename T>
class ActionSequenceBase : public T
{
public:
  virtual ~ActionSequenceBase()
  {
    for ( auto action : sequence ) {
      delete action;
    }
  }
  std::vector<T*> sequence{};
};

class GiGaRunActionSequence : public ActionSequenceBase<G4UserRunAction>
{
  virtual void BeginOfRunAction( const G4Run* aRun ) override
  {
    for ( auto& a : sequence ) {
      a->BeginOfRunAction( aRun );
    }
  };
  virtual void EndOfRunAction( const G4Run* aRun ) override
  {
    for ( auto& a : sequence ) {
      a->EndOfRunAction( aRun );
    }
  };
  virtual G4Run* GenerateRun() override
  {
    for ( auto& a : sequence ) {
      auto run = a->GenerateRun();
      if ( run ) {
        return run;
      }
    }
    return nullptr;
  };
};

class GiGaEventActionSequence : public ActionSequenceBase<G4UserEventAction>
{
  virtual void BeginOfEventAction( const G4Event* aEvent ) override
  {
    for ( auto& a : sequence ) {
      a->BeginOfEventAction( aEvent );
    }
  };
  virtual void EndOfEventAction( const G4Event* aEvent ) override
  {
    for ( auto& a : sequence ) {
      a->EndOfEventAction( aEvent );
    }
  };
  virtual void SetEventManager( G4EventManager* value ) override
  {
    for ( auto& a : sequence ) {
      a->SetEventManager( value );
    }
  };
};

class GiGaSteppingActionSequence : public ActionSequenceBase<G4UserSteppingAction>
{
  virtual void UserSteppingAction( const G4Step* aStep ) override
  {
    for ( auto& a : sequence ) {
      a->UserSteppingAction( aStep );
    }
  };
  virtual void SetSteppingManagerPointer( G4SteppingManager* value ) override
  {
    for ( auto& a : sequence ) {
      a->SetSteppingManagerPointer( value );
    }
  };
};

class GiGaTrackingActionSequence : public ActionSequenceBase<G4UserTrackingAction>
{
  virtual void PreUserTrackingAction( const G4Track* aTrack ) override
  {
    for ( auto& a : sequence ) {
      a->PreUserTrackingAction( aTrack );
    }
  };
  virtual void PostUserTrackingAction( const G4Track* aTrack ) override
  {
    for ( auto& a : sequence ) {
      a->PostUserTrackingAction( aTrack );
    }
  };
  virtual void SetTrackingManagerPointer( G4TrackingManager* value ) override
  {
    for ( auto& a : sequence ) {
      a->SetTrackingManagerPointer( value );
    }
  };
};
