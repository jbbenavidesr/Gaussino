//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//
//---------------------------------------------------------------------------
//
// ClassName:   G4EmStandardPhysics_option2_HepEm
//
// Author:      S. Diederichs 08.08.2025
//
//
//----------------------------------------------------------------------------
//
// This class is a copy of the G4EmStandardPhysics_option2 but adds the AdePT
// tracking manager to accelerate the tracking of e-/e+/gammas on GPU
//

#ifndef G4EmStandardPhysics_option2_AdePT_hh
#define G4EmStandardPhysics_option2_AdePT_hh 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

class AdePTConfiguration;
class AdePTTrackingManager;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4EmStandardPhysics_option2_AdePT : public G4VPhysicsConstructor {
public:
  explicit G4EmStandardPhysics_option2_AdePT( G4int ver = 1, const G4String& name = "" );

  ~G4EmStandardPhysics_option2_AdePT() override;

  void ConstructParticle() override;
  void ConstructProcess() override;

private:
  G4int verbose;

  AdePTConfiguration*   fAdePTConfiguration;
  AdePTTrackingManager* fTrackingManager;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
