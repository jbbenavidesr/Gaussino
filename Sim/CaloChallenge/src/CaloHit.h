/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

#pragma once

#include <memory>
#include <vector>

// Gaussino
#include "EDM/CaloHit.h"

namespace Gsino::CaloChallenge {
  /**
   * @brief Represents a calo hit (Gaussino's EDM)
   *
   * This hit is part of Gaussino's EDM and represents a hit in the
   * calorimeter simulation left by a calorimeter shower in the CaloChallenge setup.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  struct CaloHit : public EDM::CaloHit {

    using Gsino::EDM::CaloHit::CaloHit;

    inline void SetZId( int aZ ) { m_ZId = aZ; }
    inline int  GetZId() const { return m_ZId; }

    inline void SetRhoId( int aRho ) { m_RhoId = aRho; }
    inline int  GetRhoId() const { return m_RhoId; }

    inline void SetPhiId( int aPhi ) { m_PhiId = aPhi; }
    inline int  GetPhiId() const { return m_PhiId; }

    inline void   SetTime( double aTime ) { m_Time = aTime; }
    inline double GetTime() const { return m_Time; }

    inline void SetType( int aType ) { m_Type = aType; }
    inline int  GetType() const { return m_Type; }

    inline void SetTrackID( int aTrackID ) { m_TrackID = aTrackID; }
    inline int  GetTrackID() const { return m_TrackID; }

    inline void SetEventID( int aEventID ) { m_EventID = aEventID; }
    inline int  GetEventID() const { return m_EventID; }

  private:
    int    m_ZId     = -1;
    int    m_PhiId   = -1;
    int    m_RhoId   = -1;
    double m_Time    = -1;
    int    m_Type    = -1;
    int    m_TrackID = -1;
    int    m_EventID = -1;
  };

  using CaloHitPtr = std::shared_ptr<CaloHit>;
  using CaloHits   = std::vector<CaloHitPtr>;
} // namespace Gsino::CaloChallenge
