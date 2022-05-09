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
// Gaudi includes
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartIF.h"

// Gaudi includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Transform3DTypes.h"
#include "G4VSensitiveDetector.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"

#include "Kernel/IPropertyConfigSvc.h"

#include "GiGaMTGeo/IGiGaMTGeoSvc.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4Mapping.h"
#include "GiGaMTDD4hep/Utilities.h"

class DD4hepCnvSvc : public extends<Service, IGiGaMTGeoSvc> {
public:
  using extends::extends;
  virtual ~DD4hepCnvSvc() = default;

  // Service pure member functions
  StatusCode initialize() override;
  StatusCode finalize() override;

  // Pointer to the root of G4 geometry tree
  G4VPhysicalVolume* constructWorld() override;
  void               constructSDandField() override;

protected:
  // Function to load the geometry into DD4hep. Virtual
  // to allow being replaced in derived classes if needed.
  virtual const dd4hep::Detector& getDetector() const;

private:
  // G4 geometry variables
  G4VPhysicalVolume*               m_world_root{nullptr};
  dd4hep::sim::Geant4GeometryInfo* geoinfo{nullptr};
  std::string                      m_world_material;
  std::string                      m_world_pv_name;
  std::string                      m_world_lv_name;

  Gaudi::Property<bool>                               m_debugMaterials{this, "DebugMaterials", false};
  Gaudi::Property<bool>                               m_debugElements{this, "DebugElements", false};
  Gaudi::Property<bool>                               m_debugShapes{this, "DebugShapes", false};
  Gaudi::Property<bool>                               m_debugVolumes{this, "DebugVolumes", false};
  Gaudi::Property<bool>                               m_debugPlacements{this, "DebugPlacements", false};
  Gaudi::Property<bool>                               m_debugRegions{this, "DebugRegions", false};
  Gaudi::Property<std::map<std::string, std::string>> m_sensdetmappings{this, "SensDetMappings", {}};
  Gaudi::Property<std::string>                        m_mag_field_mgr{this, "FieldManager", ""};
  Gaudi::Property<std::string>                        m_detDescLocation{this, "DescriptionLocation",
                                                 "${DETECTOR_PROJECT_ROOT}/compact/LHCb-no-GDML.xml",
                                                 "Location of the XML detector description"};

  StatusCode register_mag_field( const std::string& name, G4LogicalVolume* );
  StatusCode register_sensitive( const std::string& name, G4LogicalVolume* );

  // Extract the object type name
  template <class T>
  inline const std::string objectTypeName( T object ) {
    return object ? std::string( System::typeinfoName( typeid( *object ) ) ) : std::string( "'UNKNOWN_type'" );
  }

  // Tabulated properties
  template <class T>
  StatusCode copyTableProperties( const T& src_table, G4MaterialPropertiesTable* dst_table );

  // Storage maps to manage assignment of sensdet factories to volumes
  std::map<GiGaFactoryBase<G4VSensitiveDetector>*, std::set<G4LogicalVolume*>> mmap_sensdet_to_lvols;
  std::map<std::string, ToolHandle<GiGaFactoryBase<G4VSensitiveDetector>>>     mmap_name_to_sensdetfac;

  std::atomic_bool                                                             m_found_global_fieldmgr{false};
  std::map<GiGaFactoryBase<G4FieldManager, bool>*, std::set<G4LogicalVolume*>> mmap_fieldmgr_to_lvols;
  std::map<std::string, ToolHandle<GiGaFactoryBase<G4FieldManager, bool>>>     mmap_name_to_fieldmgrfac;
};
