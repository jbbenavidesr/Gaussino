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

// Geant4
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4SDManager.hh"

// Gaussino
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

namespace Gaussino::CustomSimulation {
  class RegionFactory : public extends<GiGaTool, GiGaFactoryBase<G4Region>> {

    Gaudi::Property<std::string>              m_region_name{ this, "Name", "" };
    Gaudi::Property<std::string>              m_det_name{ this, "SensitiveDetectorName", "" };
    Gaudi::Property<std::vector<std::string>> m_volumes{ this, "Volumes", {} };

  public:
    using extends::extends;

    StatusCode        initialize() override;
    virtual G4Region* construct() const override;
  };
} // namespace Gaussino::CustomSimulation

StatusCode Gaussino::CustomSimulation::RegionFactory::initialize() {
  return extends::initialize().andThen( [&]() -> StatusCode {
    if ( m_region_name.value().empty() ) {
      error() << "No name specified for the fast region" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_det_name.value().empty() && m_volumes.value().empty() ) {
      error() << "Either the sensitive detector name or the list of volumes "
              << "must be specified for the fast region" << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  } );
}

G4Region* Gaussino::CustomSimulation::RegionFactory::construct() const {
  auto region = G4RegionStore::GetInstance()->GetRegion( m_region_name.value() );
  if ( region ) {
    debug() << "Fast Region '" + m_region_name.value() + "' already exists " << endmsg;
    return region;
  }

  auto                          sdmanager = G4SDManager::GetSDMpointer();
  std::vector<G4LogicalVolume*> lvolumes;
  auto                          volume_store = G4LogicalVolumeStore::GetInstance();

  if ( !volume_store ) { error() << "Cannot access volume store." << endmsg; }
  if ( !m_det_name.value().empty() ) {
    auto sens_det = sdmanager->FindSensitiveDetector( m_det_name.value() );
    if ( !sens_det ) {
      error() << "Cannot find the sensitive detector: " << m_det_name.value() << endmsg;
      return nullptr;
    }
    for ( auto ivol = volume_store->begin(); volume_store->end() != ivol; ++ivol ) {
      auto vol_sens_det = ( *ivol )->GetSensitiveDetector();
      if ( vol_sens_det && sens_det == vol_sens_det ) {
        debug() << "Found " << ( *ivol )->GetName() << " in " << sens_det->GetName() << endmsg;
        lvolumes.push_back( *ivol );
      }
    }
  }

  if ( !m_volumes.value().empty() ) {
    for ( auto& volume_name : m_volumes.value() ) {
      auto lvol = volume_store->GetVolume( volume_name );
      if ( !lvol ) {
        error() << "G4LogicalVolume '" + volume_name + "' is invalid " << endmsg;
        return nullptr;
      }
      if ( auto lregion = lvol->GetRegion(); lregion ) {
        debug() << "G4LogicalVolume '" + volume_name + "' already belongs to another region '" + lregion->GetName()
                << "'."
                << "This will be overriden with " << m_region_name.value() << endmsg;
      }
      lvolumes.push_back( lvol );
    }
  }

  region = new G4Region( m_region_name.value() );
  for ( auto& lvol : lvolumes ) {
    lvol->SetRegion( region );
    // TODO: should it be root logical volume for all the volumes?
    region->AddRootLogicalVolume( lvol );
  }
  return region;
}

DECLARE_COMPONENT_WITH_ID( Gaussino::CustomSimulation::RegionFactory, "CustomSimulationRegionFactory" )
