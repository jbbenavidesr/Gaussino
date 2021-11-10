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
#include "GiGaMTCoreMessage/IGiGaMessage.h"

namespace ParallelGeometry {

  using ProcessConstructor = std::function<bool()>;

  class Physics : public GiGaMessage, public G4ParallelWorldPhysics {

  protected:
    ProcessConstructor m_process_constructor;
    bool               m_layeredMass  = false;
    std::vector<int>   m_particlePIDs = {};

  public:
    Physics( std::string, bool );
    virtual ~Physics() = default;

    virtual void ConstructProcess() override;

    void setProcessConstructor( ProcessConstructor );
    void setParticlePIDs( std::vector<int> );
  };
} // namespace ParallelGeometry
