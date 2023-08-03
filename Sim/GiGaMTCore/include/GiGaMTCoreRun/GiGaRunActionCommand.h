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

#include "G4UserRunAction.hh"
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

class GiGaRunActionCommand : public virtual G4UserRunAction, public GiGaMessage {
  friend class GiGaRunActionCommandFAC;

public:
  typedef std::vector<std::string> COMMANDS;

  void BeginOfRunAction( const G4Run* run ) override;
  void EndOfRunAction( const G4Run* run ) override;

private:
  COMMANDS m_beginCmds;
  COMMANDS m_endCmds;
};
