#include <set>
// from Gaudi

// SimSvc
#include "GiGaMTRegions/IRegionsDefinitionSvc.h"
// Geant4
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "GiGaMTCore/GaussinoG4UserLimits.h"
// local
#include "GiGaSetSimAttributes.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class : GiGaSetSimAttributes
 *
 *  @author Witek Pokorski Witold.Pokorski@cern.ch
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_COMPONENT( GiGaSetSimAttributes )

// ============================================================================
/** find g4 volume by name
 *  @param name name of the volume
 */
// ============================================================================
G4LogicalVolume* GiGaSetSimAttributes::g4volume( const std::string& address ) const
{
  const G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  if ( 0 == store ) {
    Error( "g4volume('" + address + "'): G4LogicalVolumeStore* is NULL!" );
    return 0;
  }
  auto vol = store->GetVolume( address );
  if ( vol ) {
    return vol;
  }
  Error( "g4volume('" + address + "'): volume is not found!" );
  return 0;
}

// ============================================================================
/** the only one method, perform the erasure of Field Manager
 *  @see IGiGaTool
 *  @param vol volume name
 *  @return status code
 */
// ============================================================================
StatusCode GiGaSetSimAttributes::process( const std::string& vol ) const
{

  if ( !m_simSvc.isValid() ) {
    return Error( " process('" + vol + "'): simSvc () is NULL! " );
  }

  // for all volumes
  if ( vol.empty() ) {
    return process( "ALL" );
  } // RETURN
  else if ( "All" == vol ) {
    return process( "ALL" );
  } // RETURN
  else if ( "all" == vol ) {
    return process( "ALL" );
  } // RETURN
  else if ( "ALL" == vol ) {
    const G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
    if ( 0 == store ) {
      return Error( " process('" + vol + "'): G4LogicalVolumeStore* is invalid" );
    }
    for ( auto vol : *store ) {
      if ( 0 != vol ) {
        process( vol->GetName() );
      }
    }
    return StatusCode::SUCCESS; // RETURN
  };

  G4LogicalVolume* g4lv = g4volume( vol );

  if ( 0 == g4lv ) {
    return Error( " process('" + vol + "'): G4LogicalVolume* is invalid" );
  }

  // set new limits ?
  const bool newAttributes = m_simSvc->hasSimAttribute( vol );
  if ( !newAttributes ) {
    return StatusCode::SUCCESS;
  } // RETURN

  typedef std::map<int, const SimAttribute*> SimAttributes;

  debug() << "Setting SimAttributes for " + vol << endmsg;
  const SimAttributes* partattr = m_simSvc->simAttribute( vol );

  // instanciate GaussG4UserLimits
  Gaussino::UserLimits ulimit( "GaussinoUserLimits" );

  for ( auto& [pid, attr] : *partattr ) {

    if ( 0 == attr ) {
      continue;
    }

    // set max allowed step
    if ( -1 != attr->maxAllowedStep() ) {
      ulimit.SetMaxAllowedStep( attr->maxAllowedStep(), pid );
    }

    // set max track length
    if ( -1 != attr->maxTrackLength() ) {
      ulimit.SetUserMaxTrackLength( attr->maxTrackLength(), pid );
    }

    // set max time
    if ( -1 != attr->maxTime() ) {
      ulimit.SetUserMaxTime( attr->maxTime(), pid );
    }

    // set minimum kinetic energy
    if ( -1 != attr->minEkine() ) {
      ulimit.SetUserMinEkine( attr->minEkine(), pid );
    }

    // set minimum range
    if ( -1 != attr->minRange() ) {
      ulimit.SetUserMinRange( attr->minRange(), pid );
    }
  }

  // attach user limits to the given G4 volume
  StatusCode sc = setUserLimits( g4lv, ulimit );
  if ( sc.isFailure() ) {
    return Error( " process('" + vol + "'): error from setUserLimits", sc );
  }

  return StatusCode::SUCCESS;
}

// ============================================================================
/** set user Limits for the given logical volume
 *  and propagate it to all daughetr volumes
 *  @param lv logical volume
 *  @param ul user limits
 *  @return status code
 */
// ============================================================================
StatusCode GiGaSetSimAttributes::setUserLimits( G4LogicalVolume* lv, const Gaussino::UserLimits& ul ) const
{
  if ( 0 == lv ) {
    return Error( " setUserLimits(): volume* is NULL!" );
  }

  const std::string& volume = lv->GetName();

  // check the existing limits
  if ( 0 != lv->GetUserLimits() ) {
    auto aux = dynamic_cast<Gaussino::UserLimits*>( lv->GetUserLimits() );
    // keep the identical limits
    if ( 0 == aux || ( ul != *aux ) ) {
      Warning( " setUserLimits ('" + volume + "') : G4LogicalVolume has user limits " );
      // keep the existing limits
      if ( !overwrite() ) {
        return StatusCode::SUCCESS;
      } // ATTENTNION
      Warning( " setUserLimits ('" + volume + "') : Existing limits are to be replaced " );
      // overwrite existing limits!
      G4UserLimits* tmp = lv->GetUserLimits();
      if ( 0 != tmp ) {
        delete tmp;
        tmp = 0;
      }                         // ATTENTION
      lv->SetUserLimits( tmp ); // ATTENTION
    }
  }

  // set new user limits
  if ( 0 == lv->GetUserLimits() ) {
    lv->SetUserLimits( new Gaussino::UserLimits( ul ) );
  }

  Print( " setUserLimts ('" + volume + ") : \t new user limits are set ", StatusCode::SUCCESS, MSG::DEBUG ).ignore();

  typedef std::set<G4LogicalVolume*> LVs;
  LVs daughters;

  // propagate the attributes to the daughter volumes
  const size_t nPV = lv->GetNoDaughters();
  for ( size_t iPV = 0; iPV < nPV; ++iPV ) {
    G4VPhysicalVolume* pv = lv->GetDaughter( iPV );
    if ( 0 == pv ) {
      return Error( " setUserLimits ('" + volume + "') : G4VPhysicalVolume* is invalid" );
    }
    G4LogicalVolume* dlv = pv->GetLogicalVolume();
    if ( 0 == dlv ) {
      return Error( " setUserLimits ('" + volume + "') : daughter G4LogicalVolume is invalid" );
    }
    daughters.insert( dlv );
  }

  for ( LVs::const_iterator ilv = daughters.begin(); daughters.end() != ilv; ++ilv ) {
    G4LogicalVolume* dlv = *ilv;
    StatusCode sc        = setUserLimits( dlv, ul );
    if ( sc.isFailure() ) {
      return Error( " setUserLimits ('" + volume + "') : cannot process daughter ", sc );
    }
  }

  return StatusCode::SUCCESS;
}
