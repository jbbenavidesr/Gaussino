// $

#ifndef G4XiccPlusPlus_h
#define G4XiccPlusPlus_h 1

#include "Geant4/globals.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4ParticleDefinition.hh"

// ######################################################################
// ###                         XiccPlusPlus                        ###
// ######################################################################

class G4XiccPlusPlus : public G4ParticleDefinition
{
 private:
  static G4XiccPlusPlus * theInstance ;
  G4XiccPlusPlus( ) { }
  ~G4XiccPlusPlus( ) { }


 public:
  static G4XiccPlusPlus * Definition() ;
  static G4XiccPlusPlus * XiccPlusPlusDefinition() ;
  static G4XiccPlusPlus * XiccPlusPlus() ;
};


#endif
