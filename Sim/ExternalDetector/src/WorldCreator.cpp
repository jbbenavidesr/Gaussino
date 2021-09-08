/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// local
#include "WorldCreator.h"
// LHCb
#include "DetDesc/Material.h"
// G4
#include "Geant4/G4Box.hh"
#include "Geant4/G4GDMLParser.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4PVPlacement.hh"


StatusCode ExternalDetector::WorldCreator::initialize() {
  return GaudiAlgorithm::initialize().andThen( [&]() -> StatusCode {
    debug() << "Retrieving material " << m_worldMaterial.value() << " from the conditions" << endmsg;
    auto material = getDet<Material>( m_worldMaterial.value() );
    if ( !material ) {
      error() << "Material not found!" << endmsg;
      return StatusCode::FAILURE;
    }
    auto g4material =
        new G4Material( material->registry()->identifier(), material->Z(), material->A(), material->density(),
                        (G4State)material->state(), material->temperature(), material->pressure() );

    if ( !g4material ) {
      error() << "Could not create g4material!" << endmsg;
      return StatusCode::FAILURE;
    }

    debug() << "Creating external world in G4" << endmsg;
    auto world_sbox = new G4Box( "WorldBox", m_worldSizeX.value(), m_worldSizeY.value(), m_worldSizeZ.value() );
    auto world_lvol = new G4LogicalVolume( world_sbox, g4material, "World", 0, 0, 0 );
    auto world_pvol = new G4PVPlacement( nullptr, CLHEP::Hep3Vector(), "WorldPV", world_lvol, 0, false, 0, false );
    if ( !world_pvol ) {
      error() << "No physical world volume" << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_ext_dets_names.empty() ) {
      warning() << "No external detectors to embed found!" << endmsg;
    } else {
      for ( auto& ext_det : m_ext_dets ) {
        debug() << "Embedding now an external detector: " << ext_det->name() << endmsg;
        ext_det->embed( world_pvol ).ignore();
      }
    }

    debug() << "External world created!" << endmsg;

    if ( m_writeGDML.value() ) {
      debug() << "Writing geometry to: " << m_outfile.value() << endmsg;
      G4GDMLParser g4writer;
      g4writer.SetSDExport( true );
      g4writer.Write( m_outfile.value(), world_pvol, true, m_schema.value() );
    }
    return StatusCode::SUCCESS;
  } );
}

DECLARE_COMPONENT_WITH_ID( ExternalDetector::WorldCreator, "ExternalDetectorWorldCreator" )
