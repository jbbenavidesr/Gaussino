// $Id: $
#ifndef GAUSSCHERENKOV_CHERENKOVG4PMTREFLTAG_H 
#define GAUSSCHERENKOV_CHERENKOVG4PMTREFLTAG_H 1

// Include files
#include "Geant4/globals.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4ThreeVector.hh"


/** @class CherenkovG4PmtReflTag CherenkovG4PmtReflTag.h CherenkovMisc/CherenkovG4PmtReflTag.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2011-04-19
 */
extern void RichG4PmtQWIncidentTag(const G4Track& aPhotonTk, const G4ThreeVector & aPmtQWPoint, int aQWLensFlag );


#endif // GAUSSCHERENKOV_CHERENKOVG4PMTREFLTAG_H
