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

// Gaussino
#include "EDM/Hit.h"

namespace Gsino::CaloChallenge {
  /**
   * @brief Represents a collector hit (Gaussino's EDM)
   *
   * This hit is part of Gaussino's EDM and represents a hit in the
   * calorimeter simulation left by an incident particle in the CaloChallenge setup.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  struct CollectorHit : public EDM::Hit {

    inline void   SetKineticEnergy( double aEk ) { m_Ek = aEk; }
    inline double GetKineticEnergy() const { return m_Ek; }

    inline void   SetPrimaryEnergy( double aEp ) { m_Ep = aEp; }
    inline double GetPrimaryEnergy() const { return m_Ep; }

    inline void   SetTheta( double aTheta ) { m_Theta = aTheta; }
    inline double GetTheta() const { return m_Theta; }

    inline void   SetPhi( double aPhi ) { m_Phi = aPhi; }
    inline double GetPhi() const { return m_Phi; }

    inline void             SetDirection( Gaudi::XYZVector aDirection ) { m_Direction = aDirection; }
    inline Gaudi::XYZVector GetDirection() const { return m_Direction; }

    inline void SetPDG( int aPDG ) { m_PDG = aPDG; }
    inline int  GetPDG() const { return m_PDG; }

    inline void SetTrackID( int aTrackID ) { m_TrackID = aTrackID; }
    inline int  GetTrackID() const { return m_TrackID; }

    inline void SetEventID( int aEventID ) { m_EventID = aEventID; }
    inline int  GetEventID() const { return m_EventID; }

  private:
    double           m_Ek        = 0;
    double           m_Ep        = 0;
    double           m_Theta     = 0;
    double           m_Phi       = 0;
    Gaudi::XYZVector m_Direction = { 0, 0, 0 };
    int              m_PDG       = 0;
    int              m_TrackID   = -1;
    int              m_EventID   = -1;
  };

  using CollectorHitPtr = std::shared_ptr<CollectorHit>;
  using CollectorHits   = std::vector<CollectorHitPtr>;
} // namespace Gsino::CaloChallenge
