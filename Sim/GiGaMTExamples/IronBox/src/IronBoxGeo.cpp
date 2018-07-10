#include "Geant4/G4Box.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4SystemOfUnits.hh"

// local
#include "IronBoxGeo.h"

DECLARE_COMPONENT( IronBoxGeo )

//=============================================================================
// query interface
//=============================================================================
StatusCode IronBoxGeo::queryInterface( const InterfaceID& id, void** ppI )
{
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

G4VPhysicalVolume* IronBoxGeo::constructWorld()
{

  G4NistManager* nist = G4NistManager::Instance();

  G4double cube_size    = 10 * m;
  G4Material* cube_mat  = nist->FindOrBuildMaterial( "G4_Fe" );
  G4Material* world_mat = nist->FindOrBuildMaterial( "G4_AIR" );

  G4bool checkOverlaps = true;

  auto solidWorld = new G4Box( "World", cube_size, cube_size, cube_size );

  auto logicWorld = new G4LogicalVolume( solidWorld, world_mat, "World" );

  auto physWorld = new G4PVPlacement( 0,               // no rotation
                                      G4ThreeVector(), // at (0,0,0)
                                      logicWorld,      // its logical volume
                                      "World",         // its name
                                      nullptr,         // its mother  volume
                                      false,           // no boolean operation
                                      0,               // copy number
                                      checkOverlaps ); // overlaps checking

  auto solidCube = new G4Box( "Cube", 0.5 * cube_size, 0.5 * cube_size, 0.5 * cube_size );

  auto logicEnv = new G4LogicalVolume( solidCube, cube_mat, "Cube" );

  new G4PVPlacement( 0,               // no rotation
                     G4ThreeVector(), // at (0,0,0)
                     logicEnv,        // its logical volume
                     "Cube",          // its name
                     logicWorld,      // its mother  volume
                     false,           // no boolean operation
                     0,               // copy number
                     checkOverlaps ); // overlaps checking

  return physWorld;
}
