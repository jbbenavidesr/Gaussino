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
#include "GiGaMTCoreDet/IExternalDetectorEmbedder.h"
#include "GiGaMTGeo/IGiGaMTGeoSvc.h"
#include "SimInterfaces/IGaussinoTool.h"
#include <filesystem>
DECLARE_COMPONENT( GiGaMTDetectorConstructionFAC )

StatusCode GiGaMTDetectorConstructionFAC::initialize() {
  return extends::initialize().andThen( [&]() -> StatusCode {
    StatusCode sc = StatusCode::SUCCESS;

    // Retrieve the factory tools here to avoid the retrieval happening in multiple
    // threads
    for ( auto& keypairs : m_sens_dets ) { sc &= keypairs.second.retrieve(); }
    for ( auto& embedder : m_ext_dets ) { sc &= embedder.retrieve(); }
    for ( auto& par_world : m_par_worlds ) { sc &= par_world.retrieve(); }

    if ( !m_outfile.value().empty() && std::filesystem::exists( m_outfile.value() ) ) {
      warning() << "GDML file " << m_outfile.value() << " already exists! "
                << "G4 will abort execution if the file GDML already exists." << endmsg;
      if ( m_outfileOverwrite.value() ) {
        warning() << "Removing the GDML file: " << m_outfile.value() << endmsg;
        std::filesystem::remove( m_outfile.value() );
      } else {
        error() << "Overwriting the GDML is disabled." << endmsg;
        return StatusCode::FAILURE;
      }
    }

    return sc;
  } );
}

StatusCode GiGaMTDetectorConstructionFAC::finalize() {
  return extends::finalize().andThen( [&]() -> StatusCode {
    if ( !m_outfile.value().empty() ) { return SaveGDML(); }
    return StatusCode::SUCCESS;
  } );
}

G4VUserDetectorConstruction* GiGaMTDetectorConstructionFAC::construct() const {
  auto detconst = new GiGaMTDetectorConstruction();
  detconst->SetWorldConstructor( [&]() {
    // Import external materials
    debug() << "Setting up external materials" << endmsg;
    for ( auto& material : m_ext_mats ) { material->construct(); }

    debug() << "Calling world constructor" << endmsg;
    auto world = m_geoSvc->constructWorld();
    for ( auto& tool : m_afterGeo ) { tool->process().ignore(); }

    // Import external geometry
    debug() << "Setting up external embedder volumes in mass geometry" << endmsg;
    for ( auto& embedder : m_ext_dets ) { embedder->embed( world ).ignore(); }

    return world;
  } );
  detconst->SetSDConstructor( [&]() {
    debug() << "Calling SD and Field constructor" << endmsg;
    m_geoSvc->constructSDandField();

    // Import external SD
    debug() << "Setting up external embedder SD in mass geometry" << endmsg;
    for ( auto& embedder : m_ext_dets ) { embedder->embedSD().ignore(); }

    DressVolumes();
  } );

  // Setup parallel worlds
  for ( auto& par_world_fac : m_par_worlds ) {
    debug() << "Setting up parallel world " << par_world_fac->name() << endmsg;
    auto par_world = par_world_fac->construct();
    detconst->RegisterParallelWorld( par_world );
  }

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

StatusCode GiGaMTDetectorConstructionFAC::SaveGDML() const {
  try {
    G4GDMLParser g4writer;
    g4writer.SetSDExport( m_exportSD.value() );
    g4writer.SetEnergyCutsExport( m_exportEnergyCuts.value() );
    G4LogicalVolume* world = nullptr;
    g4writer.Write( m_outfile.value(), world, m_refs.value(), m_schema.value() );
  } catch ( std::exception& err ) {
    error() << "Caught an exception while writing a GDML file: " << err.what() << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
