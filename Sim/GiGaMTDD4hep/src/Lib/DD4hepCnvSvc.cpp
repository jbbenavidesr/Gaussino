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
// Gaudi includes
#include "GiGaMTDD4hep/DD4hepCnvSvc.h"

// Geant4
#include "G4LogicalVolumeStore.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"
#include "TSystem.h"

// ============================================================================
// Conversion service for convertion DD4hep geometry from LHCb to Geant4
//
// 2020-6-15 : Dominik Muller
// ============================================================================

StatusCode DD4hepCnvSvc::initialize() {
  auto  sc    = extends::initialize();
  auto& g4map = dd4hep::sim::Geant4Mapping::instance();
  // FIXME: Setting the global print level for DD4hep
  // does not work, i.e. it does get set to the correct
  // value but the printout level is not changed.
  // To be investigated.
  dd4hep::setPrintLevel( DD4hepGaudiMessaging::Convert( msgLevel() ) );
  dd4hep::DetElement           world = getDetector().world();
  dd4hep::sim::Geant4Converter conv( dd4hep::Detector::getInstance(), dd4hep::VERBOSE );
  conv.debugMaterials  = m_debugMaterials.value();
  conv.debugElements   = m_debugElements.value();
  conv.debugShapes     = m_debugShapes.value();
  conv.debugVolumes    = m_debugVolumes.value();
  conv.debugPlacements = m_debugPlacements.value();
  conv.debugRegions    = m_debugRegions.value();

  dd4hep::sim::Geant4GeometryInfo* geoinfo = conv.create( world ).detach();
  geoinfo->printLevel                      = dd4hep::VERBOSE;

  g4map.attach( geoinfo );
  m_world_root = geoinfo->world();
  // Create Geant4 volume manager only if not yet available
  g4map.volumeManager();
  // Printout all active volumes and assigned sensitive detectors from dd4hep
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Stored paths in volume manager: " << endmsg;
    for ( auto& pp : g4map.volumeManager()->g4Paths ) {
      debug() << "/";
      for ( auto& pv : pp.first ) { debug() << pv->GetName() << "/"; }
      debug() << " ID:" << pp.second.volumeID << endmsg;
    }
    for ( auto& [dd4hep_volume, g4_volume] : g4map.data().g4Volumes ) {
      if ( !dd4hep_volume.isValid() || !dd4hep_volume.isSensitive() ) { continue; }
      auto dd4hep_sensdet_name = dd4hep_volume.sensitiveDetector().name();
      debug() << "Active volume '" << dd4hep_volume.name() << "' SensDet: '" << dd4hep_sensdet_name << "'" << endmsg;
    }
  }

  auto world_lvol = m_world_root->GetLogicalVolume();
  world_lvol->SetVisAttributes( G4VisAttributes::Invisible );

  if ( !m_invisibleVolumes.value().empty() ) {
    auto vol_store = G4LogicalVolumeStore::GetInstance();
    if ( !vol_store ) {
      error() << "G4LogicalVolumeStore points to NULL" << endmsg;
      return StatusCode::FAILURE;
    }

    for ( auto& vol_name : m_invisibleVolumes.value() ) {
      auto vol = vol_store->GetVolume( vol_name );
      if ( !vol ) {
        warning() << "Cannot find " << vol_name << "in the volume store!" << endmsg;
        continue;
      }
      vol->SetVisAttributes( G4VisAttributes::Invisible );
    }
  }

  // We now loop over the existing logical volume mappings and extract the
  // sensitive detector knowledge from the DD4hep volumes and map them to the
  // requested Gaussino factories
  if ( m_sensdetmappings.size() > 0 ) {
    for ( auto& [dd4hep_volume, g4_volume] : g4map.data().g4Volumes ) {
      if ( !dd4hep_volume.isValid() ) { continue; }
      if ( dd4hep_volume.isSensitive() ) {
        auto dd4hep_sensdet_name = dd4hep_volume.sensitiveDetector().name();
        // Now only continue if the sensitive volume information is present in
        // the factory mappings provided
        if ( m_sensdetmappings.find( dd4hep_sensdet_name ) != std::end( m_sensdetmappings ) ) {
          auto tool_name = m_sensdetmappings[dd4hep_sensdet_name];
          sc &= register_sensitive( tool_name, g4_volume );
        }
      }
    }
  }

  // Register the factory for the global magnetic field
  if ( !m_mag_field_mgr.empty() ) {
    sc &= register_mag_field( m_mag_field_mgr, nullptr );
  } else {
    warning() << "Global magnetic Field was not requested to be loaded" << endmsg;
  }
  return sc;
}

StatusCode DD4hepCnvSvc::finalize() { return extends::finalize(); }

//=============================================================================
// Retrieve the pointer to G4 geometry tree root
//=============================================================================
G4VPhysicalVolume* DD4hepCnvSvc::constructWorld() {
  // Simply return the world root volume. It was populated during the
  // initialisation
  return m_world_root;
}

