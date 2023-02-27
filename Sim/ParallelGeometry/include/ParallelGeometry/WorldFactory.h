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
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4VFastSimulationModel.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserParallelWorld.hh"
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

    // GDML Export
    Gaudi::Property<std::string> m_schema{this, "GDMLSchema", "$GDML_base/src/GDMLSchema/gdml.xsd"};
    Gaudi::Property<bool>        m_refs{this, "GDMLAddReferences", true};
    Gaudi::Property<std::string> m_outfile{this, "GDMLFileName", ""};
    Gaudi::Property<bool>        m_outfileOverwrite{this, "GDMLFileNameOverwrite", false,
                                             "Overwrite a GDML if it already exists"};
    // export auxilliary information
    Gaudi::Property<bool> m_exportSD{this, "GDMLExportSD", false};
    Gaudi::Property<bool> m_exportEnergyCuts{this, "GDMLExportEnergyCuts", false};

  public:
    using extends::extends;
    StatusCode             initialize() override;
    StatusCode             finalize() override;
    virtual ParallelWorld* construct() const override;

  protected:
    std::string         parseName() const;
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

    // Custom Simulation
    using CustomSimulationModelFactory = GiGaFactoryBase<G4VFastSimulationModel>;
    ToolHandleArray<CustomSimulationModelFactory> m_cust_model_factories{this};
    Gaudi::Property<std::vector<std::string>>     m_cust_model_factories_names{
        this,
        "CustomSimulationModelFactories",
        {},
        tool_array_setter( m_cust_model_factories, m_cust_model_factories_names ),
        Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};

    using CustomSimulationRegionFactory = GiGaFactoryBase<G4Region>;
    ToolHandleArray<CustomSimulationRegionFactory> m_cust_region_factories{this};
    Gaudi::Property<std::vector<std::string>>      m_cust_region_factories_names{
        this,
        "CustomSimulationRegionFactories",
        {},
        tool_array_setter( m_cust_region_factories, m_cust_region_factories_names ),
        Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};
  };
} // namespace ParallelGeometry

#include "WorldFactory.icpp"
