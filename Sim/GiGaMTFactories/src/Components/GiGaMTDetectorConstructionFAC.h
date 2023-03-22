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
#include "G4VUserDetectorConstruction.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

#include "G4VFastSimulationModel.hh"
#include "GaudiAlg/FunctionalDetails.h"
#include "GaudiAlg/FunctionalUtilities.h"
#include "Utils/ToolProperty.h"

class IGiGaMTGeoSvc;
class IGaussinoTool;
class G4Material;
class G4VUserParallelWorld;
class IGDMLReader;

namespace ExternalDetector {
  class IEmbedder;
}

// Factory class implemented as a GaudiTool that creates and configures the
// GiGaMTRunMangager singleton.

class GiGaMTDetectorConstructionFAC : public extends<GiGaTool, GiGaFactoryBase<G4VUserDetectorConstruction>> {
public:
  using extends::extends;

  G4VUserDetectorConstruction* construct() const override;
  StatusCode                   initialize() override;
  StatusCode                   finalize() override;

protected:
  typedef std::pair<std::string, std::vector<std::string>>  SensDetNameVolumesPair;
  typedef std::map<std::string, std::vector<std::string>>   SensDetNameVolumesMap;
  typedef ToolHandle<GiGaFactoryBase<G4VSensitiveDetector>> SensDetFac;
  typedef std::map<std::string, SensDetFac>                 SensDetVolumeMap;

  StatusCode                                DressVolumes() const;
  StatusCode                                SaveGDML() const;
  ServiceHandle<IGiGaMTGeoSvc>              m_geoSvc{this, "GiGaMTGeoSvc", "GiGaMTGeo"};
  ToolHandleArray<IGaussinoTool>            m_afterGeo{this};
  Gaudi::Property<std::vector<std::string>> m_afterGeoNames{this,
                                                            "AfterGeoConstructionTools",
                                                            {},
                                                            tool_array_setter( m_afterGeo, m_afterGeoNames ),
                                                            Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};

private:
  // External Detectors
  ToolHandleArray<ExternalDetector::IEmbedder> m_ext_dets{this};
  using ExternalDetectors = std::vector<std::string>;
  Gaudi::Property<ExternalDetectors> m_ext_dets_names{this,
                                                      "ExternalDetectors",
                                                      {},
                                                      tool_array_setter( m_ext_dets, m_ext_dets_names ),
                                                      Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};

  // GDML Export
  Gaudi::Property<std::string> m_schema{this, "GDMLSchema", ""};
  Gaudi::Property<bool>        m_refs{this, "GDMLAddReferences", true};
  Gaudi::Property<std::string> m_outfile{this, "GDMLFileName", ""};
  Gaudi::Property<std::string> m_rootVolumeName{this, "GDMLRootVolumeName", "", "Name of the root volume"};
  Gaudi::Property<bool>        m_outfileOverwrite{this, "GDMLFileNameOverwrite", false,
                                           "Overwrite a GDML if it already exists"};
  // export auxilliary information
  Gaudi::Property<bool> m_exportSD{this, "GDMLExportSD", false};
  Gaudi::Property<bool> m_exportEnergyCuts{this, "GDMLExportEnergyCuts", false};

  // GDML Import
  ToolHandleArray<IGDMLReader> m_gdml_readers{this};
  using GDMLReaders = std::vector<std::string>;
  Gaudi::Property<GDMLReaders> m_gdml_readers_names{this,
                                                    "GDMLReaders",
                                                    {},
                                                    tool_array_setter( m_gdml_readers, m_gdml_readers_names ),
                                                    Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};

  SensDetVolumeMap                       m_sens_dets;
  Gaudi::Property<SensDetNameVolumesMap> m_namemap{this,
                                                   "SensDetVolumeMap",
                                                   {},
                                                   [this]( Gaudi::Details::PropertyBase& ) {
                                                     for ( auto& keypairs : this->m_namemap ) {
                                                       auto& name = keypairs.first;
                                                       m_sens_dets.emplace( std::piecewise_construct,
                                                                            std::forward_as_tuple( name ),
                                                                            std::forward_as_tuple( name, this ) );
                                                     }
                                                   },
                                                   Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};

  // External Materials
  // it's called external materials, and will most likely be used by ExternalDetector package only,
  // but please note that it has GiGaFactoryBase<G4Material>, so any factory inheriting from G4Material will suffice
  using ExternalMaterialTool = GiGaFactoryBase<G4Material>;
  ToolHandleArray<ExternalMaterialTool> m_ext_mats{this};
  using ExternalMaterials = std::vector<std::string>;
  Gaudi::Property<ExternalMaterials> m_ext_mats_names{this,
                                                      "ExternalMaterials",
                                                      {},
                                                      tool_array_setter( m_ext_mats, m_ext_mats_names ),
                                                      Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};

  // Parallel Geometry
  using ParallelWorlds = std::vector<std::string>;
  ToolHandleArray<GiGaFactoryBase<G4VUserParallelWorld>> m_par_worlds;
  Gaudi::Property<ParallelWorlds>                        m_par_worlds_names{this,
                                                     "ParallelWorlds",
                                                     {},
                                                     tool_array_setter( m_par_worlds, m_par_worlds_names ),
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
