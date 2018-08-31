#include "G4GdmlTestDetector.h"

#include "FCCTest/CalorimeterSD.h"
#include "Geant4/G4SDManager.hh"
#include "GiGaMTCore/GdmlDetectorConstruction.h"

DECLARE_COMPONENT( G4GdmlTestDetector )

G4VUserDetectorConstruction* G4GdmlTestDetector::construct() const
{
  auto constr = new GDMLDetectorConstruction( m_gdmlFile );

  auto sdsetter = []( G4GDMLParser* parser ) {
    std::string caloSDname = "ECal";
    auto caloSD            = new FCCTest::CalorimeterSD( caloSDname, 201 );
    auto SDman             = G4SDManager::GetSDMpointer();
    SDman->AddNewDetector( caloSD );
    const G4GDMLAuxMapType* auxMap = parser->GetAuxMap();
    for ( auto& entry : *auxMap ) {
      for ( auto& info : entry.second ) {
        if ( info.type == "SensDet" ) {
          G4cout << "Attaching a sensitive detector: " << info.value << " to volume: " << entry.first->GetName()
                 << std::endl;
          G4VSensitiveDetector* mydet = SDman->FindSensitiveDetector( info.value );
          if ( mydet ) {
            entry.first->SetSensitiveDetector( mydet );
            entry.first->GetSolid()->DumpInfo();
          } else {
            G4cerr << "GDML contains sensitive detector of type: " << info.value << " but it is not found" << std::endl;
            ;
          }
        }
      }
    }
  };
  constr->SetSDConstructor( sdsetter );
  return constr;
}
