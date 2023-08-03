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

namespace Gaussino::G4Par04 {
  struct CaloHit {

    inline void   SetEdep( double aEdep ) { m_Edep = aEdep; }
    inline double GetEdep() const { return m_Edep; }

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
    double m_Edep    = 0;
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
} // namespace Gaussino::G4Par04
