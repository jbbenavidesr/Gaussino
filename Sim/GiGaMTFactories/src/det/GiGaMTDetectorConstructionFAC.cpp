#include "GiGaMTDetectorConstructionFAC.h"
#include "GiGaMTCoreDet/GiGaMTDetectorConstruction.h"
#include "GiGaMTGeo/IGiGaMTGeoSvc.h"
#include "SimInterfaces/IGaussinoTool.h"

DECLARE_COMPONENT( GiGaMTDetectorConstructionFAC )

StatusCode GiGaMTDetectorConstructionFAC::initialize() {
  auto sc = extends::initialize();
  // Retrieve the factory tools here to avoid the retrieval happening in multiple
  // threads
  for ( auto& keypairs : m_sens_dets ) { keypairs.second.retrieve(); }
  return sc;
}

G4VUserDetectorConstruction* GiGaMTDetectorConstructionFAC::construct() const {
  auto detconst = new GiGaMTDetectorConstruction();
  detconst->SetWorldConstructor( [&]() {
    debug() << "Calling world constructor" << endmsg;
    auto world = m_geoSvc->constructWorld();
    for ( auto& tool : m_afterGeo ) { tool->process(); }
    SaveGDML( world->GetLogicalVolume() );
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

#include "Geant4/G4GDMLParser.hh"

void GiGaMTDetectorConstructionFAC::SaveGDML( G4LogicalVolume* world ) const {
  if(m_outfile.value() == ""){
    return;
  }
  if ( !world ) {
    error() << "Null pointer to world volume" << endmsg;
    return;
  }
  G4GDMLParser g4writer;
  try {
    g4writer.Write( m_outfile.value(), world, true, m_schema.value() );
  } catch ( std::logic_error& lerr ) { error() << "Caught an exception " << lerr.what() << endmsg; }
}
