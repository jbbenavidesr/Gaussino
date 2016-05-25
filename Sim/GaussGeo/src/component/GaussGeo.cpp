// Gaudi includes
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartIF.h"

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Solids.h"
#include "DetDesc/TabulatedProperty.h"
// Class IDs
#include "DetDesc/CLIDDetectorElement.h"
#include "DetDesc/Element.h"
#include "DetDesc/Isotope.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/Surface.h"
#include "DetDesc/CLIDLAssembly.h"
#include "DetDesc/CLIDLVolume.h"

// CLHEP
#include "ClhepTools/MathCore2Clhep.h"

// GEANT4
#include "Geant4/G4Element.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4PVPlacement.hh"
// For solids
#include "Geant4/G4Box.hh"
#include "Geant4/G4Cons.hh"
#include "Geant4/G4Sphere.hh"
#include "Geant4/G4Trd.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4Trap.hh"
#include "Geant4/G4Polycone.hh"
// For SolidBoolean
#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4IntersectionSolid.hh"
#include "Geant4/G4UnionSolid.hh"
// For surfaces
#include "Geant4/G4OpticalSurface.hh"
#include "Geant4/G4LogicalSurface.hh"
#include "Geant4/G4LogicalSkinSurface.hh"
#include "Geant4/G4LogicalBorderSurface.hh"
// Other G4
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4FieldManager.hh"
#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4MagIntegratorStepper.hh"
// Used for cleanup in finilization
#include "Geant4/G4GeometryManager.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4PhysicalVolumeStore.hh"
#include "Geant4/G4SolidStore.hh"

// GiGa
#include "GiGa/IGiGaSensDet.h"
#include "GiGa/IGiGaMagField.h"
#include "GiGa/IGiGaFieldMgr.h"
#include "GiGa/IGDMLReader.h"

// Local unclides
#include "GaussGeo.h"
#include "GaussGeoAssembly.h"
#include "GaussGeoAssemblyStore.h"
#include "GaussGeoVolumeUtils.h"

// ============================================================================
// Implementation file for class : GaussGeo
//
// 2015-11-11 : Dmitry Popov
// ============================================================================

DECLARE_SERVICE_FACTORY(GaussGeo)

//=============================================================================
// Standard constructor
//=============================================================================
GaussGeo::GaussGeo(const std::string& service_name, ISvcLocator* service_locator)
  : Service(service_name, service_locator),
    m_property_config_svc(nullptr),
    m_incident_svc(nullptr),
    m_tool_svc(nullptr),
    m_detector_data_svc(nullptr),
    m_geo_items_names(),
    m_geo_items(),
    m_data_selector(),
    m_world_root(nullptr),
    m_mag_field_mgr(""),
    m_sensitive_detectors(),
    m_magnetic_fields(),
    m_mag_field_managers(),
    m_use_alignment(false),
    m_align_all(false),
    m_align_dets_names(),
    m_clear_stores(true),
    m_gdml_readers_names(),
    m_str_prefix(" ") {
  declareProperty("GeoItemsNames", m_geo_items_names);

  declareProperty("WorldPhysicalVolumeName", m_world_pv_name = "Universe");
  declareProperty("WorldLogicalVolumeName", m_world_lv_name = "World");
  declareProperty("WorldMaterial", m_world_material = "/dd/Materials/Air");

  declareProperty("XsizeOfWorldVolume", m_world_volume_size_x = 50. * Gaudi::Units::m);
  declareProperty("YsizeOfWorldVolume", m_world_volume_size_y = 50. * Gaudi::Units::m);
  declareProperty("ZsizeOfWorldVolume", m_world_volume_size_z = 50. * Gaudi::Units::m);

  declareProperty("GlobalSensitivity", m_budget = "");

  declareProperty("FieldManager", m_mag_field_mgr);

  declareProperty("UseAlignment", m_use_alignment = false);
  declareProperty("AlignAllDetectors", m_align_all = false);
  declareProperty("AlignDetectors", m_align_dets_names);

  declareProperty("ClearStores", m_clear_stores = true);

  declareProperty("GdmlReaders", m_gdml_readers_names);
}

//=============================================================================
// Service initialization
//=============================================================================
StatusCode GaussGeo::initialize() {
  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "=> Service Initisalisation Start" << endmsg;
  }

  StatusCode sc = Service::initialize();
  if (sc.isFailure()) {
    return reportError("Service::initialize() failed!", sc);
  }

  // Get IncidentSvc
  if (!service("IncidentSvc", m_incident_svc).isSuccess()) {
    return reportError("GaussGeo failed to get the IncidentSvc!");
  }

  if (svcLocator() == nullptr) {
    return reportError("ServiceLocator is invalid!");
  }

  // Get tool service
  {
    StatusCode sc = svcLocator()->service("ToolSvc", m_tool_svc, true);
    if (sc.isFailure()) {
      return reportError("Unable to locate Tool Service!");
    }
    if (toolSvc() == nullptr) {
      return reportError("ToolSvc is invalid!");
    }
  }

  // Get detector data service
  {
    StatusCode sc = svcLocator()->service("DetectorDataSvc", m_detector_data_svc, true);
    if (sc.isFailure()) {
      return reportError("Unable to locate DetectorDataSvc!", sc);
    }
    if (detectorDataSvc() == nullptr) {
      return reportError("DetectorDataSvc is invalid!", sc);
    }
  }

  // Check for special run with material budget counters
  if (!m_budget.empty()) {
    warning() << "Special run for material budget calculation requested!" << endmsg;
  }

  // Retrieve GDMLReader tools
  {
    StatusCode sc = StatusCode::SUCCESS;
    m_gdml_readers.clear();
    for (const auto& reader : m_gdml_readers_names) {
      if (reader == "") {
        continue;
      }

      IGDMLReader* gdml_tool = nullptr;
      sc = toolSvc()->retrieveTool("GDMLReader/" + reader, gdml_tool, this);
      if (sc.isFailure()) {
        return reportError("Failed to retrieve a GDMLReader tool!", sc);
      }

      if (gdml_tool != nullptr) {
        m_gdml_readers.push_back(gdml_tool);
      }
    }

    if (!m_gdml_readers.empty()) {
      info() << "Retrieved " << m_gdml_readers.size() << " GDMLReader tools..." << endmsg;
    }
  }

  // Print out names of geo items for debugging
  if (outputLevel() == MSG::VERBOSE) {
    verbose() << "List of provided geometry objects names:" << endmsg;
    for (const auto& geo_item_name : m_geo_items_names) {
      verbose() << m_str_prefix << geo_item_name << endmsg;
    }
  }

  // Convert names into objects, put into the list of items
  for (const auto& geo_item_name : m_geo_items_names) {
    verbose() << geo_item_name << endmsg;
    SmartDataPtr<DataObject> obj(detectorDataSvc(), geo_item_name);
    m_data_selector.push_back(obj);
  }

  // Print variables set via Gauss Python Configurable
  info() << "============================================" << endmsg;
  info() << "=  useAlignment: " << useAlignment() << endmsg;
  info() << "=  alignAll: " << alignAll() << endmsg;
  info() << "=  alignDetsNames: " << endmsg;
  if (alignDetsNames().size() != 0) {
    for (const auto& val : alignDetsNames()) {
      info() << "=    " << val << endmsg;
    }
  } else {
    info() << "=    None" << endmsg;
  }
  info() << "============================================" << endmsg;

  // Convert geometry
  {
    StatusCode sc = convertGeometry();
    if (sc.isFailure()) {
      return reportError("Failed to convert geometry!", sc);
    }
  }

  if (outputLevel() == MSG::VERBOSE) {
    printConfiguraion();
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= Service Initisalisation End" << endmsg;
  }

  return sc;
}

