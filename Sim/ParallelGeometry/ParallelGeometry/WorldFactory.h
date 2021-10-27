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
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VUserParallelWorld.hh"
// local
#include "GaudiAlg/FunctionalDetails.h"
#include "GiGaMTCoreDet/IExternalDetectorEmbedder.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"
#include "ParallelGeometry/World.h"
#include "Utils/ToolProperty.h"

namespace ParallelGeometry {

  template <class ParallelWorld>
  class WorldFactory : public extends<GiGaTool, GiGaFactoryBase<G4VUserParallelWorld>> {
    static_assert( std::is_base_of<ParallelGeometry::World, ParallelWorld>::value );

  public:
    using extends::extends;
    virtual ParallelWorld* construct() const override;

  protected:
    inline virtual void additionalWorldConstrution( G4VPhysicalVolume* ) const {};
    inline virtual void additionalSDConstrution() const {};

    Gaudi::Property<std::string> m_worldMaterial{this, "WorldMaterial", ""};

    // External Detectors
    ToolHandleArray<ExternalDetector::IEmbedder> m_ext_dets{this};
    using ExternalDetectors = std::vector<std::string>;
    Gaudi::Property<ExternalDetectors> m_ext_dets_names{this,
                                                        "ExternalDetectors",
                                                        {},
                                                        tool_array_setter( m_ext_dets, m_ext_dets_names ),
                                                        Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};
  };
} // namespace ParallelGeometry

#include "WorldFactory.icpp"
