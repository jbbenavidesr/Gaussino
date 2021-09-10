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
#include "GiGaMTRunManagerFAC.h"

DECLARE_COMPONENT(GiGaMTRunManagerFAC)

/*static*/ std::atomic_bool GiGaMTRunManagerFAC::created{false};

GiGaMTRunManager* GiGaMTRunManagerFAC::construct() const {
  if(created){
    error() << "Already called construct(). Just returning the singleton!" << endmsg;
    return GiGaMTRunManager::GetGiGaMTRunManager();
  }
  debug() << "Constructing the main GiGaMTRunManager instance" << endmsg;

  auto mgr = GiGaMTRunManager::GetGiGaMTRunManager();
  mgr->SetMessageInterface(message_interface());
  created = true;
  return mgr;
}
