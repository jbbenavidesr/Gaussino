#pragma once

#include "GaudiAlg/GaudiTool.h"
#include "Geant4/G4VUserActionInitialization.hh"
#include "GiGaMTCore/IGiGaMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include <vector>

/* GiGa initialization class for user actions. After creating the
 * worker threads and creating the WorkerRunManager from within the
 * thread, Build() should be called to configure the actions for this worker.
 *
 * As this class only exists once and works like a factory, it directly inherits
 * from GaudiTool to allow easy configuration. The different actions are provided
 * via their respective factories which must implement the templated interface
 * GiGaFactoryBase<T> which requires the instantiation of the abstract method
 *
 * T construct() const
 *
 * When Build() is called, the factories are used to construct the individual actions
 * which are then placed in small wrapper-actions to allow specifying more than
 * one action.
 *
 * TODO: This tool will likely need to instantiate some dummy G4VUserPrimaryGeneratorAction.
 *
 * TODO: Add a stepping verbose thingy, whatever that is ...
 *
 * TODO: Can't easily implement sequences for UserStackingAction as everything is non-void.
 */

typedef GiGaFactoryBase<G4UserRunAction> RunActionFactory;
typedef GiGaFactoryBase<G4UserEventAction> EventActionFactory;
typedef GiGaFactoryBase<G4UserStackingAction> StackingActionFactory;
typedef GiGaFactoryBase<G4UserTrackingAction> TrackingActionFactory;
typedef GiGaFactoryBase<G4UserSteppingAction> SteppingActionFactory;

class GigaActionInitializer : public G4VUserActionInitialization, public GaudiTool, public IGiGaMessage
{
public:
  // All actions can be provided as a list of strings which are then used to fetch
  // the corresponding tools used as the factories in the build. Properties are
  // default constructed, i.e. the lists are empty
  Gaudi::Property<std::vector<std::string>> m_UserRunAction{this, "RunActions"};
  Gaudi::Property<std::vector<std::string>> m_UserEventAction{this, "EventActions"};
  Gaudi::Property<std::string> m_UserStackingAction{this, "StackingAction"};
  Gaudi::Property<std::vector<std::string>> m_UserTrackingAction{this, "TrackingActions"};
  Gaudi::Property<std::vector<std::string>> m_UserSteppingAction{this, "SteppingActions"};

public:
  // Just use the GaudiTool constructors here. Remaining two base classes are hence default
  // constructed which is fine as they are default constructed.
  using GaudiTool::GaudiTool;
  virtual ~GigaActionInitializer();
  StatusCode initialize() override;
  StatusCode finalize() override;

  /* Function only called in the G4MTRunManager to setup actions that are handled by
   * the master thread. According to the G4 documentation, the only type of action
   * that is sensible are UserRunActions and hence UserRunActions are also applied to the
   * Master thread.
   */
  virtual void BuildForMaster() const override;
  /* Main function which constructs the various objects using the provided factories
   * and places them in tiny sequencers. These sequencers are header only implementations
   * in GiGaMTCore.
   */
  virtual void Build() const override;

private:
  template <typename T>
  void release_tools( T& cont )
  {
    for ( auto& t : cont ) {
      cont->release();
    }
  }
  // Storage for the factories
  std::vector<RunActionFactory*> m_UserRunActionsFactories{};
  std::vector<EventActionFactory*> m_UserEventActionFactories{};
  StackingActionFactory* m_UserStackingActionFactory = nullptr;
  std::vector<TrackingActionFactory*> m_UserTrackingActionFactories{};
  std::vector<SteppingActionFactory*> m_UserSteppingActionFactories{};
};
