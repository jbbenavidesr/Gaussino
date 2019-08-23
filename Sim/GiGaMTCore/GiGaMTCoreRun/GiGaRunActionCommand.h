#pragma once

#include "Geant4/G4UserRunAction.hh"
#include "GiGaMTCoreMessage/IGiGaMessage.h"
#include <string>
#include <vector>

/** @class GiGaRunActionCommand GiGaRunActionCommand.h
 *
 *  A concrete Run Action.
 *  It executes some Geant4 commands at begin and at the
 *  end of each run
 *
 *  @author Vanya Belyaev
 *  @author Dominik Muller
 *  @date   17/06/2019
 */

class GiGaRunActionCommandFAC;

class GiGaRunActionCommand : public virtual G4UserRunAction, public GiGaMessage
{
  friend class GiGaRunActionCommandFAC;

public:
  typedef std::vector<std::string> COMMANDS;

  void BeginOfRunAction( const G4Run* run ) override;
  void EndOfRunAction( const G4Run* run ) override;

private:
  COMMANDS m_beginCmds;
  COMMANDS m_endCmds;
};