//=============================================================================
// Service finilization
//=============================================================================
StatusCode GaussGeo::finalize() {
  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "=> Service Finilization Start" << endmsg;
  }

  // Finalize all sensitive detectors
  for (auto& idet : m_sensitive_detectors) {
    IAlgTool* det = idet;
    if (det != nullptr && toolSvc() != nullptr) {
      toolSvc()->releaseTool(det);
    }
  }
  m_sensitive_detectors.clear();

  // Finalize magnetic field objects
  for (auto& imf : m_magnetic_fields) {
    IAlgTool* mf = imf;
    if (mf != nullptr && toolSvc() != nullptr) {
      toolSvc()->releaseTool(mf);
    }
  }
  m_magnetic_fields.clear();

  // Finalize magnetic field manager objects
  for (auto& imfmgr : m_mag_field_managers) {
    IAlgTool* mfmgr = imfmgr;
    if (mfmgr != nullptr && toolSvc() != nullptr) {
      toolSvc()->releaseTool(mfmgr);
    }
  }
  m_mag_field_managers.clear();

  // Clear assembly store
  GaussGeoAssemblyStore::store()->clear();

  // Delete volumes
  if (m_clear_stores) {
    G4GeometryManager* g4geo_mgr = G4GeometryManager::GetInstance();
    if (g4geo_mgr == nullptr) {
      warning() << "G4GeometryManager* is invalid!" << endmsg;
    } else if (g4geo_mgr->IsGeometryClosed()) {
      g4geo_mgr->OpenGeometry();
      verbose() << "Opened G4 geometry" << endmsg;
    }

    G4LogicalVolumeStore::Clean();
    verbose() << "Cleared G4LogicalVolumeStore..." << endmsg;

    G4PhysicalVolumeStore::Clean();
    verbose() << "Cleared G4PhysicalVolumeStore..." << endmsg;

    G4SolidStore::Clean();
    verbose() << "Cleared G4SolidStore..." << endmsg;
  }

  if (m_incident_svc) {
    m_incident_svc->release();
    m_incident_svc = nullptr;
  }

  if (toolSvc() != nullptr) {
    toolSvc()->release();
    m_tool_svc = nullptr;
  }

  if (detectorDataSvc() != nullptr) {
    detectorDataSvc()->release();
    m_detector_data_svc = nullptr;
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= Service Finilization End" << endmsg;
  }

  return Service::finalize();
}

//=============================================================================
// Incident handler
//=============================================================================
void GaussGeo::handle(const Incident& incident) {
  warning() << "  Incident: '"  + incident .type() << "'\t" << " Src: '" + incident.source() + "'" << endmsg;
}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode GaussGeo::queryInterface(const InterfaceID& iid, void** ppi) {
  if (ppi == nullptr) {
    return StatusCode::FAILURE;
  }

  if (iid == IGiGaGeoSrc::interfaceID()) {
    *ppi = static_cast<IGiGaGeoSrc*>(this);
  } else if (iid == IIncidentListener::interfaceID()) {
    *ppi = static_cast<IIncidentListener*>(this);
  } else if (iid == IService::interfaceID()) {
    *ppi = static_cast<IService*>(this);
  } else if (iid == INamedInterface::interfaceID()) {
    *ppi = static_cast<INamedInterface*>(this);
  } else if (iid == IInterface::interfaceID()) {
    *ppi = static_cast<IInterface*>(this);
  } else {
    return Service::queryInterface(iid, ppi);
  }

  addRef();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Convert geometry
//=============================================================================
StatusCode GaussGeo::convertGeometry() {
  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "=> Starting converting geometry" << endmsg;
  }

  StatusCode exec_status = StatusCode::SUCCESS;
  for (const auto& object : m_data_selector) {
    exec_status = convertGeoObject(object);
    if (exec_status.isFailure()) {
      error() << "convertGeoObject() returned a failure status!" << endmsg;
    }
  }

  // Print out objects counters
  info() << "================================" << endmsg;
  info() << "=    Converted objects"  << endmsg << "=" << endmsg;
  for (const auto& counter : m_objects_counters) {
    info() << "=    " << counter.first << " : \t" << counter.second << endmsg;
  }
  info() << "================================" << endmsg;

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= Finished converting geometry" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Call the specific converter based on the classID of the object
//=============================================================================
StatusCode GaussGeo::convertGeoObject(DataObject* object) {
  if (object == nullptr) {
    return reportError("GeoObject is invalid!");
  }

  if (object->clID() == Surface::classID()) {
    m_objects_counters["Surfaces"]++;
    return convertSurface(object);
  }

  if (object->clID() == CLID_LVolume) {
    m_objects_counters["LVolumes"]++;
    return convertLVolumeObject(object);
  }

  if (object->clID() == Element::classID()) {
    m_objects_counters["Elements"]++;
    return convertElement(object);
  }

  if (object->clID() == Mixture::classID()) {
    m_objects_counters["Mixtures"]++;
    return convertMixture(object);
  }

  if (object->clID() == CLID_LAssembly) {
    m_objects_counters["LAssemblies"]++;
    return convertLAssemblyObject(object);
  }

  if (object->clID() == Isotope::classID()) {
    m_objects_counters["Isotopes"]++;
    return convertIsotope(object);
  }

  if (object->clID() == DataObject::classID()) {
    m_objects_counters["DataObjects"]++;
    return convertCatalog(object);
  }

  // All other objects are treated as DetectorElements
  m_objects_counters["DetElements"]++;
  return convertDetectorElementObject(object);
}

//=============================================================================
// Convert a Detector Element object
//=============================================================================
StatusCode GaussGeo::convertDetectorElementObject(DataObject* object) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> convertDetectorElement(): " << object->registry()->identifier() << endmsg;
  }

  IDetectorElement* det_element = nullptr;
  try {
    det_element = dynamic_cast<IDetectorElement*>(object);
  } catch(std::bad_cast& bc_msg) {
    return reportError("Failed to cast object: '" + object->registry()->identifier() + "', " + bc_msg.what());
  }

  IGeometryInfo* geo_info = det_element->geometry();

  // Check if the element has geometry information, otherwise treat it as a container of DetectorElements
  if (geo_info == nullptr || geo_info->lvolume() == nullptr) {
    warning() << "Element '" << det_element->name()
              << "' has no geometry info, treating it as a container of daughter elements" << endmsg;
    // IDetectorElement::IDEContainer::iterator el_it
    for (auto el_it = det_element->childBegin(); el_it != det_element->childEnd(); ++el_it) {
      DataObject* child_object = nullptr;
      try {
        child_object = dynamic_cast<DataObject*>(*el_it);
      } catch (std::bad_cast& bc_msg) {
        return reportError("Failed to cast daughter object while looping over "
                           + det_element->name() + "'s children: " + bc_msg.what());
      }
      convertGeoObject(child_object);
    }
  }

  if (geo_info == nullptr) {
    return reportError("IGeometryInfo is not available for " + det_element->name());
  }

  const ILVolume* lvolume = geo_info->lvolume();
  if (lvolume == nullptr) {
    return reportError("ILVolume is not available for " + det_element->name());
  }

  const GaussGeoVolume gg_volume = volume(lvolume->name());
  if (!gg_volume.isValid()) {
    return reportError("Failed to convert logical volume " + lvolume->name());
  }

  G4VPhysicalVolume* pvolume = world();
  if (pvolume == nullptr) {
    return reportError("G4WorldPV is not available!");
  }

  StatusCode sc = installVolume(gg_volume,
                                det_element->name(),
                                geo_info->toLocalMatrix(),
                                pvolume->GetLogicalVolume());

  if (sc.isFailure()) {
    return reportError("Failed to place physical volume!", sc);
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= convertDetectorElement()" << endmsg;
    prefixOut();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Convert an Assembly object
//=============================================================================
StatusCode GaussGeo::convertLAssemblyObject(DataObject* object) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> convertLAssembly(): " << object->registry()->identifier() << endmsg;
  }

  const ILVolume* logical_volume = dynamic_cast<const ILVolume*>(object);
  if (logical_volume == nullptr) {
    return reportError("Bad cast to ILVolume!");
  }

  if (!logical_volume->isAssembly()) {
    return reportError("Object '" + object->registry()->identifier() + "' is not an Assembly!");
  }

  GaussGeoAssembly* gg_assembly = new GaussGeoAssembly(logical_volume->name());
  for (auto pv_iter = logical_volume->pvBegin(); logical_volume->pvEnd() != pv_iter; ++pv_iter) {
    const IPVolume* pvolume = *pv_iter;
    if (pvolume == nullptr) {
      return reportError("IPVolume* is invalid!");
    }

    const GaussGeoVolume gg_volume = volume(pvolume->lvolumeName());
    if (!gg_volume.isValid()) {
      return reportError("GaussGeoVolume is invalid!");
    }
    gg_assembly->addVolume(GaussGeoVolumePair(gg_volume, pvolume->matrix()), pvolume->name());
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= convertLAssembly()" << endmsg;
    prefixOut();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Convert a Volume object
//=============================================================================
StatusCode GaussGeo::convertLVolumeObject(DataObject* object) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> convertLVolume(): " << object->registry()->identifier() << endmsg;
  }

  const ILVolume* lvolume = dynamic_cast<const ILVolume*>(object);
  if (lvolume == nullptr) {
    return reportError("Bad cast to ILVolume!");
  }

  // Check if the volume has already been converted
  if (GaussGeoVolumeUtils::findLVolume(lvolume->name()) != nullptr) {
    if (outputLevel() == MSG::VERBOSE) {
      verbose() << m_str_prefix << "<= convertLVolume()" << endmsg;
      prefixOut();
    }
    return StatusCode::SUCCESS;
  }

  G4VSolid* g4_solid = solid(lvolume->solid());
  if (g4_solid == nullptr) {
    return reportError("Failed to create a G4Solid!");
  }

  G4Material* g4_material = material(lvolume->materialName());
  if (g4_material == nullptr) {
    return reportError("Failed to locate G4Material '" + lvolume->materialName() + "'");
  }

  G4LogicalVolume* g4_volume = createG4LVolume(g4_solid, g4_material, lvolume->name());

  // Convert daughters
  StatusCode align_status = StatusCode::SUCCESS;
  for (auto pvolume_iter = lvolume->pvBegin(); lvolume->pvEnd() != pvolume_iter; ++pvolume_iter) {
    const IPVolume* pvolume = *pvolume_iter;
    if (pvolume == nullptr) {
      return reportError("IPVolume* is invalid for " + lvolume->name());
    }

    const GaussGeoVolume gg_volume = volume(pvolume->lvolumeName());
    if (!gg_volume.isValid()) {
      return reportError("Failed to convert DPVolume for " + lvolume->name());
    }

    Gaudi::Transform3D pv_transform_with_alignment = pvolume->matrix();

    // To apply alignment get transformation from the detector element
    if (useAlignment()) {
      align_status = transformWithAlignment(pvolume, pv_transform_with_alignment);
    }

    const Gaudi::Transform3D& pv_transform = pv_transform_with_alignment;
    StatusCode exec_status = installVolume(gg_volume,
                                           lvolume->name() + "#" + pvolume->name(),
                                           pv_transform,
                                           g4_volume);

    if (exec_status.isFailure()) {
      return reportError("Failed to install DPV for " + lvolume->name());
    }
  }

  if (align_status.isFailure()) {
    warning() << "Multiple indistinguishable misalignable detector elements found inside '"
              << lvolume->name() << "'!" << endmsg;
  }

  // Sensitivity
  if (!lvolume->sdName().empty()) {
    if (g4_volume->GetSensitiveDetector() == nullptr) {
      IGiGaSensDet* sens_det = nullptr;
      StatusCode sc = sensitive(lvolume->sdName(), sens_det);

      if (sc.isFailure() || sens_det == nullptr) {
        return reportError("Failed to create SensDet", sc);
      }

      // Set sensitive detector
      g4_volume->SetSensitiveDetector(sens_det);
    } else {
      warning() << "SensDet is already defined!'" << endmsg;
    }
  }

  // Magnetic field
  if (!lvolume->mfName().empty()) {
    IGiGaFieldMgr* fmanager = nullptr;
    StatusCode sc = fieldMgr(lvolume->mfName(), fmanager);
    if (sc.isFailure() || fmanager == nullptr) {
      return reportError("Failed to create FieldMgr!", sc);
    }

    if (fmanager->fieldMgr() == nullptr) {
      return reportError("FieldMgr is invalid!");
    }

    if (fmanager->global()) {
      return reportError("FieldMgr is 'global'!");
    }

    // Set magnetic field manager
    g4_volume->SetFieldManager(fmanager->fieldMgr(), false);
  }

  // Check the G4 static store
  if (GaussGeoVolumeUtils::findLVolume(lvolume->name()) != nullptr) {
    if (outputLevel() == MSG::VERBOSE) {
      verbose() << m_str_prefix << "<= convertLVolume()" << endmsg;
      prefixOut();
    }

    return StatusCode::SUCCESS;
  }

  return reportError("Failed to convert '" + lvolume->name() + "'");
}

