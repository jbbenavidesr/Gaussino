// $

#ifndef G4AntiXibcZero_h
#define G4AntiXibcZero_h 1

#include "Geant4/globals.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4ParticleDefinition.hh"

// ######################################################################
// ###                         AntiXibcZero                        ###
// ######################################################################

class G4AntiXibcZero : public G4ParticleDefinition
{
 private:
  static G4AntiXibcZero * theInstance ;
  G4AntiXibcZero( ) { }
  ~G4AntiXibcZero( ) { }


 public:
  static G4AntiXibcZero * Definition() ;
  static G4AntiXibcZero * AntiXibcZeroDefinition() ;
  static G4AntiXibcZero * AntiXibcZero() ;
};


#endif
