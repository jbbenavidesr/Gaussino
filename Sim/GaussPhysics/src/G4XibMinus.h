// $

#ifndef G4XibMinus_h
#define G4XibMinus_h 1

#include "Geant4/globals.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4ParticleDefinition.hh"

// ######################################################################
// ###                         Xib minus                        ###
// ######################################################################

class G4XibMinus : public G4ParticleDefinition
{
 private:
  static G4XibMinus * theInstance ;
  G4XibMinus( ) { }
  ~G4XibMinus( ) { }


 public:
  static G4XibMinus
 * Definition() ;
  static G4XibMinus * XibMinusDefinition() ;
  static G4XibMinus * XibMinus() ;
};


#endif