//=============================================================================
// Convert a Mixture object
//=============================================================================
StatusCode GaussGeo::convertMixture(DataObject* object) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> convertMixtureObject(): " << object->registry()->identifier() << endmsg;
  }

  Mixture* mixture = dynamic_cast<Mixture*>(object);
  if (mixture == nullptr) {
    error() << "Bad cast to Mixture!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Check if the mixture has been already converted
  const std::string& mixture_name = object->registry()->identifier();
  G4bool g4_warning = false;
  if (G4Material::GetMaterial(mixture_name, g4_warning) != nullptr) {
    if (outputLevel() == MSG::VERBOSE) {
      verbose() << m_str_prefix << "<= convertMixture()" << endmsg;
      prefixOut();
    }

    return StatusCode::SUCCESS;
  }

  // Convert all items
  {
    for (unsigned int index = 0; index < (unsigned int) mixture->nOfItems(); ++index) {
      Element* element = mixture->element(index);
      if (element == nullptr) {
        return reportError("Element is invalid for Mixture: " + mixture_name);
      }

      StatusCode exec_status = convertGeoObject(element);
      if (exec_status.isFailure()) {
        return reportError("Failed to convert elements for Mixture: " + mixture_name, exec_status);
      }
    }
  }

  // Create new material
  G4Material* new_material = nullptr;
  if (mixture->nOfItems() == 0) {
    new_material = new G4Material(mixture_name,
                                  mixture->Z(),
                                  mixture->A(),
                                  mixture->density(),
                                  (G4State)mixture->state(),
                                  mixture->temperature(),
                                  mixture->pressure());
  } else {
    new_material = new G4Material(mixture_name,
                                  mixture->density(),
                                  mixture->nOfItems(),
                                  (G4State)mixture->state(),
                                  mixture->temperature(),
                                  mixture->pressure());

    for (unsigned int index = 0; index < (unsigned int) mixture->nOfItems(); ++index) {
      G4Material* g4_material = G4Material::GetMaterial(mixture->element(index)->registry()->identifier());
      if (g4_material == nullptr) {
        return reportError("Failed to extract material: " + mixture->element(index)->registry()->identifier());
      }
      new_material->AddMaterial(g4_material, mixture->elementFraction(index));
    }
  }

  // Copy properties
  if (!mixture->tabulatedProperties().empty()) {
    if (new_material->GetMaterialPropertiesTable() == nullptr) {
      new_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
    }

    StatusCode exec_status = copyTableProperties(mixture->tabulatedProperties(),
                                                 new_material->GetMaterialPropertiesTable());

    if (exec_status.isFailure()) {
      return reportError("Failed to add TabulatedProperties for: "
                         + mixture_name, exec_status);
    }
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= convertMixture()" << endmsg;
    prefixOut();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Convert an Isotope object
//=============================================================================
StatusCode GaussGeo::convertIsotope(DataObject* object) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> convertIsotope(): " << object->registry()->identifier() << endmsg;
  }

  Isotope* isotope = nullptr;
  try {
    isotope = dynamic_cast<Isotope*>(object);
  } catch(std::bad_cast& bc_msg) {
    return reportError("Failed to cast object: '" + object->registry()->identifier() + "', " + bc_msg.what());
  }

  // Check if the isotope has been already converted
  if (G4Isotope::GetIsotope(isotope->registry()->identifier()) != nullptr) {
    if (outputLevel() == MSG::VERBOSE) {
      verbose() << m_str_prefix << "<= convertIsotope()" << endmsg;
      prefixOut();
    }

    return StatusCode::SUCCESS;
  }

  // Create the new isotope
  // G4Isotope* g4_isotope
  new G4Isotope(isotope->registry()->identifier(),
                static_cast<int>(isotope->Z()),
                static_cast<int>(isotope->N()),
                isotope->A());

  G4bool g4_warning = false;
  if (G4Material::GetMaterial(isotope->registry()->identifier(), g4_warning) != nullptr) {
    if (outputLevel() == MSG::VERBOSE) {
      verbose() << m_str_prefix << "<= convertIsotope()" << endmsg;
      prefixOut();
    }

    return StatusCode::SUCCESS;
  }

  // Create simple material for the new isotope
  G4Material* g4_material = new G4Material(isotope->registry()->identifier(),
                               isotope->Z(),
                               isotope->A(),
                               isotope->density(),
                               (G4State)isotope->state(),
                               isotope->temperature(),
                               isotope->pressure());

std::cout << "======================================================" << std::endl;
std::cout << "======================================================" << std::endl;
std::cout << g4_material->GetName() << "  " << g4_material->GetZ() << "  " << g4_material->GetA() << "  " << g4_material->GetDensity() << std::endl;
std::cout << *(G4Isotope::GetIsotopeTable()) << std::endl;
std::cout << "======================================================" << std::endl;
std::cout << "======================================================" << std::endl;

  // Copy properties
  if (!isotope->tabulatedProperties().empty()) {
    if (g4_material->GetMaterialPropertiesTable() == nullptr) {
      g4_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
    }

    StatusCode exec_status = copyTableProperties(isotope->tabulatedProperties(),
                                                 g4_material->GetMaterialPropertiesTable());

    if (exec_status.isFailure()) {
      return reportError("Failed to add TabulatedProperties for: "
                         + isotope->registry()->identifier(), exec_status);
    }
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= convertIsotope()" << endmsg;
    prefixOut();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Convert a Surface object
//=============================================================================
StatusCode GaussGeo::convertSurface(DataObject* object) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> convertSurface(): " << object->registry()->identifier() << endmsg;
  }

  Surface* surface = dynamic_cast<Surface*>(object);
  if (surface == nullptr) {
    return reportError("Bad cast to Surface!");
  }

  if (surface->firstVol().empty()) {
    return reportError("Surface's firstVol() name is empty!");
  }

  G4LogicalSurface* g4_lsurface = nullptr;
  {
    // Skin surface
    if (surface->secondVol().empty()) {
      // Check G4LogicalVolumeStore
      G4LogicalVolume* g4_lvolume = GaussGeoVolumeUtils::findLVolume(surface->firstVol());
      if (g4_lvolume == nullptr) {
        return reportError("Failed to locate G4LogicalVolume by name '" + surface->firstVol() + "'");
      }

      // Check existing LogicalSkinSurface-s
      G4LogicalSkinSurface* g4_skin_surface = G4LogicalSkinSurface::GetSurface(g4_lvolume);
      if (g4_skin_surface != nullptr) {
        if (std::string(g4_skin_surface->GetName()) != surface->registry()->identifier()) {
          return reportError("Surface with this G4LVolume already exists!");
        }
      }

      // Create a new surface
      if (g4_skin_surface == nullptr) {
        g4_lsurface = new G4LogicalSkinSurface(surface->registry()->identifier(), g4_lvolume, 0);
      } else {
        g4_lsurface = g4_skin_surface;
      }
    } else {
      // Border surface
      G4VPhysicalVolume* pvolume1 = GaussGeoVolumeUtils::findPVolume(surface->firstVol());
      if (pvolume1 == nullptr) {
        return reportError("Failed to locate physical volume '" + surface->firstVol() + "'" );
      }

      G4VPhysicalVolume* pvolume2 = GaussGeoVolumeUtils::findPVolume(surface->secondVol());
      if (pvolume2 == nullptr) {
        return reportError("Failed to locate physical volume '" + surface->secondVol() + "'");
      }

      // Locate logical border surface
      G4LogicalBorderSurface* g4_border_surface = G4LogicalBorderSurface::GetSurface(pvolume1, pvolume2);
      if (g4_border_surface != nullptr) {
        if (std::string(g4_border_surface->GetName()) != surface->registry()->identifier()) {
          return reportError("Surface with these G4VPhysicalVolume-s already exists!");
        }
      }

      if (g4_border_surface == nullptr) {
        g4_lsurface = new G4LogicalBorderSurface(surface->registry()->identifier(), pvolume1, pvolume2, 0);
      } else {
        g4_lsurface = g4_border_surface;
      }
    }
  }

  if (g4_lsurface->GetSurfaceProperty() == nullptr) {
    // Create optical surface
    G4OpticalSurface* optical_surface = new G4OpticalSurface(object->registry()->identifier(),
                                                             (G4OpticalSurfaceModel)surface->model(),
                                                             (G4OpticalSurfaceFinish)surface->finish(),
                                                             (G4SurfaceType)surface->type(),
                                                             surface->value());

    if (optical_surface->GetMaterialPropertiesTable() == nullptr) {
      optical_surface->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
    }

    StatusCode exec_status = copyTableProperties(surface->tabulatedProperties(),
                                                 optical_surface->GetMaterialPropertiesTable());
    if (exec_status.isFailure()) {
      return reportError("Failed to set properties for G4OpticalSurface!");
    }

    g4_lsurface->SetSurfaceProperty(optical_surface);
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= convertSurface()" << endmsg;
    prefixOut();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Convert an Element object
//=============================================================================
StatusCode GaussGeo::convertElement(DataObject* object) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> convertElement(): " << object->registry()->identifier() << endmsg;
  }

  const std::string& element_path = object->registry()->identifier();
  Element* element = dynamic_cast<Element*>(object);
  if (element == nullptr) {
    return reportError("Bad to cast to Element: " + element_path);
  }

  // Check if the element is already known to G4
  G4bool g4_warning = false;
  if (G4Element::GetElement(element_path, g4_warning) != nullptr) {
    if (outputLevel() == MSG::VERBOSE) {
      verbose() << m_str_prefix << "<= convertElement()" << endmsg;
      prefixOut();
    }

    return StatusCode::SUCCESS;
  }

  // Create isotopes
  {
    for (auto& elem_iter : element->isotopes()) {
      StatusCode exec_status = convertGeoObject(elem_iter.second);
      if (exec_status.isFailure()) {
        return reportError("Failed to convert Isotope: " + element_path);
      }
    }
  }

  G4Element* new_element = nullptr;
  // Simple element
  if (element->nOfIsotopes() == 0) {
    new_element = new G4Element(element_path, element->name(), element->Z(), element->A());
  } else {
    // Compound element
    new_element = new G4Element(element_path, element->name(), element->nOfIsotopes());
    for (auto& elem_iter : element->isotopes()) {
      G4Isotope* g4_isotope = G4Isotope::GetIsotope(elem_iter.second->registry()->identifier());
      if (g4_isotope == nullptr) {
        return reportError("Failed to extract Isotope: " + elem_iter.second->registry()->identifier());
      }
      new_element->AddIsotope(g4_isotope, elem_iter.first);
    }
  }

  // Check if Material is already known to G4
  if (G4Material::GetMaterial(element_path, g4_warning) != nullptr) {
    if (outputLevel() == MSG::VERBOSE) {
      verbose() << m_str_prefix << "<= convertElement()" << endmsg;
      prefixOut();
    }

    return StatusCode::SUCCESS;
  }

  // Simple material
  G4Material* new_material = nullptr;
  if (element->nOfIsotopes() == 0) {
    new_material = new G4Material(element_path,
                                  element->Z(),
                                  element->A(),
                                  element->density(),
                                  (G4State)element->state(),
                                  element->temperature(),
                                  element->pressure());
    } else {
      // Compound material
      new_material = new G4Material(element_path,
                                    element->density(),
                                    element->nOfIsotopes(),
                                    (G4State)element->state(),
                                    element->temperature(),
                                    element->pressure());

      for (auto& elem_iter : element->isotopes()) {
        G4Element* g4_element = G4Element::GetElement(elem_iter.second->registry()->identifier());
        if (g4_element == nullptr) {
          return reportError("Failed to extract Element: " + elem_iter.second->registry()->identifier());
        }
        new_material->AddElement(g4_element, elem_iter.first);
      }
    }

  // Copy properties
  if (!element->tabulatedProperties().empty()) {
    if (new_material->GetMaterialPropertiesTable() == nullptr) {
      new_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
    }
    StatusCode exec_status = copyTableProperties(element->tabulatedProperties(),
                                                 new_material->GetMaterialPropertiesTable());
    if (exec_status.isFailure()) {
      return reportError("Failed to add TabulatedProperties for: " + element_path, exec_status);
    }
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= convertElement" << endmsg;
    prefixOut();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Convert a Catalog object
//=============================================================================
StatusCode GaussGeo::convertCatalog(DataObject* object) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> convertCatalog(): " << object->registry()->identifier() << endmsg;
  }

  if (object == nullptr) {
    return reportError("DataObject is invalid!");
  }

  IRegistry* registry = object->registry();
  if (registry == nullptr) {
    return reportError("Object's registry is invalid!");
  }

  SmartIF<IDataManagerSvc> data_manager(registry->dataSvc());
  if (data_manager == nullptr) {
    return reportError("Object's DataService is invalid!");
  }

  std::vector<IRegistry*> leaves;

  StatusCode exec_status = data_manager->objectLeaves(registry, leaves);
  if (exec_status.isFailure()) {
    return reportError("Failed to retrieve leaves!", exec_status);
  }

  for (const auto& leaf : leaves) {
    if (leaf == nullptr) {
      return reportError("IRegistry* for leaf is invalid!");
    }

    SmartDataPtr<DataObject> new_object(registry->dataSvc(), leaf);
    if (!new_object) {
      return reportError("The leaf is not accessible!");
    }

    exec_status = convertGeoObject(new_object);
    if (exec_status.isFailure()) {
      return reportError("Failed to convert leaf object: " + new_object->registry()->identifier(), exec_status);
    }
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= convertCatalog()" << endmsg;
    prefixOut();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Retrieve the pointer to G4 material from G4MaterialTable
//=============================================================================
G4Material* GaussGeo::material(const std::string& material_name) {
  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "material(): " << material_name << endmsg;
  }

  G4bool g4_warning = false;

  // Search the G4MaterialTable
  {
    G4Material* g4_material = G4Material::GetMaterial(material_name, g4_warning);
    if (g4_material != nullptr) {
      return g4_material;
    }
  }

  // The material is not in the G4 table, retrieve it by name and convert
  SmartDataPtr<DataObject> material_object(detectorDataSvc(), material_name);
  if (!material_object) {
    error() << "Failed to create DataObject for: " << material_name << endmsg;
    return nullptr;
  }

  StatusCode exec_status = convertGeoObject(material_object);
  if (exec_status.isFailure()) {
    error() << "Failed to convert: " << material_name << endmsg;
    return nullptr;
  }

  // Search the G4MaterialTable again
  G4Material* g4_material = G4Material::GetMaterial(material_name, g4_warning);
  if (g4_material == nullptr) {
    error() << "Failed to find G4Material for: " << material_name << endmsg;
    return nullptr;
  }

  return g4_material;
}

//=============================================================================
// Retrieve GaussGeo volume/assembly
//=============================================================================
GaussGeoVolume GaussGeo::volume(const std::string& volume_name) {
  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "volume(): " << volume_name << endmsg;
  }

  {
    G4LogicalVolume* lvolume = GaussGeoVolumeUtils::findLVolume(volume_name);
    if (lvolume != nullptr) {
      return GaussGeoVolume(lvolume, 0);
    }
  }

  {
    GaussGeoAssembly* lassembly = GaussGeoVolumeUtils::findLAssembly(volume_name);
    if (lassembly != nullptr) {
      return GaussGeoVolume(0, lassembly);
    }
  }

  SmartDataPtr<DataObject> object(detectorDataSvc(), volume_name);
  if (!object) {
    error() << "There is no DataObject with name '" << volume_name << "'" << endmsg;
    return GaussGeoVolume();
  }

  StatusCode sc = convertGeoObject(object);
  if (sc.isFailure()) {
    error() << "Could not convert '" << volume_name << "'" << endmsg;
    return GaussGeoVolume();
  }


  {
    G4LogicalVolume* lvolume = GaussGeoVolumeUtils::findLVolume(volume_name);
    if (lvolume != nullptr) {
      return GaussGeoVolume(lvolume, 0);
    }
  }

  {
    GaussGeoAssembly* lassembly = GaussGeoVolumeUtils::findLAssembly(volume_name);
    if (lassembly != nullptr) {
      return GaussGeoVolume(0, lassembly);
    }
  }

  error() << "Failed to find GaussGeoVolume with name '" << volume_name << "'" << endmsg;

  return GaussGeoVolume();
}

