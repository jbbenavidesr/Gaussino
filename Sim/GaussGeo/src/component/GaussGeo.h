#ifndef  GAUSSGEO_GAUSSGEO_H_
#define  GAUSSGEO_GAUSSGEO_H_

// Kernel includes
#include "Kernel/IPropertyConfigSvc.h"

// Gaudi includes
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/IDataSelector.h"

// GiGa Geo Interface
#include "GiGa/IGiGaGeoSrc.h"

// Forward declaration
class GaussGeoVolume;
class GaussGeoAssembly;

// ============================================================================
// Interface file for class : GaussGeo
//
// Conversion service for convertion Gaudi detector and geometry description
// into GEANT4 format. Based on GiGaGeo convertion service.
//
// 2015-11-11 : Dmitry Popov
// ============================================================================

class GaussGeo : public Service,
                 virtual public IIncidentListener,
                 virtual public IGiGaGeoSrc {
 public:
  GaussGeo(const std::string& service_name, ISvcLocator* service_locator);
  virtual ~GaussGeo() {}

  // Service pure virtual member functions
  StatusCode initialize();
  StatusCode finalize();

  StatusCode queryInterface(const InterfaceID& iid, void** ppi);

  // IncidentListener interface
  void handle(const Incident& incident);

  // Pointer to the root of G4 geometry tree
  G4VPhysicalVolume* world();
  G4VPhysicalVolume* G4WorldPV();  // Obsolete method for IGiGeGeoSrc

 private:
  GaussGeo();
  GaussGeo(const GaussGeo&);
  GaussGeo& operator=(const GaussGeo&);

  // Services variables
  IPropertyConfigSvc * m_property_config_svc;
  IIncidentSvc * m_incident_svc;
  IToolSvc * m_tool_svc;
  IDataProviderSvc * m_detector_data_svc;

  // Geometry objects
  std::vector<std::string> m_geo_items_names;
  std::vector<DataStoreItem> m_geo_items;
  IDataSelector m_data_selector;

  // G4 geometry variables
  G4VPhysicalVolume* m_world_root;
  std::string m_world_material;
  std::string m_world_pv_name;
  std::string m_world_lv_name;

  float m_world_volume_size_x;
  float m_world_volume_size_y;
  float m_world_volume_size_z;

  std::string m_mag_field_mgr;

  std::string m_budget;  // Special sensitive detector for estimation of material budget

  std::vector<IGiGaSensDet*>  m_sensitive_detectors;
  std::vector<IGiGaMagField*> m_magnetic_fields;
  std::vector<IGiGaFieldMgr*> m_mag_field_managers;

  bool m_use_alignment;  // Flag to switch on use of condDB info for children detector elements
  bool m_align_all;  // Flag to switch on for which detector to use condDB info for children detector elements
  std::vector<std::string> m_align_dets_names;  // List of paths in TES to which to apply condDB info

  bool m_clear_stores;

  std::map <std::string, unsigned int> m_objects_counters;  // Counters by object class ID

  // GDML readers to be called when creating world volume
  std::vector<std::string> m_gdml_readers_names;
  std::vector<IGDMLReader*> m_gdml_readers;

  // For verbose info printout beautification
  std::string m_str_prefix;

  // Services accessors
  inline ISvcLocator * svcLocator() const { return serviceLocator(); }
  inline IToolSvc * toolSvc() const { return m_tool_svc; }
  inline IDataProviderSvc * detectorDataSvc() const { return m_detector_data_svc; }
  inline bool useAlignment() { return m_use_alignment; }
  inline bool alignAll() { return m_align_all; }
  inline const std::vector<std::string>& alignDetsNames() { return m_align_dets_names; }

  // Conversion of geo items
  StatusCode convertGeometry();
  StatusCode convertGeoObject(DataObject* object);
  StatusCode convertDetectorElementObject(DataObject* object);
  StatusCode convertLVolumeObject(DataObject* object);
  StatusCode convertLAssemblyObject(DataObject* object);
  StatusCode convertMixture(DataObject* object);
  StatusCode convertIsotope(DataObject* object);
  StatusCode convertSurface(DataObject* object);
  StatusCode convertElement(DataObject* object);
  StatusCode convertCatalog(DataObject* object);

  // Methods for returning G4 objects
  G4Material* material(const std::string& material_name);
  GaussGeoVolume volume(const std::string& volume_name);
  G4VSolid* solid(const ISolid* isolid);
  G4VSolid* solidBoolToG4Solid(const SolidBoolean* solid_bool);
  G4LogicalVolume* createG4LVolume(G4VSolid* g4_solid, G4Material* g4_material, const std::string& volume_name);

  // Misc utilities
  StatusCode reportError(const std::string& message, const StatusCode& status = StatusCode::FAILURE) const;
  void printConfiguraion() const;
  inline void prefixIn() { m_str_prefix.resize(m_str_prefix.size() + 1, ' '); }
  inline void prefixOut() { if (m_str_prefix.size() > 0) { m_str_prefix.resize(m_str_prefix.size() - 1); } }

  // Extract the object type name
  template <class T>
  inline const std::string objectTypeName(T object) {
    return object ? std::string(System::typeinfoName(typeid(*object))) : std::string("'UNKNOWN_type'");
  }

  // Tabulated properties
  template<class T>
  StatusCode copyTableProperties(const T& src_table, G4MaterialPropertiesTable* dst_table);

  // Volumes installation
  StatusCode installVolume(const GaussGeoVolume& gg_volume, const std::string& name,
                           const Gaudi::Transform3D& matrix, G4LogicalVolume* mother_volume);
  StatusCode installVolume(const GaussGeoAssembly* gg_assembly, const std::string& name,
                           const Gaudi::Transform3D& matrix, G4LogicalVolume* mother_volume);
  StatusCode installVolume(G4LogicalVolume* g4_volume, const std::string& name,
                           const Gaudi::Transform3D& matrix, G4LogicalVolume* mother_volume);

  StatusCode sensitive(const std::string& name, IGiGaSensDet*& detector);
  StatusCode fieldMgr(const std::string& name, IGiGaFieldMgr*& fmanager);

  // Methods to simplify tools retrieval
  template<class T>
  T* getTool(const std::string& type, const std::string& name, T*& tool, const IInterface* parent = 0, bool create = true) const;
  template<class T>
  T* getTool(const std::string& type, T*& tool, const IInterface* parent = 0, bool create = true) const;

  // Methods to handle missalignment for the volumes conversion method
  StatusCode transformWithAlignment(const IPVolume* pvolume, Gaudi::Transform3D& result_matrix);
  int detElementByLVNameWithAlignment(const IDetectorElement* det_element, const std::string& lvolume_name,
                                      std::vector<const IDetectorElement*>& det_elements);
  int findBestDetElemFromPVName(std::string pv_name, std::vector<const IDetectorElement*> found_detelem, int& found_detelems_num);
  StatusCode detectorElementSupportPath(const IDetectorElement* det_elem, std::string& path, const std::string& parent_path = "");
};

#endif  // GAUSSGEO_GAUSSGEO_H_
