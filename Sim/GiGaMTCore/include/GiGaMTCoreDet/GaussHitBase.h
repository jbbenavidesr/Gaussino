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
// Include files
#include "G4VHit.hh"

/** @class Det/GaussHitBase Det/GaussHitBase.h GaussTools/Det/GaussHitBase.h
 *
 *
 *  @author Witold Pokorski
 *  @date   2002-10-29
 */

namespace Gaussino {

  class HitBase : public G4VHit {
  public:
    /// Standard constructor
    inline HitBase() = default;

    virtual inline ~HitBase() = default; ///< Destructor

    inline void  SetTrackID( G4int tid ) { m_trackID = tid; };
    inline G4int GetTrackID() const { return m_trackID; };
    inline void  SetDetectorID( G4int tid ) { m_detectorID = tid; };
    inline G4int GetDetectorID() const { return m_detectorID; };

    /// Accessor in "Gaudi-style"
    inline G4int trackID() const { return m_trackID; }
    /// Accessor in "Gaudi-style"
    inline void setTrackID( const G4int id ) { m_trackID = id; }

  protected:
    G4int m_trackID{ -1 };
    G4int m_detectorID{ -1 };
  };
} // namespace Gaussino
