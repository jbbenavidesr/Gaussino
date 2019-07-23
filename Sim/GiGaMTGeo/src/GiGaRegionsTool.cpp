// from Gaudi
#include "GaudiKernel/MsgStream.h"

// SimSvc
#include "GiGaMTRegions/IRegionsDefinitionSvc.h"
#include "GiGaMTRegions/RegionCuts.h"

#include "Geant4/G4LogicalVolumeStore.hh"

// local
#include "GiGaRegionsTool.h"

// G4
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4ProductionCuts.hh"
#include "Geant4/G4Region.hh"

// Declaration of the Tool Factory
DECLARE_COMPONENT( GiGaRegionsTool )

// ============================================================================
/** the only one method
 *  @see GiGaToolBase
 *  @see IGiGaTool
 *  @param region region name
 *  @return status code
 */
// ============================================================================
StatusCode GiGaRegionsTool::process( const std::string& region ) const
{
  if ( m_simSvc.isValid() ) {
    return Error( " process('" + region + "'): IRegionsDefinitionSvc* is invalid!" );
  }

  typedef IRegionsDefinitionSvc::VectOfRegCuts Regions;
  typedef std::vector<std::string> Volumes;

  const Regions* regions = m_simSvc->regionsDefs();
  if ( 0 == regions ) {
    return Error( " process('" + region + "'): Regions* poitns to NULL " );
  }

  // loop over all regions
  debug() << " Number of G4 regions =  " << (int)regions->size() << endmsg;
  for ( auto& ireg : *regions ) {
    // all regions? or only selected region?
    if ( !region.empty() && ireg.region() != region ) {
      continue;
    }
    debug() << " Process the region '" << ireg.region() << "'" << endmsg;
    //
    G4Region* reg = new G4Region( ireg.region() );
    // add volumes to the region
    const Volumes& volumes = ireg.volumes();
    debug() << " Number of volumes in this region =  " << (int)volumes.size() << endmsg;
    for ( auto ivolume : volumes ) {

      G4LogicalVolume* volume = G4LogicalVolumeStore::GetInstance()->GetVolume( ivolume );
      if ( 0 == volume ) {
        Error( " process('" + ireg.region() + "'): G4LogicalVolume* '" + ivolume + "' points to NULL, skip it  " );
        continue;
      }

      if ( 0 != volume->GetRegion() && !m_overwrite ) {
        Warning( " G4LogicalVolume '" + ivolume + "' already belongs to region '" + volume->GetRegion()->GetName() +
                 "' , skip " );
        continue;
      } else if ( 0 != volume->GetRegion() && m_overwrite ) {
        if ( ( volume->GetRegion()->GetName() ) == "DefaultRegionForTheWorld" ) {
          debug() << "G4Region Change for  "
                  << " G4LogicalVolume '" << ivolume << " ' to " << reg->GetName() << endmsg;
        } else {
          Warning( " G4LogicalVolume '" + ivolume + "' already belongs to region '" + volume->GetRegion()->GetName() +
                   "', overwrite " );
        }
      }
      // set region
      volume->SetRegion( reg );
      reg->AddRootLogicalVolume( volume );
    }

    // create production cuts
    G4ProductionCuts* cuts = new G4ProductionCuts();

    if ( 0 <= ireg.gammaCut() ) {
      cuts->SetProductionCut( ireg.gammaCut(), G4ProductionCuts::GetIndex( "gamma" ) );
    }
    if ( 0 <= ireg.electronCut() ) {
      cuts->SetProductionCut( ireg.electronCut(), G4ProductionCuts::GetIndex( "e-" ) );
    }
    if ( 0 <= ireg.positronCut() ) {
      cuts->SetProductionCut( ireg.positronCut(), G4ProductionCuts::GetIndex( "e+" ) );
    }

    // set production cuts for region
    reg->SetProductionCuts( cuts );
  }

  return StatusCode::SUCCESS;
}
