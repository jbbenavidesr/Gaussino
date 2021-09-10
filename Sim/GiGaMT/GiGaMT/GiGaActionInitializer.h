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
#pragma once

#include "GaudiAlg/GaudiTool.h"
#include "Geant4/G4VUserActionInitialization.hh"
#include "GiGaMTCoreMessage/IGiGaMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"
#include "Utils/ToolProperty.h"
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
 *
 * TODO: Fix that ugly hack that wraps the factory functions into a dummy G4VUserActionInitialization
 * object
 */

typedef GiGaFactoryBase<G4UserRunAction> RunActionFactory;
typedef GiGaFactoryBase<G4UserEventAction> EventActionFactory;
typedef GiGaFactoryBase<G4UserStackingAction> StackingActionFactory;
typedef GiGaFactoryBase<G4UserTrackingAction> TrackingActionFactory;
typedef GiGaFactoryBase<G4UserSteppingAction> SteppingActionFactory;

class GiGaActionInitializer : public extends<GiGaTool, GiGaFactoryBase<G4VUserActionInitialization>>,
                              public G4VUserActionInitialization
{
public:
  using extends::extends;
  // All actions can be provided as a list of strings which are then used to fetch
  // the corresponding tools used as the factories in the build. Properties are
  // default constructed, i.e. the lists are empty

public:
  /** Function only called in the G4MTRunManager to setup actions that are handled by
   * the master thread. According to the G4 documentation, the only type of action
   * that is sensible are UserRunActions and hence UserRunActions are also applied to the
   * Master thread.
   */
  virtual void BuildForMaster() const override;
  /** Main function which constructs the various objects using the provided factories
   * and places them in tiny sequencers. These sequencers are header only implementations
   * in GiGaMTCore.
   */
  virtual void Build() const override;

  /**Function to construct a dummy G4VUserActionInitialization object to wrap Build() and
   * BuildForMaster() to be passed to Geant4 which Geant4 can later delete as it likes
   * without messing up the GaudiTool here.
   * Basically, this is now a factory for a factory: Factoriception
   */
  virtual G4VUserActionInitialization* construct() const override;

  /**Simple initialize to explicitly trigger the retrieve of the ToolHandles to avoid
   * thread-safety issues
   */
  virtual StatusCode initialize() override;

private:
  // Storage for the factories
  ToolHandleArray<RunActionFactory> m_UserRunActionFactories{this};
  ToolHandleArray<EventActionFactory> m_UserEventActionFactories{this};
  ToolHandle<StackingActionFactory> m_UserStackingActionFactory{this, "StackingAction", ""};
  ToolHandleArray<TrackingActionFactory> m_UserTrackingActionFactories{this};
  ToolHandleArray<SteppingActionFactory> m_UserSteppingActionFactories{this};
  Gaudi::Property<std::vector<std::string>> m_UserRunAction{
      this,
      "RunActions",
      {},
      tool_array_setter( m_UserRunActionFactories, m_UserRunAction ),
      Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};
  Gaudi::Property<std::vector<std::string>> m_UserEventAction{
      this,
      "EventActions",
      {},
      tool_array_setter( m_UserEventActionFactories, m_UserEventAction ),
      Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};
  Gaudi::Property<std::vector<std::string>> m_UserTrackingAction{
      this,
      "TrackingActions",
      {},
      tool_array_setter( m_UserTrackingActionFactories, m_UserTrackingAction ),
      Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};
  Gaudi::Property<std::vector<std::string>> m_UserSteppingAction{
      this,
      "SteppingActions",
      {},
      tool_array_setter( m_UserSteppingActionFactories, m_UserSteppingAction ),
      Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};
};
