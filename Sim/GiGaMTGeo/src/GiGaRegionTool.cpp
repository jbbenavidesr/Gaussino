// G4
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4ProductionCuts.hh"
#include "Geant4/G4Region.hh"
#include "Geant4/G4RegionStore.hh"

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
StatusCode GiGaRegionTool::process( const std::string& /* region */ ) const
{
  // check the existence of the region
  G4Region* region = G4RegionStore::GetInstance()->GetRegion( m_region.value() );
  // FIXME: Should this really skip the rest or instead just updates the cuts and volumes?
  if ( 0 != region ) {
    Warning( " The Region '" + m_region + "'  already exist, skip " );
    return StatusCode::SUCCESS;
  }

  // create new region
  region = new G4Region( m_region.value() );
  // add volumes to the region
  for ( auto& ivolume : m_volumes ) {
    G4LogicalVolume* volume = G4LogicalVolumeStore::GetInstance()->GetVolume( ivolume );
    if ( 0 == volume ) {
      return Error( " G4LogicalVolume '" + ivolume + "' is invalid " );
    }
    if ( 0 != volume->GetRegion() && !m_overwrite ) {
      Warning( " G4LogicalVolume '" + ivolume + "' already belongs to region '" + volume->GetRegion()->GetName() +
               "' , skip " );
      continue;
    } else if ( 0 != volume->GetRegion() && m_overwrite ) {
      Warning( " G4LogicalVolume '" + ivolume + "' already belongs to region '" + volume->GetRegion()->GetName() +
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