//=============================================================================
// Retrieve the pointer to G4 Solid
//=============================================================================
G4VSolid* GaussGeo::solid(const ISolid* isolid) {
  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "isolid()" << endmsg;
  }

  if (isolid == nullptr) {
    error() << "ISolid is invalid!" << endmsg;
    return nullptr;
  }

  const std::string solid_type(isolid->typeName());

  // Box
  {
    const SolidBox* solid_box = dynamic_cast<const SolidBox*>(isolid);
    if (solid_type == "SolidBox" && solid_box != nullptr) {
      return new G4Box(solid_box->name(),
                       solid_box->xHalfLength(),
                       solid_box->yHalfLength(),
                       solid_box->zHalfLength());
    }
  }
  // Cons
  {
    const SolidCons* solid_cons = dynamic_cast<const SolidCons*>(isolid);
    if (solid_type == "SolidCons" && solid_cons != nullptr) {
      return new G4Cons(solid_cons->name(),
                        solid_cons->innerRadiusAtMinusZ(),
                        solid_cons->outerRadiusAtMinusZ(),
                        solid_cons->innerRadiusAtPlusZ(),
                        solid_cons->outerRadiusAtPlusZ(),
                        solid_cons->zHalfLength(),
                        solid_cons->startPhiAngle(),
                        solid_cons->deltaPhiAngle());
    }
  }
  // Sphere
  {
    const SolidSphere* solid_sphere = dynamic_cast<const SolidSphere*>(isolid);
    if (solid_type == "SolidSphere" && solid_sphere != nullptr) {
      return new G4Sphere(solid_sphere->name(),
                          solid_sphere->insideRadius(),
                          solid_sphere->outerRadius(),
                          solid_sphere->startPhiAngle(),
                          solid_sphere->deltaPhiAngle(),
                          solid_sphere->startThetaAngle(),
                          solid_sphere->deltaThetaAngle());
    }
  }
  // Trd
  {
    const SolidTrd* solid_trd = dynamic_cast<const SolidTrd*>(isolid);
    if (solid_type == "SolidTrd" && solid_trd != nullptr) {
      return new G4Trd(solid_trd->name(),
                       solid_trd->xHalfLength1(),
                       solid_trd->xHalfLength2(),
                       solid_trd->yHalfLength1(),
                       solid_trd->yHalfLength2(),
                       solid_trd->zHalfLength());
    }
  }
  // Tubs
  {
    const SolidTubs* solid_tubs = dynamic_cast<const SolidTubs*>(isolid);
    if (solid_type == "SolidTubs" && solid_tubs != nullptr) {
      return new G4Tubs(solid_tubs->name(),
                        solid_tubs->innerRadius(),
                        solid_tubs->outerRadius(),
                        solid_tubs->zHalfLength(),
                        solid_tubs->startPhiAngle(),
                        solid_tubs->deltaPhiAngle());
    }
  }
  // Trap
  {
    const SolidTrap* solid_trap = dynamic_cast<const SolidTrap*>(isolid);
    if (solid_type == "SolidTrap" && solid_trap != nullptr) {
      return new G4Trap(solid_trap->name(),
                        solid_trap->zHalfLength(),
                        solid_trap->theta(),
                        solid_trap->phi(),
                        solid_trap->dyAtMinusZ(),
                        solid_trap->dxAtMinusZMinusY(),
                        solid_trap->dxAtMinusZPlusY(),
                        solid_trap->alphaAtMinusZ(),
                        solid_trap->dyAtPlusZ(),
                        solid_trap->dxAtPlusZMinusY(),
                        solid_trap->dxAtPlusZPlusY(),
                        solid_trap->alphaAtPlusZ());
    }
  }
  // Polycone
  {
    const SolidPolycone* solid_polycone = dynamic_cast<const SolidPolycone*>(isolid);
    if (solid_type == "SolidPolycone" && solid_polycone != nullptr) {
      const int num_zplanes = solid_polycone->number();
      double* zplanes = new double[num_zplanes];
      double* r_inner = new double[num_zplanes];
      double* r_outer = new double[num_zplanes];

      for (int i = 0; i < num_zplanes; i++) {
        zplanes[i] = solid_polycone->z(i);
        r_inner[i] = solid_polycone->RMin(i);
        r_outer[i] = solid_polycone->RMax(i);
      }

      G4VSolid* g4_solid = new G4Polycone(solid_polycone->name(),
                                          solid_polycone->startPhiAngle(),
                                          solid_polycone->deltaPhiAngle(),
                                          num_zplanes,
                                          zplanes,
                                          r_inner,
                                          r_outer);
      delete[] zplanes;
      delete[] r_inner;
      delete[] r_outer;

      return g4_solid;
    }
  }
  // Boolean
  {
    const SolidBoolean* solid_bool = dynamic_cast<const SolidBoolean*>(isolid);
    if (solid_bool != nullptr) {
      return solidBoolToG4Solid(solid_bool);
    }
  }
  // Unknown solid
  error() << "Unknown solid: " << solid_type << "/" << isolid->name() << endmsg;

  return nullptr;
}

