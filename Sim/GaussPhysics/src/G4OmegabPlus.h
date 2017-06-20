

#ifndef G4OmegabPlus_h
#define G4OmegabPlus_h 1

#include "Geant4/globals.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4ParticleDefinition.hh"

// ######################################################################
// ###                         Omegab plus                        ###
// ######################################################################

class G4OmegabPlus : public G4ParticleDefinition
{
 private:
  static G4OmegabPlus * theInstance ;
  G4OmegabPlus( ) { }
  ~G4OmegabPlus( ) { }


 public:
  static G4OmegabPlus
 * Definition() ;
  static G4OmegabPlus * OmegabPlusDefinition() ;
  static G4OmegabPlus * OmegabPlus() ;
};


#endif
