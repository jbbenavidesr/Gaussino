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

// G4
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
// Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SystemOfUnits.h"
// GiGaMT
#include "GiGaMTGeo/IGiGaMTGeoSvc.h"

namespace ExternalDetector {
  class WorldCreator : public Service, virtual public IGiGaMTGeoSvc {

    // required
    Gaudi::Property<std::string> m_worldMaterial{this, "WorldMaterial", ""};

    // optional
    Gaudi::Property<std::string> m_worldName{this, "WorldName", "WorldBox"};
    Gaudi::Property<std::string> m_worldLogicalVolumeName{this, "WorldLogicalVolumeName", "WorldLVol"};
    Gaudi::Property<std::string> m_worldPhysicalVolumeName{this, "WorldPhysicalVolumeName", "WorldPVol"};
    Gaudi::Property<double>      m_worldSizeX{this, "WorldSizeX", 50. * Gaudi::Units::m};
    Gaudi::Property<double>      m_worldSizeY{this, "WorldSizeY", 50. * Gaudi::Units::m};
    Gaudi::Property<double>      m_worldSizeZ{this, "WorldSizeZ", 50. * Gaudi::Units::m};
    // visual attributes
    Gaudi::Property<double> m_visible{this, "Visible", false};

  public:
    using Service::Service;
    StatusCode                 initialize() override;
    virtual G4VPhysicalVolume* constructWorld() override;
    inline virtual void        constructSDandField() override{};
    virtual StatusCode         queryInterface( const InterfaceID& iid, void** pI ) override;
  };
} // namespace ExternalDetector

DECLARE_COMPONENT_WITH_ID( ExternalDetector::WorldCreator, "ExternalWorldCreator" )

StatusCode ExternalDetector::WorldCreator::initialize() {
  return Service::initialize().andThen( [&]() -> StatusCode {
    if ( m_worldMaterial.value().empty() ) {
      error() << "External world must have a material defined!" << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  } );
}

G4VPhysicalVolume* ExternalDetector::WorldCreator::constructWorld() {
  debug() << "Retrieving material " << m_worldMaterial.value();
  auto g4material = G4Material::GetMaterial( m_worldMaterial.value() );

  if ( !g4material ) {
    error() << "Material: " << m_worldMaterial.value() << " does not exist!" << endmsg;
    return nullptr;
  }

  debug() << "Creating an external world in G4" << endmsg;
  auto world_sbox = new G4Box( m_worldName.value(), m_worldSizeX.value(), m_worldSizeY.value(), m_worldSizeZ.value() );
  auto world_lvol = new G4LogicalVolume( world_sbox, g4material, m_worldLogicalVolumeName.value(), 0, 0, 0 );
  auto world_pvol = new G4PVPlacement( nullptr, CLHEP::Hep3Vector(), m_worldPhysicalVolumeName.value(), world_lvol, 0,
                                       false, 0, false );

  if ( !m_visible.value() ) { world_lvol->SetVisAttributes( G4VisAttributes::Invisible ); }

  debug() << "External world created!" << endmsg;
  return world_pvol;
}

StatusCode ExternalDetector::WorldCreator::queryInterface( const InterfaceID& id, void** ppI ) {
  if ( 0 == ppI ) {
    return StatusCode::FAILURE; //  RETURN !!!
  } else if ( IGiGaMTGeoSvc::interfaceID() == id ) {
    *ppI = static_cast<IGiGaMTGeoSvc*>( this );
  } else {
    return Service::queryInterface( id, ppI ); //  RETURN !!!
  }

  addRef();

  return StatusCode::SUCCESS;
}