//=============================================================================
// Convert a DetDesc SolidBoolean into a G4 Solid
//=============================================================================
G4VSolid* GaussGeo::solidBoolToG4Solid(const SolidBoolean* solid_bool) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> solidBoolToG4Solid()" << endmsg;
  }

  if (solid_bool == nullptr) {
    error() << "SolidBoolean is invalid!" << endmsg;
    return nullptr;
  }

  const SolidSubtraction* solid_subtr = dynamic_cast<const SolidSubtraction*>(solid_bool);
  const SolidIntersection* solid_inter = dynamic_cast<const SolidIntersection*>(solid_bool);
  const SolidUnion* solid_union = dynamic_cast<const SolidUnion*>(solid_bool);

  if (solid_subtr == nullptr && solid_inter == nullptr && solid_union == nullptr) {
    error() << "Unknown type of a SolidBoolean: " << solid_bool->name() << endmsg;
    return nullptr;
  }

  if (solid_bool->first() == nullptr) {
    error() << "Wrong main/first solid for SolidBoolean: " << solid_bool->name() << endmsg;
    return nullptr;
  }

  G4VSolid* first_solid = solid(solid_bool->first());
  if (first_solid == nullptr) {
    error() << "Failed to convert the first solid of the SolidBoolean: " << solid_bool->name() << endmsg;
    return nullptr;
  }

  G4VSolid* g4_resulting_solid = first_solid;
  // typedef SolidBoolean::SolidChildrens::const_iterator CI;
  for (auto it = solid_bool->childBegin(); solid_bool->childEnd() != it; ++it) {
    const SolidChild* solid_child = *it;
    G4VSolid* g4_solid_child = solid(solid_child->solid());
    if (g4_solid_child == nullptr) {
      error() << "Failed to convert solid for SolidBoolean: " << solid_bool->name() << endmsg;
    }

    // CLHEP matrix for G4
    HepGeom::Transform3D clhep_matrix = LHCb::math2clhep::transform3D(solid_child->matrix());

    if (solid_subtr != nullptr) {
      double matrix_elems[3][4];
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
          if (fabs(clhep_matrix[i][j]) < 0.0000001) {
              matrix_elems[i][j] = 0.0;
          } else {
            matrix_elems[i][j] = clhep_matrix[i][j];
          }
        }
      }

      CLHEP::HepRep3x3 trep(matrix_elems[0][0], matrix_elems[0][1], matrix_elems[0][2],
                            matrix_elems[1][0], matrix_elems[1][1], matrix_elems[1][2],
                            matrix_elems[2][0], matrix_elems[2][1], matrix_elems[2][2]);

      CLHEP::HepRotation new_rotation;
      new_rotation.set(trep);
      HepGeom::Transform3D new_transform(new_rotation, CLHEP::Hep3Vector(matrix_elems[0][3],
                                                                         matrix_elems[1][3],
                                                                         matrix_elems[2][3]));

      g4_resulting_solid = new G4SubtractionSolid(solid_bool->first()->name() + "-" + solid_child->name(),
                                                  g4_resulting_solid,
                                                  g4_solid_child,
                                                  new_transform.inverse());
    } else if (solid_inter != nullptr) {
      g4_resulting_solid = new G4IntersectionSolid(solid_bool->first()->name() + "*" + solid_child->name(),
                                                   g4_resulting_solid,
                                                   g4_solid_child,
                                                   clhep_matrix.inverse());
    } else if (solid_union != nullptr) {
      g4_resulting_solid = new G4UnionSolid(solid_bool->first()->name() + "+" + solid_child->name(),
                                            g4_resulting_solid,
                                            g4_solid_child,
                                            clhep_matrix.inverse());
    } else {
      error() << "Unknown type of a SolidBoolean: " << solid_bool->name() << endmsg;
      return nullptr;
    }
  }

  g4_resulting_solid->SetName(solid_bool->name());

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= solidBoolToG4Solid" << endmsg;
    prefixOut();
  }

  return g4_resulting_solid;
}

