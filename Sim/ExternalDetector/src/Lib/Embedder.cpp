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
// local
#include "ExternalDetector/Embedder.h"
// G4
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"

class G4Box;
class G4Tubs;
namespace ExternalDetector {

  template <class Solid>
  StatusCode Embedder<Solid>::initialize() {
    return extends::initialize().andThen( [&] {
      StatusCode sc = StatusCode::SUCCESS;
      if ( m_sensDet ) { sc &= m_sensDet.retrieve(); }
      return sc;
    } );
  }

  template <class Solid>
  StatusCode Embedder<Solid>::embedSD() const {
    if ( m_sensDet ) {
      auto sensDet    = m_sensDet->construct();
      auto sd_manager = G4SDManager::GetSDMpointer();
      if ( !sd_manager ) return StatusCode::FAILURE;
      auto lvol = G4LogicalVolumeStore::GetInstance()->GetVolume( m_lVolName.value() );
      if ( !lvol ) return StatusCode::FAILURE;
      sd_manager->AddNewDetector( sensDet );
      lvol->SetSensitiveDetector( sensDet );
      debug() << "Registered sensitive " << sensDet->GetName() << " for " << m_pVolName.value() << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  template <class Solid>
  StatusCode Embedder<Solid>::embed( G4VPhysicalVolume* motherVolume ) const {
    if ( !motherVolume ) {
      error() << "Mother volume was not set." << endmsg;
      return StatusCode::FAILURE;
    }

    auto pvol = place( motherVolume->GetLogicalVolume() );

    if ( !pvol ) {
      error() << "Cannot create " << m_pVolName.value() << " physical volume" << endmsg;
      return StatusCode::FAILURE;
    }

    debug() << "Successfully embedded " << m_pVolName.value() << " in its mothers volume!" << endmsg;
    return StatusCode::SUCCESS;
  }

  template <class Solid>
  G4VPhysicalVolume* Embedder<Solid>::place( G4LogicalVolume* motherLVolume ) const {
    auto box = build();
    if ( !box ) {
      error() << "Unable to build a G4Box!" << endmsg;
      return nullptr;
    }
    G4Material* material = nullptr;
    if ( m_materialName.value().empty() ) {
      warning() << "Empty material name for " << m_lVolName.value() << endmsg;
      material = motherLVolume->GetMaterial();
      if ( material ) {
        warning() << "Setting its mother volume material: " << material->GetName() << endmsg;
      } else {
        warning() << "Mother volume's material was not set either. Leaving nullptr. " << endmsg;
      }
    } else {
      material = G4Material::GetMaterial( m_materialName.value(), true );
      if ( !material ) {
        error() << "Not able to find material: " << m_materialName.value() << endmsg;
        return nullptr;
      }
    }

    auto lvol   = new G4LogicalVolume( box, material, m_lVolName.value() );
    auto rot    = new CLHEP::HepRotation( CLHEP::HepRotationX( m_xAngle.value() ) );
    auto transl = CLHEP::Hep3Vector( m_xPos.value(), m_yPos.value(), m_zPos.value() );

    return new G4PVPlacement( rot, transl, lvol, m_pVolName.value(), motherLVolume, false, 0, false );
  }

} // namespace ExternalDetector

template class ExternalDetector::Embedder<G4Box>;
template class ExternalDetector::Embedder<G4Tubs>;
