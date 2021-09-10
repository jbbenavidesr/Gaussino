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
#include "GiGaMTCoreRun/GiGaRunActionCommand.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

// Factory class implemented as a GaudiTool that creates and configures the
// GiGaMTRunMangager singleton.

class GiGaRunActionCommandFAC : public extends<GiGaTool, GiGaFactoryBase<G4UserRunAction>>
{
public:
  Gaudi::Property<std::vector<std::string>> m_beginCmds{this, "BeginOfRunCommands", {}};
  Gaudi::Property<std::vector<std::string>> m_endCmds{this, "EndOfRunCommands", {}};
  using extends::extends;

  G4UserRunAction* construct() const override;

private:
  static std::atomic_bool created;
};