const dd4hep::Detector& DD4hepCnvSvc::getDetector() const {
  static std::once_flag flag;
  std::call_once( flag, [&]() {
    auto&   detector = dd4hep::Detector::getInstance();
    TString descriptionFile( m_detDescLocation.value() );
    gSystem->ExpandPathName( descriptionFile );
    const char* fname = descriptionFile.Data();
    if ( this->msgLevel( MSG::DEBUG ) ) debug() << "Loading DD4hep Geometry: " << fname << endmsg;
    detector.apply( "DD4hep_CompactLoader", 1, (char**)&fname );
  } );
  return dd4hep::Detector::getInstance();
}

void DD4hepCnvSvc::constructSDandField() {
  auto sdmanager = G4SDManager::GetSDMpointer();
  for ( auto& [sensdetfac, vols] : mmap_sensdet_to_lvols ) {
    debug() << "Assigning " << sensdetfac->name() << " to " << vols.size() << " logical volumes" << endmsg;
    auto sensdet = sensdetfac->construct();
    sdmanager->AddNewDetector( sensdet );
    for ( auto* vol : vols ) { vol->SetSensitiveDetector( sensdet ); }
  }

  for ( auto& [fieldmgrfac, vols] : mmap_fieldmgr_to_lvols ) {
    // Catch the global field manager
    if ( vols.size() == 1 && *std::begin( vols ) == nullptr ) {
      debug() << "Constructing global " << fieldmgrfac->name() << "." << endmsg;
      fieldmgrfac->construct( true );
    } else {
      auto fieldmgr = fieldmgrfac->construct( false );
      debug() << "Assigning " << fieldmgrfac->name() << " to " << vols.size() << " logical volumes" << endmsg;
      for ( auto* vol : vols ) { vol->SetFieldManager( fieldmgr, false ); }
    }
  }
}

//=============================================================================
// Instantiate a Magnetic Field Factories
//=============================================================================
StatusCode DD4hepCnvSvc::register_mag_field( const std::string& name, G4LogicalVolume* vol ) {
  // First check if we have already obtained the magnetic field factory for the
  // provided name.
  if ( !vol && m_found_global_fieldmgr ) {
    error() << "Tried to register second global magnetic field manager" << endmsg;
    return StatusCode::FAILURE;
  }
  auto it = mmap_name_to_fieldmgrfac.find( name );
  if ( it == std::end( mmap_name_to_fieldmgrfac ) ) {
    mmap_name_to_fieldmgrfac.emplace( std::piecewise_construct, std::forward_as_tuple( name ),
                                      std::forward_as_tuple( name, this ) );

    if ( !mmap_name_to_fieldmgrfac[name].retrieve().isSuccess() ) {
      error() << "Field manager not found: '" << name << "'" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Now fill the map of field managers to volumes
  if ( auto& fieldmgr = mmap_name_to_fieldmgrfac[name]; fieldmgr ) {
    if ( mmap_fieldmgr_to_lvols.find( fieldmgr.get() ) == std::end( mmap_fieldmgr_to_lvols ) ) {
      mmap_fieldmgr_to_lvols[fieldmgr.get()] = {};
    }
    mmap_fieldmgr_to_lvols[fieldmgr.get()].insert( vol );
    if ( vol ) { debug() << "Asked to use field " << name << " for " << vol->GetName() << "" << endmsg; }
  }
  if ( !vol ) {
    auto giga_fmanager = mmap_name_to_fieldmgrfac[name];
    info() << "Global 'Field Manager' is set to '" << objectTypeName( giga_fmanager ) << "/" << giga_fmanager->name()
           << "'" << endmsg;
    m_found_global_fieldmgr = true;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Instantiate a Sensitive Detector object
//=============================================================================
StatusCode DD4hepCnvSvc::register_sensitive( const std::string& name, G4LogicalVolume* vol ) {
  // First check if we have already obtained the sensitive detector factory for
  // the provided name.
  auto it = mmap_name_to_sensdetfac.find( name );
  if ( it == std::end( mmap_name_to_sensdetfac ) ) {
    mmap_name_to_sensdetfac.emplace( std::piecewise_construct, std::forward_as_tuple( name ),
                                     std::forward_as_tuple( name, this ) );

    if ( !mmap_name_to_sensdetfac[name].retrieve().isSuccess() ) {
      // mmap_name_to_sensdetfac[name] = nullptr;
      // getTool(name, mmap_name_to_sensdetfac[name], this);
      // Check if successful
      // if(!mmap_name_to_sensdetfac[name]){
      warning() << "Could not retrieve sensitive detector " << name << "." << endmsg;
      warning() << "What do we say to the geo service? Not today ..." << endmsg;
    }
  }
  // Now fill the map of identified sensitive detectors to volumes
  if ( auto sensdet = mmap_name_to_sensdetfac[name]; sensdet ) {
    if ( mmap_sensdet_to_lvols.find( sensdet.get() ) == std::end( mmap_sensdet_to_lvols ) ) {
      mmap_sensdet_to_lvols[sensdet.get()] = {};
    }
    mmap_sensdet_to_lvols[sensdet.get()].insert( vol );
    if ( vol ) { debug() << "Asked to use sens det " << name << " for " << vol->GetName() << "" << endmsg; }
  }
  return StatusCode::SUCCESS;
}
