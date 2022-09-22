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
#include "GiGaMTCoreRun/G4EventProxy.h"
#include "GiGaMTCoreRun/GiGaWorkerPilot.h"

G4EventProxy::~G4EventProxy()
{
  if ( m_vec && m_event ) {
    m_vec->RegisterForCleanUp( m_event );
  } else if (m_event) {
    delete m_event;
  }
}

G4EventProxy::G4EventProxy( G4EventProxy&& right ) noexcept : m_event( right.m_event ), m_vec( right.m_vec ), m_truth(right.m_truth)
{
  right.m_event = nullptr;
  right.m_vec   = nullptr;
  right.m_truth = nullptr;
}
