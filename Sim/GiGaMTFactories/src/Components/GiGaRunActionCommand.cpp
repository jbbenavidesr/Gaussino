/*****************************************************************************\
* (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// G4
#include "G4UIterminal.hh"
#include "G4UserRunAction.hh"
// Gaussino
#include "GiGaMTCoreMessage/UIMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

/** @class GiGaRunActionCommand GiGaRunActionCommand.h
 *
 *  A concrete Run Action.
 *  It executes some Geant4 commands at begin and at the end of each run
 *
 *  @author Vanya Belyaev
 *  @author Dominik Muller
 *  @author Michał Mazurek
 *  @date   25/07/2001
 *  @date   17/06/2019
 *  @date   21/07/2022
 */

namespace GiGa {
  using RunCommands = std::vector<std::string>;

  class RunActionCommand : public G4UserRunAction, public Gsino::UIMessage {

    RunCommands m_beginCmds = {};
    RunCommands m_endCmds   = {};

    bool m_beginSession = false;
    bool m_endSession   = false;

  public:
    void BeginOfRunAction( const G4Run* run ) override;
    void EndOfRunAction( const G4Run* run ) override;

    inline void setBeginCmds( RunCommands beginCmds ) { m_beginCmds = beginCmds; };
    inline void setEndCmds( RunCommands endCmds ) { m_endCmds = endCmds; };

    inline void setBeginSession( bool beginSession ) { m_beginSession = beginSession; };
    inline void setEndSession( bool endSession ) { m_endSession = endSession; };
  };

  class RunActionCommandFactory : public extends<GiGaTool, GiGaFactoryBase<G4UserRunAction>> {
    Gaudi::Property<RunCommands> m_beginCmds{this, "BeginOfRunCommands", {}};
    Gaudi::Property<RunCommands> m_endCmds{this, "EndOfRunCommands", {}};

    // enable UI session
    Gaudi::Property<bool> m_beginSession{this, "BeginOfRunUISession", false};
    Gaudi::Property<bool> m_endSession{this, "EndOfRunUISession", false};

  public:
    using extends::extends;

    RunActionCommand* construct() const override {
      auto run_act = new RunActionCommand{};
      run_act->SetMessageInterface( message_interface() );
      run_act->setBeginCmds( m_beginCmds.value() );
      run_act->setEndCmds( m_endCmds.value() );
      run_act->setBeginSession( m_beginSession.value() );
      run_act->setEndSession( m_endSession.value() );
      return run_act;
    }
  };
} // namespace GiGa

DECLARE_COMPONENT_WITH_ID( GiGa::RunActionCommandFactory, "GiGaRunActionCommand" )

void GiGa::RunActionCommand::BeginOfRunAction( const G4Run* run ) {
  if ( !run ) { warning( "BeginOfRunAction:: G4Run* points to NULL!" ); }

  for ( auto& command : m_beginCmds ) {
    info( "BeginOfRunAction(): execute '" + command + "'" );
    applyUIcommand( command );
  }

  if ( m_beginSession ) {
    debug( "Opening interactive UI session at the beginning of the run:" );
    auto session = new G4UIterminal();
    session->SessionStart();
    delete session;
  }
}

void GiGa::RunActionCommand::EndOfRunAction( const G4Run* run ) {
  if ( !run ) { warning( "EndOfRunAction:: G4Run* points to NULL!" ); }

  for ( auto& command : m_endCmds ) {
    info( "EndOfRunAction(): execute '" + command + "'" );
    applyUIcommand( command );
  }

  if ( m_endSession ) {
    debug( "Opening interactive UI session at the end of the run:" );
    auto session = new G4UIterminal();
    session->SessionStart();
    delete session;
  }
}
