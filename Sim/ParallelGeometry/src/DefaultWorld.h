/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

// G4
#include "Geant4/G4VUserParallelWorld.hh"

class G4VPhysicalVolume;

namespace ParallelGeometry {
  class DefaultWorld : public G4VUserParallelWorld {

  private:
    using WorldConstructor = std::function<void( G4VPhysicalVolume* )>;
    WorldConstructor m_world_constructor;

    using SDConstructor = std::function<void()>;
    SDConstructor m_sd_constructor;

  public:
    inline DefaultWorld( std::string name ) : G4VUserParallelWorld( name ){};
    virtual ~DefaultWorld() = default;

    inline virtual void Construct() override { m_world_constructor( GetWorld() ); };
    inline virtual void ConstructSD() override { m_sd_constructor(); };

    inline void setWorldConstructor( WorldConstructor constr ) { m_world_constructor = constr; };
    inline void setSDConstructor( SDConstructor constr ) { m_sd_constructor = constr; };
  };
} // namespace ParallelGeometry
