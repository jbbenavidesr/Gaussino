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
#include "GiGaRunActionCommandFAC.h"

DECLARE_COMPONENT_WITH_ID( GiGaRunActionCommandFAC, "GiGaRunActionCommand" )

G4UserRunAction* GiGaRunActionCommandFAC::construct() const
{
  auto ret         = new GiGaRunActionCommand{};
  ret->m_beginCmds = m_beginCmds;
  ret->m_endCmds   = m_endCmds;
  ret->SetMessageInterface( message_interface() );
  return ret;
}
