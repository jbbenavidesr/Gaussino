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
  struct CollectorHit {

    inline void   SetKineticEnergy( double aEk ) { m_Ek = aEk; }
    inline double GetKineticEnergy() const { return m_Ek; }

    inline void   SetPrimaryEnergy( double aEp ) { m_Ep = aEp; }
    inline double GetPrimaryEnergy() const { return m_Ep; }

    inline void SetAngle( int aAngle ) { m_Angle = aAngle; }
    inline int  GetAngle() const { return m_Angle; }

    inline void SetPDG( int aPDG ) { m_PDG = aPDG; }
    inline int  GetPDG() const { return m_PDG; }

    inline void SetTrackID( int aTrackID ) { m_TrackID = aTrackID; }
    inline int  GetTrackID() const { return m_TrackID; }

    inline void SetEventID( int aEventID ) { m_EventID = aEventID; }
    inline int  GetEventID() const { return m_EventID; }

  private:
    double m_Ek      = 0;
    double m_Ep      = 0;
    double m_Angle   = 0;
    int    m_PDG     = 0;
    int    m_TrackID = -1;
    int    m_EventID = -1;
  };

  using CollectorHitPtr = std::shared_ptr<CollectorHit>;
  using CollectorHits   = std::vector<CollectorHitPtr>;
} // namespace Gaussino::G4Par04
