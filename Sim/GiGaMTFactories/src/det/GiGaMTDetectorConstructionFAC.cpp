#include "GiGaMTDetectorConstructionFAC.h"
#include "GiGaMTCoreDet/GiGaMTDetectorConstruction.h"
#include "GiGaMTCoreDet/GiGaMTProxyDetectorConstruction.h"
#include "GiGaMTGeo/IGiGaMTGeoSvc.h"
#include "SimInterfaces/IGaussinoTool.h"

DECLARE_COMPONENT( GiGaMTDetectorConstructionFAC )
DECLARE_COMPONENT( GiGaMTProxyDetectorConstructionFAC )

StatusCode GiGaMTDetectorConstructionFAC::initialize() {
  auto sc = extends::initialize();
  // Retrieve the factory tools here to avoid the retrieval happening in multiple
  // threads
  for ( auto& keypairs : m_sens_dets ) { keypairs.second.retrieve(); }
  return sc;
}
G4VUserDetectorConstruction* GiGaMTDetectorConstructionFAC::construct() const {
  auto detconst = new GiGaMTDetectorConstruction();
  detconst->SetWorld( m_geoSvc->constructWorld() );

  return detconst;
}

G4VUserDetectorConstruction* GiGaMTProxyDetectorConstructionFAC::construct() const {
  auto detconst = new GiGaMTProxyDetectorConstruction();
  detconst->SetWorldConstructor( [&]() {
    debug() << "Calling world constructor" << endmsg;
    auto world = m_geoSvc->constructWorld();
    for ( auto& tool : m_afterGeo ) { tool->process(); }
    return world;
  } );
  detconst->SetSDConstructor( [&]() {
    debug() << "Calling SD and Field constructor" << endmsg;
    m_geoSvc->constructSDandField();
    DressVolumes();
  } );

  return detconst;
}

#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4SDManager.hh"

void GiGaMTDetectorConstructionFAC::DressVolumes() const {
  auto sdmanager = G4SDManager::GetSDMpointer();
  for ( auto& [name, volumes] : m_namemap ) {
    auto& tool    = m_sens_dets.at( name );
    auto  sensdet = tool->construct();
    sdmanager->AddNewDetector( sensdet );
    for ( auto& volname : volumes ) {
      auto vol = G4LogicalVolumeStore::GetInstance()->GetVolume( volname );
      if ( vol ) {
        debug() << "Attaching " << name << " to " << volname << endmsg;
        vol->SetSensitiveDetector( sensdet );
      } else {
        error() << "Couldn't find " << volname << endmsg;
      }
    }
  }
}
