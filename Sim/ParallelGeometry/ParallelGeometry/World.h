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
#include "Geant4/G4VUserParallelWorld.hh"

class G4VPhysicalVolume;

namespace ParallelGeometry {
  using WorldConstructor = std::function<void( G4VPhysicalVolume* )>;
  using SDConstructor    = std::function<void()>;

  class World : public G4VUserParallelWorld {

  protected:
    WorldConstructor m_world_constructor;
    SDConstructor    m_sd_constructor;

  public:
    World( std::string name );
    virtual ~World() = default;

    virtual void Construct() override;
    virtual void ConstructSD() override;

    void setWorldConstructor( WorldConstructor );
    void setSDConstructor( SDConstructor );
  };
} // namespace ParallelGeometry
