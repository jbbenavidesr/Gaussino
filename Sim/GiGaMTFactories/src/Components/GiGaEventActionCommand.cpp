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
#include "G4UserEventAction.hh"
// Gaussino
#include "GiGaMTCoreMessage/UIMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

/** @class GiGaEventActionCommand GiGaEventActionCommand.h
 *
 *  A concrete Event Action.
 *  It executes some Geant4 commands at begin and at the
 *  end of each event
 *
 *  @author Vanya Belyaev
 *  @author Michał Mazurek
 *  @date   25/07/2001
 *  @date   21/07/2022
 */

namespace GiGa {

  using EventCommands = std::vector<std::string>;

  class EventActionCommand : public G4UserEventAction, public Gsino::UIMessage {

    EventCommands m_beginCmds = {};
    EventCommands m_endCmds   = {};

    bool m_beginSession = false;
    bool m_endSession   = false;

  public:
    void BeginOfEventAction( const G4Event* ) override;
    void EndOfEventAction( const G4Event* ) override;

    inline void setBeginCmds( EventCommands beginCmds ) { m_beginCmds = beginCmds; };
    inline void setEndCmds( EventCommands endCmds ) { m_endCmds = endCmds; };

    inline void setBeginSession( bool beginSession ) { m_beginSession = beginSession; };
    inline void setEndSession( bool endSession ) { m_endSession = endSession; };
  };

  class EventActionCommandFactory : public extends<GiGaTool, GiGaFactoryBase<G4UserEventAction>> {
    Gaudi::Property<EventCommands> m_beginCmds{this, "BeginOfEventCommands", {}};
    Gaudi::Property<EventCommands> m_endCmds{this, "EndOfEventCommands", {}};

    // enable UI session
    Gaudi::Property<bool> m_beginSession{this, "BeginOfEventUISession", false};
    Gaudi::Property<bool> m_endSession{this, "EndOfEventUISession", false};

  public:
    using extends::extends;

    EventActionCommand* construct() const override {
      auto evt_act = new EventActionCommand{};
      evt_act->SetMessageInterface( message_interface() );
      evt_act->setBeginCmds( m_beginCmds.value() );
      evt_act->setEndCmds( m_endCmds.value() );
      evt_act->setBeginSession( m_beginSession.value() );
      evt_act->setEndSession( m_endSession.value() );
      return evt_act;
    }
  };
} // namespace GiGa

DECLARE_COMPONENT_WITH_ID( GiGa::EventActionCommandFactory, "GiGaEventActionCommand" )

void GiGa::EventActionCommand::BeginOfEventAction( const G4Event* event ) {
  if ( !event ) { warning( "BeginOfEventAction:: G4Event* points to NULL!" ); }

  for ( auto& beginCmd : m_beginCmds ) {
    info( "BeginOfEventAction(): execute '" + beginCmd + "'" );
    applyUIcommand( beginCmd );
  }

  if ( m_beginSession ) {
    debug( "Opening interactive UI session at the beginning of the event:" );
    auto session = new G4UIterminal();
    session->SessionStart();
    delete session;
  }
}

void GiGa::EventActionCommand::EndOfEventAction( const G4Event* event ) {
  if ( !event ) { warning( "EndOfEventAction:: G4Event* points to NULL!" ); }

  for ( auto& endCmd : m_endCmds ) {
    info( "EndOfEventAction(): execute '" + endCmd + "'" );
    applyUIcommand( endCmd );
  }

  if ( m_endSession ) {
    debug( "Opening interactive UI session at the end of the event:" );
    auto session = new G4UIterminal();
    session->SessionStart();
    delete session;
  }
}