//=============================================================================
// Retrieve the pointer to G4 geometry tree root
//=============================================================================
G4VPhysicalVolume* GaussGeo::world() {
  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "world()" << endmsg;
  }

  if (m_world_root != nullptr) {
    return m_world_root;
  }

  G4Material* world_material = material(m_world_material);
  if (world_material == nullptr) {
    error() << "Failed to locate/convert world material " << m_world_material << endmsg;
    return nullptr;
  }

  G4VSolid* world_solid_box = new G4Box("WorldBox", m_world_volume_size_x,
                                                    m_world_volume_size_y,
                                                    m_world_volume_size_z);

  G4LogicalVolume* world_logical_volume = createG4LVolume(world_solid_box, world_material, m_world_lv_name);
  world_logical_volume->SetVisAttributes(G4VisAttributes::Invisible);

  m_world_root = new G4PVPlacement(0, CLHEP::Hep3Vector(), m_world_pv_name, world_logical_volume, 0, false, 0);

  // Import GDML geometry
  for (const auto& reader : m_gdml_readers) {
    reader->import(m_world_root);
  }

  if (!m_mag_field_mgr.empty()) {
    IGiGaFieldMgr* giga_fmanager = nullptr;
    StatusCode sc = fieldMgr(m_mag_field_mgr, giga_fmanager);
    if (sc.isFailure() || giga_fmanager == nullptr) {
      error() << "Faild to construct GiGaFieldManager '" << m_mag_field_mgr << "'" << endmsg;
      return nullptr;
    }

    if (!giga_fmanager->global()) {
      error() << "GiGaFieldManager '" + m_mag_field_mgr + "' is not 'global'" << endmsg;
      return nullptr;
    }

    if (giga_fmanager->fieldMgr() ==  nullptr) {
      error() << "GiGaFieldManager '" + m_mag_field_mgr + "' has invalid G4FieldManager" << endmsg;
      return nullptr;
    }

    if (giga_fmanager->field() == nullptr) {
      error() << "GiGaFieldManager '" + m_mag_field_mgr + "' has invalid G4MagneticField" << endmsg;
      return nullptr;
    }

    if (giga_fmanager->stepper() == nullptr) {
      error() << "GiGaFieldManager '" + m_mag_field_mgr + "' has invalid G4MagIntegratorStepper" << endmsg;
      return nullptr;
    }

    // Additional check
    G4TransportationManager* trans_manager = G4TransportationManager::GetTransportationManager();
    if (trans_manager == nullptr) {
      error() << "G4TransportationMgr is invalid!" << endmsg;
      return nullptr;
    }

    if (trans_manager->GetFieldManager() != giga_fmanager->fieldMgr()) {
      error() << "Mismatch in the G4FieldManager!" << endmsg;
      return nullptr;
    }

    info() << "Global 'Field Manager' is set to '" << objectTypeName(giga_fmanager)
           << "/" << giga_fmanager->name() << "'" << endmsg;
    info() << "Global 'G4Field Manager' is set to '" << objectTypeName(giga_fmanager->fieldMgr()) << "'" << endmsg;
    info() << "Global 'G4MagneticField' is set to '" << objectTypeName(giga_fmanager->field()) << "'" << endmsg;
    info() << "Global 'G4MagIntegratorStepper' is set to '" << objectTypeName(giga_fmanager->stepper()) << "'" << endmsg;
  } else {
    warning() << "Magnetic Field was not requested to be loaded" << endmsg;
  }

  return m_world_root;
}

//=============================================================================
// Retrieve the pointer to G4 geometry tree root
// Obsolete method, for compatibility with IGiGaGeoSrc interface
//=============================================================================
G4VPhysicalVolume* GaussGeo::G4WorldPV() {
  warning() << "G4WorldPV() is obsolete, use world()!" << endmsg;

  return world();
}

