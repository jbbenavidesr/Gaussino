// Geant
#include "GiGaMTDD4Hep/DD4hepDetectorConstruction.h"
#include <functional>

// DD4Hep
#include "DD4hep/Detector.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4Mapping.h"

#include "Geant4/G4VUserDetectorConstruction.hh"

DD4hepDetectorConstruction::DD4hepDetectorConstruction(
    const dd4hep::Detector& detector)
    : m_detector(detector){}

DD4hepDetectorConstruction::DD4hepDetectorConstruction()
    : m_detector(dd4hep::Detector::getInstance()){}

G4VPhysicalVolume* DD4hepDetectorConstruction::Construct() {
  // TODO: Review implementation which is currently just copied from
  // Geant4DetectorGeometryConstruction::constructGeo
  static G4VPhysicalVolume* w{nullptr};
  if(w) return w;
  auto& g4map = dd4hep::sim::Geant4Mapping::instance();
  dd4hep::DetElement world = m_detector.world();
  dd4hep::sim::Geant4Converter conv(dd4hep::Detector::getInstance(), m_printlevel);
  conv.debugMaterials = m_debugMaterials;
  conv.debugElements = m_debugElements;
  conv.debugShapes = m_debugShapes;
  conv.debugVolumes = m_debugVolumes;
  conv.debugPlacements = m_debugPlacements;
  conv.debugRegions = m_debugRegions;

  dd4hep::sim::Geant4GeometryInfo* geoinfo = conv.create(world).detach();

  g4map.attach(geoinfo);
  w = geoinfo->world();
  // Create Geant4 volume manager only if not yet available
  g4map.volumeManager();
  return w;
}
