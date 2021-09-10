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
#include "GiGaMTDetectorConstructionFAC.h"
#include "GiGaMTCoreDet/GiGaMTDetectorConstruction.h"
#include "GiGaMTGeo/IGiGaMTGeoSvc.h"
#include "SimInterfaces/IGaussinoTool.h"

#include "GiGaMTCoreDet/IExternalDetectorEmbedder.h"

DECLARE_COMPONENT( GiGaMTDetectorConstructionFAC )

StatusCode GiGaMTDetectorConstructionFAC::initialize() {
  auto sc = extends::initialize();
  // Retrieve the factory tools here to avoid the retrieval happening in multiple
  // threads
  for ( auto& keypairs : m_sens_dets ) { sc &= keypairs.second.retrieve(); }
  return sc;
}

G4VUserDetectorConstruction* GiGaMTDetectorConstructionFAC::construct() const {
  auto detconst = new GiGaMTDetectorConstruction();
  detconst->SetWorldConstructor( [&]() {
    debug() << "Calling world constructor" << endmsg;
    auto world = m_geoSvc->constructWorld();
    for ( auto& tool : m_afterGeo ) { tool->process().ignore(); }

    // Import external geometry
    for ( auto& embedder : m_ext_dets ) { embedder->embed( world ).ignore(); }

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
  if ( m_outfile.value() == "" ) { return; }
  if ( !world ) {
    error() << "Null pointer to world volume" << endmsg;
    return;
  }
  G4GDMLParser g4writer;
  try {
    g4writer.Write( m_outfile.value(), world, true, m_schema.value() );
  } catch ( std::logic_error& lerr ) { error() << "Caught an exception " << lerr.what() << endmsg; }
}
