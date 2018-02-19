// $

#ifndef G4XibcPlus_h
#define G4XibcPlus_h 1

#include "Geant4/globals.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4ParticleDefinition.hh"

// ######################################################################
// ###                         XibcPlus                        ###
// ######################################################################

class G4XibcPlus : public G4ParticleDefinition
{
 private:
  static G4XibcPlus * theInstance ;
  G4XibcPlus( ) { }
  ~G4XibcPlus( ) { }


 public:
  static G4XibcPlus * Definition() ;
  static G4XibcPlus * XibcPlusDefinition() ;
  static G4XibcPlus * XibcPlus() ;
};


#endif
