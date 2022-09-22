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
#include "GiGaMTCoreRun/GaussinoTrackInformation.h"

// ============================================================================
/** @file
 *  Implementation file for class : GaussTrackInformation
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/02/2001
 */
// ============================================================================

G4ThreadLocal G4Allocator<GaussinoTrackInformation> *aGaussinoTrackInformationAllocator = nullptr;

GaussinoTrackInformation::GaussinoTrackInformation(const GaussinoTrackInformation& right)
    : G4VUserTrackInformation(right),
      m_appendStep(right.m_appendStep),
      m_storeTruth(right.m_storeTruth),
      m_createdHit(right.m_createdHit),
      m_directParent(right.m_directParent),
      m_wasSuspended(right.m_wasSuspended),
      m_hits(right.m_hits),
      m_detInfo(right.m_detInfo){
}