//=============================================================================
// Create a new G4LogicalVolume
//=============================================================================
G4LogicalVolume* GaussGeo::createG4LVolume(G4VSolid* g4_solid, G4Material* g4_material, const std::string& volume_name) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> createG4LVolume()" << endmsg;
  }

  if (g4_solid == nullptr) {
    error() << "Provided G4VSolid is invalid!" << endmsg;
    return nullptr;
  }

  if (g4_material == nullptr) {
    error() << "Provided G4Material is invalid!" << endmsg;
    return nullptr;
  }

  if (GaussGeoVolumeUtils::findLVolume(volume_name) != nullptr) {
    error() << "G4LogicalVolume with name '" << volume_name << "' already exists!" << endmsg;
    return nullptr;
  }

  G4LogicalVolume* g4_lvolume = new G4LogicalVolume(g4_solid, g4_material, volume_name, 0, 0, 0);

  // Look for global material budget counter
  if (!m_budget.empty()) {
    IGiGaSensDet* budget = 0;
    StatusCode sc = sensitive(m_budget, budget);

    if (sc.isFailure() || budget == nullptr) {
      error() << "Failed to get Material Budget '" << m_budget << "'" << endmsg;
    } else {
      g4_lvolume->SetSensitiveDetector(budget);
    }
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= createG4LVolume()" << endmsg;
    prefixOut();
  }

  return g4_lvolume;
}

//=============================================================================
// Print error message and return status code
//=============================================================================
StatusCode GaussGeo::reportError(const std::string& message, const StatusCode& status) const {
  error() << message << endmsg;
  return status;
}

//=============================================================================
// Print GaussGeo configuration
//=============================================================================
void GaussGeo::printConfiguraion() const {
  const auto& properties = this->getProperties();
  debug() << "List of ALL properties of " << System::typeinfoName(typeid(*this))
          << "/" << this->name() << "  #properties = " << properties.size() << endmsg;

  for (const auto& property_item : properties) {
    debug() << "Property ['Name': Value] = " << (property_item) << endmsg;
  }
}

