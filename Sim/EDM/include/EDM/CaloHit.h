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

// Gaudi
#include "GaudiKernel/Point3DTypes.h"

// LHCb
#include "Event/MCParticle.h"

namespace Gsino::EDM {
  struct CaloHit {

    virtual ~CaloHit() = default;

    inline Gaudi::XYZPoint         GetPosition() const { return m_position; }
    inline double                  GetTime() const { return m_time; }
    inline double                  GetEdep() const { return m_energy; }
    inline const LHCb::MCParticle* GetMCParticle() const { return m_mcparticle; }

    inline void SetPosition( Gaudi::XYZPoint position ) { m_position = position; }
    inline void SetTime( double time ) { m_time = time; }
    inline void SetEdep( double energy ) { m_energy = energy; }
    inline void SetMCParticle( const LHCb::MCParticle* mcparticle ) { m_mcparticle = mcparticle; }

  private:
    double                  m_time   = 0.;
    double                  m_energy = 0.;
    Gaudi::XYZPoint         m_position{ 0., 0., 0. };
    const LHCb::MCParticle* m_mcparticle = nullptr;
  };

  using CaloHitPtr = std::shared_ptr<CaloHit>;
  using CaloHits   = std::vector<CaloHitPtr>;
} // namespace Gsino::EDM
