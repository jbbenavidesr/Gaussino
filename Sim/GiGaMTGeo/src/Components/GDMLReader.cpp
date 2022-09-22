/*****************************************************************************\
* (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SystemOfUnits.h"
// Interface
#include "GiGaMTGeo/IGDMLReader.h"
// Geant4
#include "G4AssemblyVolume.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"

/** @class GDMLReader GDMLReader.h
 *
 *  Tool to import geometry from GDML file
 *
 */

class GDMLReader : public extends<GaudiTool, IGDMLReader> {

public:
  using extends::extends;

  StatusCode import( G4VPhysicalVolume* world ) const override;

private:
  Gaudi::Property<std::string> m_gdmlfile{this, "GDMLFileName", "", "Name of the GDML file to be loaded"};
  Gaudi::Property<double>      m_tx{this, "GDMLTranslationX", 0. * Gaudi::Units::mm};
  Gaudi::Property<double>      m_ty{this, "GDMLTranslationY", 0. * Gaudi::Units::mm};
  Gaudi::Property<double>      m_tz{this, "GDMLTranslationZ", 0. * Gaudi::Units::mm};
  Gaudi::Property<double>      m_rx{this, "GDMLRotationX", 0. * Gaudi::Units::deg};
  Gaudi::Property<double>      m_ry{this, "GDMLRotationY", 0. * Gaudi::Units::deg};
  Gaudi::Property<double>      m_rz{this, "GDMLRotationZ", 0. * Gaudi::Units::deg};
};

DECLARE_COMPONENT( GDMLReader )

StatusCode GDMLReader::import( G4VPhysicalVolume* world ) const {

  info() << "Loading GDML geometry description from file " << m_gdmlfile.value() << "." << endmsg;
  /// Read the GDML file.
  G4GDMLParser g4parser;
  g4parser.Read( m_gdmlfile.value(), true );
  /// Get the world volume.
  auto gdmlWorldPV = g4parser.GetWorldVolume();
  if ( !gdmlWorldPV ) {
    error() << "Could not retrieve world volume from file " << m_gdmlfile.value() << "." << endmsg;
    return StatusCode::FAILURE;
  }
  auto gdmlWorldLV = gdmlWorldPV->GetLogicalVolume();

  if ( gdmlWorldLV->GetName() == world->GetLogicalVolume()->GetName() ) {
    error() << "The name of the imported world from GDML is the same "
            << "as the name of the already exisiting world! "
            << "GDMLReader will not work correctly in this case." << endmsg;
    return StatusCode::FAILURE;
  }

  /// Make an assembly volume to hold the daughter volumes of the GDML world.
  auto gdmlAssembly = new G4AssemblyVolume();
  // Loop over the daughter volumes.
  const int nDaughters = gdmlWorldLV->GetNoDaughters();
  info() << "GDML world volume " << gdmlWorldLV->GetName() << " has " << nDaughters << " daughter(s)" << endmsg;
  for ( int i = nDaughters; i--; ) {
    auto gdmlDaughterPV = gdmlWorldLV->GetDaughter( i );
    auto nameDaughter   = gdmlDaughterPV->GetName();
    info() << "Picking up volume " << nameDaughter << " from " << m_gdmlfile.value() << endmsg;
    auto transDaughter  = gdmlDaughterPV->GetTranslation();
    auto rotDaughter    = gdmlDaughterPV->GetRotation();
    auto gdmlDaughterLV = gdmlDaughterPV->GetLogicalVolume();
    /// Give the GDML volume a different colour (to better distinguish it
    /// in the G4 visualization)
    auto gdmlDaughterVisAtt = new G4VisAttributes( G4Colour( 0, 1, 1 ) );
    gdmlDaughterLV->SetVisAttributes( gdmlDaughterVisAtt );

    /// Add the volume to the assembly
    gdmlAssembly->AddPlacedVolume( gdmlDaughterLV, transDaughter, rotDaughter );
  }
  /// Set the position of the GDML assembly in the LHCb world.
  G4ThreeVector transAssembly( m_tx.value(), m_ty.value(), m_tz.value() );
  auto          rotAssembly = new G4RotationMatrix();
  rotAssembly->rotateX( m_rx.value() * Gaudi::Units::deg );
  rotAssembly->rotateY( m_ry.value() * Gaudi::Units::deg );
  rotAssembly->rotateZ( m_rz.value() * Gaudi::Units::deg );
  /// Place the assembly volume in the LHCb world.
  gdmlAssembly->MakeImprint( world->GetLogicalVolume(), transAssembly, rotAssembly );
  return StatusCode::SUCCESS;
}
