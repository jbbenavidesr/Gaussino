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

// Geant4
#include "Geant4/G4ParallelWorldPhysics.hh"

namespace ParallelGeometry {

  using ParticleConstructor = std::function<void()>;
  using ProcessConstructor  = std::function<void()>;

  class Physics : public G4ParallelWorldPhysics {

  protected:
    ParticleConstructor m_particle_constructor;
    ProcessConstructor  m_process_constructor;
    bool                m_runStandardProcess = true;

  public:
    Physics( std::string, bool, bool );
    virtual ~Physics() = default;

    virtual void ConstructParticle() override;
    virtual void ConstructProcess() override;

    void setParticleConstructor( ParticleConstructor );
    void setProcessConstructor( ProcessConstructor );
  };
} // namespace ParallelGeometry
