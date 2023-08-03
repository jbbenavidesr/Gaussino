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
#include "GiGaWorkerPilotFAC.h"

DECLARE_COMPONENT( GiGaWorkerPilotFAC )

/*static*/ std::atomic_uint GiGaWorkerPilotFAC::n_created{ 0 };

GiGaWorkerPilot* GiGaWorkerPilotFAC::construct() const {
  n_created++;
  debug() << "Creating instance number " << std::to_string( n_created ) << endmsg;

  auto pilot = new GiGaWorkerPilot{};
  pilot->SetMessageInterface( message_interface() );
  pilot->m_track_eventstructure = m_track_structure.value();
  pilot->iWorker                = n_created - 1;
  return pilot;
}
