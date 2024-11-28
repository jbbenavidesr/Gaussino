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
#include "GaudiKernel/Vector3DTypes.h"

// LHCb
#include "Event/MCParticle.h"

namespace Gsino::EDM {
  struct Hit {

    virtual ~Hit() = default;

    inline Gaudi::XYZPoint         GetPosition() const { return m_position; }
    inline Gaudi::XYZVector        GetDisplacement() const { return m_displacement; }
    inline Gaudi::XYZVector        GetMomentum() const { return m_momentum; }
    inline double                  GetP() const { return m_p; }
    inline double                  GetTime() const { return m_time; }
    inline double                  GetEnergy() const { return m_energy; }
    inline const LHCb::MCParticle* GetMCParticle() const { return m_mcparticle; }

    inline void SetPosition( Gaudi::XYZPoint position ) { m_position = position; }
    inline void SetDisplacement( Gaudi::XYZVector displacement ) { m_displacement = displacement; }
    inline void SetMomentum( Gaudi::XYZVector momentum ) {
      m_momentum = momentum;
      m_p        = std::sqrt( momentum.mag2() );
    }
    inline void SetTime( double time ) { m_time = time; }
    inline void SetEnergy( double energy ) { m_energy = energy; }
    inline void SetMCParticle( const LHCb::MCParticle* mcparticle ) { m_mcparticle = mcparticle; }

  private:
    double                  m_time   = 0.;
    double                  m_energy = 0.;
    Gaudi::XYZPoint         m_position{ 0., 0., 0. };
    Gaudi::XYZVector        m_displacement{ 0., 0., 0. };
    Gaudi::XYZVector        m_momentum{ 0., 0., 0. };
    double                  m_p          = 0.;
    const LHCb::MCParticle* m_mcparticle = nullptr;
  };

  using HitPtr = std::shared_ptr<Hit>;
  using Hits   = std::vector<HitPtr>;
} // namespace Gsino::EDM
