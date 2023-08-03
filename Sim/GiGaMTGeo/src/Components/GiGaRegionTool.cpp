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
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ProductionCuts.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"

// local
#include "GiGaRegionTool.h"

// Declaration of the Tool Factory
DECLARE_COMPONENT( GiGaRegionTool )

// ============================================================================
/** the only one method
 *  @see GiGaToolBase
 *  @see IGiGaTool
 *  @param reg region name
 *  @return status code
 */
// ============================================================================
StatusCode GiGaRegionTool::process( const std::string& /* region */ ) const {
  // check the existence of the region
  G4Region* region = G4RegionStore::GetInstance()->GetRegion( m_region.value() );
  // FIXME: Should this really skip the rest or instead just updates the cuts and volumes?
  StatusCode sc = StatusCode::SUCCESS;
  if ( 0 != region ) { return Warning( " The Region '" + m_region + "'  already exist, skip " ); }

  // create new region
  region = new G4Region( m_region.value() );
  // add volumes to the region
  for ( auto& ivolume : m_volumes ) {
    G4LogicalVolume* volume = G4LogicalVolumeStore::GetInstance()->GetVolume( ivolume );
    if ( 0 == volume ) { return Error( " G4LogicalVolume '" + ivolume + "' is invalid " ); }
    if ( 0 != volume->GetRegion() && !m_overwrite ) {
      sc &= Warning( " G4LogicalVolume '" + ivolume + "' already belongs to region '" + volume->GetRegion()->GetName() +
                     "' , skip " );
      continue;
    } else if ( 0 != volume->GetRegion() && m_overwrite ) {
      sc &= Warning( " G4LogicalVolume '" + ivolume + "' already belongs to region '" + volume->GetRegion()->GetName() +
                     "', overwrite " );
    }
    // set region
    volume->SetRegion( region );
    region->AddRootLogicalVolume( volume );
  }

  // create production cuts
  G4ProductionCuts* cuts = new G4ProductionCuts();
  cuts->SetProductionCut( m_gamma, G4ProductionCuts::GetIndex( "gamma" ) );
  cuts->SetProductionCut( m_electron, G4ProductionCuts::GetIndex( "e-" ) );
  cuts->SetProductionCut( m_positron, G4ProductionCuts::GetIndex( "e+" ) );

  // set production cuts for region

  region->SetProductionCuts( cuts );

  return StatusCode::SUCCESS;
}