//=============================================================================
// Copy tabulated properties from one object's properties table to the other
//=============================================================================
template<class T>
StatusCode GaussGeo::copyTableProperties(const T& src_table, G4MaterialPropertiesTable* dst_table) {
  if (dst_table == nullptr) {
    return StatusCode::FAILURE;
  }

  for (const auto& property : src_table) {
    if (property != 0 || !property->table().empty()) {
      const auto& ptable = property->table();

      const auto ptable_size = ptable.size();
      double* vx = new double[ptable_size];
      double* vy = new double[ptable_size];

      for (unsigned int index = 0; index < ptable_size; ++index) {
          vx[index] = ptable[index].first;
          vy[index] = ptable[index].second;
      }

      dst_table->AddProperty(property->type().c_str(), vx, vy, ptable_size);

      delete[] vx;
      delete[] vy;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Install GaussGeoVolume
//=============================================================================
StatusCode GaussGeo::installVolume(const GaussGeoVolume& gg_volume, const std::string& name,
                                   const Gaudi::Transform3D& matrix, G4LogicalVolume* mother_volume) {
  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "installVolume()" << endmsg;
  }

  if (!gg_volume.isValid()) {
    return reportError("Failed to install volume - GaussGeoVolume is invalid!");
  }

  if (mother_volume == nullptr) {
    return reportError("Mother volume is invalid!");
  }

  if (gg_volume.volume() != nullptr) {
    return installVolume(gg_volume.volume(),
                         name,
                         matrix,
                         mother_volume);
  }

  if (gg_volume.assembly() != nullptr) {
    return installVolume(gg_volume.assembly(),
                         name,
                         matrix,
                         mother_volume);
  }

  return reportError("Failed to install GaussGeoVolume!");
}

//=============================================================================
// Install GaussGeoAssembly
//=============================================================================
StatusCode GaussGeo::installVolume(const GaussGeoAssembly* gg_assembly, const std::string& name,
                                   const Gaudi::Transform3D& matrix, G4LogicalVolume* mother_volume) {
  if (gg_assembly == nullptr) {
    return reportError("GaussGeoAssembly is invalid!");
  }

  if (mother_volume == nullptr) {
    return reportError("Mother volume is invalid!");
  }

  for (const auto& vol_iter : gg_assembly->volumes()) {
    G4LogicalVolume* g4_volume = vol_iter.first.first;
    if (g4_volume == nullptr) {
      return reportError("Failed to install volume - corrupted assembly: " + gg_assembly->name());
    }

    StatusCode exec_status = installVolume(g4_volume, name + "#" + vol_iter.first.second,
                                           vol_iter.second * matrix, mother_volume);

    if (exec_status.isFailure()) {
      return reportError("Failed to install volume - recursion error!", exec_status);
      }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Install G4LogicalVolume
//=============================================================================
StatusCode GaussGeo::installVolume(G4LogicalVolume* g4_volume, const std::string& name,
                                   const Gaudi::Transform3D& matrix, G4LogicalVolume* mother_volume) {
  if (g4_volume == nullptr) {
    return reportError("G4 volume is invalid!");
  }

  if (mother_volume == nullptr) {
    return reportError("Mother volume is invalid!");
  }

  // Get the copy number
  int copy_num = 1000 + mother_volume->GetNoDaughters();
  {
    const std::string::size_type position = name.find(':');
    if (std::string::npos != position) {
      std::string tmp_string(name, position + 1, std::string::npos);
      copy_num = atoi(tmp_string.c_str());
    }
  }

  // Create placement
  HepGeom::Transform3D transform_matrix = LHCb::math2clhep::transform3D(matrix);
  new G4PVPlacement(transform_matrix.inverse(), g4_volume, name, mother_volume, false, copy_num);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Instantiate a Sensitive Detector object
//=============================================================================
StatusCode GaussGeo::sensitive(const std::string& name, IGiGaSensDet*& detector) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> sensitive(): " << name << endmsg;
  }

  // Reset the output value
  detector = nullptr;

  // Locate the detector
  detector = getTool(name, detector, this);
  if (detector == nullptr) {
    return reportError("Failed to locate Sensitive Detector '" + name + "'");
  }

  // Inform G4 sensitive detector manager
  if (std::find(m_sensitive_detectors.begin(), m_sensitive_detectors.end(), detector) == m_sensitive_detectors.end()) {
    G4SDManager* sd_manager = G4SDManager::GetSDMpointer();
    if (sd_manager == nullptr) {
      return reportError("Failed to locate G4SDManager!");
    }
    sd_manager->AddNewDetector(detector);
  }

  // Keep a local copy
  m_sensitive_detectors.push_back(detector);

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= sensitive()" << endmsg;
    prefixOut();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Instantiate a Magnetic Field object
//=============================================================================
StatusCode GaussGeo::fieldMgr(const std::string& name, IGiGaFieldMgr*& fmanager) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> fieldMgr(): " << name << endmsg;
  }

  // Reset the output value
  fmanager = nullptr;

  // Locate the magnetic field tool
  fmanager = getTool(name, fmanager, this);
  if (fmanager == nullptr) {
    return reportError("Failed to locate FieldManager '" + name + "'");
  }

  // Keep a local copy
  m_mag_field_managers.push_back(fmanager);

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= fieldMgr()" << endmsg;
    prefixOut();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Retrieve a tool by type and name
//=============================================================================
template<class T>
T* GaussGeo::getTool(const std::string& type, const std::string& name, T*& tool, const IInterface* parent,
                     bool create) const {
  if (toolSvc() == nullptr) {
    fatal() << "ToolService is invalid!" << endmsg;
    return nullptr;
  }

  if (name.empty()) {
    return getTool(type, tool, parent, create);
  }

  StatusCode sc = toolSvc()->retrieveTool(type, name, tool, parent, create);
  if (sc.isFailure() || tool == nullptr) {
    fatal() << "Failed to retrieve tool '" << type << "'/'" << name << "'" << endmsg;
  }

  return tool;
}

//=============================================================================
// Retrieve a tool by type
//=============================================================================
template<class T>
T* GaussGeo::getTool(const std::string& type, T*& tool, const IInterface* parent, bool create) const {
  if (toolSvc() == nullptr) {
    fatal() << "ToolService is invalid!" << endmsg;
    return nullptr;
  }

  // Check if it is "type" or "type/name"
  const auto iter = std::find(type.begin(), type.end(), '/');
  if (iter != type.end()) {
    std::string::size_type position = iter - type.begin();
    const std::string new_type(type, 0, position);
    const std::string new_name(type, position + 1, std::string::npos);
    return getTool(new_type, new_name, tool, parent, create);
  }

  StatusCode sc = toolSvc()->retrieveTool(type, tool, parent, create);
  if (sc.isFailure() || tool == nullptr) {
    fatal() << "Failed to retrieve tool '" << type << "'" << endmsg;
  }

  return tool;
}

//=============================================================================
// Provide matrix transformation of physical volume taking into account
// misalignment in detector elements. Works only if parent is unique!
//=============================================================================
StatusCode GaussGeo::transformWithAlignment(const IPVolume* pvolume, Gaudi::Transform3D& result_matrix) {
  if (outputLevel() == MSG::VERBOSE) {
    prefixIn();
    verbose() << m_str_prefix << "=> transformWithAlignment(): " << pvolume->name() << endmsg;
  }

  StatusCode sc = StatusCode::SUCCESS;
  result_matrix = pvolume->matrix();

  std::vector<const IDetectorElement*> found_detelem;
  int detelem_num = 0;

  if (detectorDataSvc() == nullptr) {
    return reportError("Detector data service is invalid!");
  }

  SmartDataPtr<IDetectorElement> cave(detectorDataSvc(), "/dd/Structure/LHCb");
  if (alignAll()) {
    detelem_num = detElementByLVNameWithAlignment(cave, pvolume->lvolumeName(), found_detelem);
  } else {
    // Loop over the path of DetectorElements to align
    for (const auto& det_name : alignDetsNames()) {
      SmartDataPtr<IDetectorElement> check_detelem(detectorDataSvc(), det_name);
      detelem_num = detElementByLVNameWithAlignment(check_detelem, pvolume->lvolumeName(), found_detelem);
    }
  }

  // One or more det elem found
  if (detelem_num >= 1) {
    int detelem_index = 0;
    int detelem_num_after_npath_search = 0;

    if (detelem_num > 1) {
      detelem_index = findBestDetElemFromPVName(pvolume->name(), found_detelem, detelem_num_after_npath_search);
    }

    if (detelem_num_after_npath_search <= 1) {
      const Gaudi::Transform3D& delta_matrix = found_detelem[detelem_index]->geometry()->ownToNominalMatrix();

      result_matrix = delta_matrix * (pvolume->matrix());
       // // begin test
       // if (found_detelem[detelem_index]->name() == "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0") {
       //   info() << " Now for " << pvolume->name() << "  " << pvolume->lvolumeName()
       //          << "  " << found_detelem[detelem_index]->name() << endmsg;
       //   info() << "Deltamatrix for alignment for " << pvolume->name() << "   "
       //          << delta_matrix << endmsg;
       //   info() << "Resultmatrix for alignment " << result_matrix << endmsg;
       //   DataObject* obj = nullptr;
       //   detectorDataSvc()->findObject("Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0", obj);
       //   IDetectorElement* de = dynamic_cast<IDetectorElement *>(obj);
       //   if (de != nullptr) {
       //     info() << de->name() << endmsg;
       //     info() << de->geometry()->matrix() << endmsg;
       //     info() << de->geometry()->alignmentCondition()->name() << endmsg;
       //     info() << de->geometry()->alignmentCondition()->matrix() << endmsg;
       //     info() << de->geometry()->alignmentCondition()->toXml("dummy") << endmsg;
       //     info() << de->geometry()->toGlobal(Gaudi::XYZPoint()) << endmsg;
       //   }
       // } // end of test
    } else {
      // multiple det elem found even after npath search with physvol name.
      // For now use the ideal geometry as the result.
      // A warning is printed at the end of converting the mother log vol.
      sc = StatusCode::FAILURE;
    }
  }

  if (outputLevel() == MSG::VERBOSE) {
    verbose() << m_str_prefix << "<= transformWithAlignment()" << endmsg;
    prefixOut();
  }

  return sc;
}

//===============================================================================
// Get the list of detector elemnts which are valid, have geomentry, have logical
// volume, corresponds (by name) to a given logical volume, have AlignmentConditions
//===============================================================================
int GaussGeo::detElementByLVNameWithAlignment(const IDetectorElement* det_element, const std::string& lvolume_name,
                                              std::vector<const IDetectorElement*>& det_elements) {
  int result = 0;

  if (det_element == nullptr) {
    return result;
  }

  const IGeometryInfo* geo_info = det_element->geometry();

  // test of the method
  // std::string::size_type posa = lvolume_name.find("/dd/Geometry/BeforeMagnetRegion/Rich1");
  // if (posa != std::string::npos) {
  //   if (geo_info->alignmentCondition() > 0) {
  //     info() << "Det elem search lvname geolvname " << lvolume_name << "  "
  //            << geo_info->lvolumeName()<< "  " << det_element->name() << "  "
  //            << geo_info->hasLVolume() << "  " << geo_info->hasSupport() << "  "
  //            << geo_info->alignmentCondition() << endmsg;
  //   }
  // }
  // end of test

  if (geo_info != nullptr && geo_info->hasLVolume() && geo_info->hasSupport()
      && geo_info->alignmentCondition() != nullptr && geo_info->lvolumeName() == lvolume_name) {
    det_elements.push_back(det_element);
    ++result;
  }

  const auto& de_children = det_element->childIDetectorElements();
  for (const auto& ichild : de_children) {
    result += detElementByLVNameWithAlignment(ichild, lvolume_name, det_elements);
  }

  return result;
}

//=============================================================================
// Search best detector element in TDS provided the name of a physical volume
//=============================================================================
int GaussGeo::findBestDetElemFromPVName(std::string pv_name, std::vector<const IDetectorElement*> found_detelem,
                                        int& found_detelems_num) {
  found_detelems_num = 0;
  int index = 0;

  debug() << "For misalignment Multiple DetElem-s for phys vol name =  " << pv_name << endmsg;

  for (auto det = found_detelem.begin(); det != found_detelem.end(); ++det) {
    std::string de_support_path_name = "";
    StatusCode sc = detectorElementSupportPath((*det), de_support_path_name);

    if (sc.isSuccess()) {
      std::string::size_type position = de_support_path_name.rfind(pv_name);

      debug() << "PhysVol search for misalignment  " << de_support_path_name << "   " << pv_name << endmsg;

      if (position != std::string::npos) {
        int expected_position = de_support_path_name.length() - pv_name.length();
        if (expected_position == (int)position) {
          found_detelems_num++;
          index = det - found_detelem.begin();

          if (found_detelems_num > 1) {
            debug() << position << "  " << de_support_path_name << "  " << pv_name << "  " << found_detelems_num
                    << "  " << (*found_detelem.begin())->name() << endmsg;
          }
        }
      }
    }
  }

  if (found_detelems_num > 1) {
    debug() << " PhysVol Alignment: Too many DetElem-s: " << found_detelems_num << " for  "
            << pv_name << std::endl << " Please Check xmldb for  " << (*found_detelem.begin())->name() << endmsg;
  }

  return index;
}

//=========================================================================
// Get the PV path of the given DE
// method temporarily copied from DetElmFinder on 12-03-2007. SE, M.Cl.
//=========================================================================
StatusCode GaussGeo::detectorElementSupportPath(const IDetectorElement* det_element, std::string& path,
                                                const std::string& parent_path) {
  StatusCode sc = StatusCode::SUCCESS;

  // Initialize the output variable
  path = parent_path;

  // Get the parent geometryInfo
  IGeometryInfo* parent_geo_info = det_element->geometry()->supportIGeometryInfo();
  if (NULL == parent_geo_info) {
    return sc;
  }

  // If the parent path was not specified, I have to find it recursively
  if (parent_path.empty()) {
    if (det_element->parentIDetectorElement()) {
      sc = detectorElementSupportPath(det_element->parentIDetectorElement(), path);
      if (sc.isFailure()) {
        return sc;
      }
    } else {  // this should never happen
      return sc;
    }
  }

  // Get support LVolume
  const ILVolume* support_lvolume = parent_geo_info->lvolume();

  // Define a PVolumePath (this is simply std::vector<const IPVolume*>)
  ILVolume::PVolumePath volume_path;

  // Fill it with PVolume* corresponding to the supportPath()
  sc = support_lvolume->traverse(det_element->geometry()->supportPath().begin(),
                     det_element->geometry()->supportPath().end(),
                     volume_path);

  if (sc.isFailure()) {
    return reportError("DetElem search: Cannot traverse the support path '" + det_element->name() + "'", sc);
  }

  // Build the string
  for (const auto& vpath : volume_path) {
    path += "/" + vpath->name();
  }

  return sc;
}
