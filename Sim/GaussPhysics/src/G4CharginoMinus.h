// $Id: G4CharginoMinus.h,v 1.1 2008-11-27 16:02:08 robbep Exp $

#ifndef G4CharginoMinus_h
#define G4CharginoMinus_h 1 

#include "Geant4/globals.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4ParticleDefinition.hh"

/** @class  G4CharginoMinus G4CharginoMinus.h 
 *  
 *  Define the Chargino minus in Geant
 * 
 *  @author Neal Gueissaz
 *  @date   2008-09-12
 */

// ######################################################################
// ###                       CharginoMinus                            ###
// ######################################################################

class G4CharginoMinus : public G4ParticleDefinition
{
 private:
  static G4CharginoMinus * theInstance ;
  G4CharginoMinus( ) { }
  ~G4CharginoMinus( ) { }

 public:
   static G4CharginoMinus* Definition();
   static G4CharginoMinus* CharginoMinusDefinition();
   static G4CharginoMinus* CharginoMinus();
};

#endif

