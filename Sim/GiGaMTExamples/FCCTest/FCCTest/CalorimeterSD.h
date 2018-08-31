#pragma once
#include "FCCTest/CalorimeterHit.h"

#include "Geant4/G4VSensitiveDetector.hh"
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
namespace FCCTest {
  class CalorimeterHit;
}

/** @class CalorimeterHit.h TestGeometry/TestGeometry/CalorimeterHit.h CalorimeterHit.h
 *
 *  Implementation of the hit for the calorimeter.
 *  Based on B5HadCalorimeterSD from examples/basic/B5.
 *
 *  @author Anna Zaborowska
 *
 *  Modification for MT Gaussino
 *
 *  @author Dominik Muller
 */
namespace FCCTest
{
  class CalorimeterSD : public G4VSensitiveDetector
  {
  public:
    CalorimeterSD( G4String name );
    CalorimeterSD( G4String name, G4int aCellNoInAxis );
    virtual ~CalorimeterSD();
    virtual void Initialize( G4HCofThisEvent* HCE );
    virtual G4bool ProcessHits( G4Step* aStep, G4TouchableHistory* ROhist );

  private:
    CalorimeterHitsCollection* fHitsCollection;
    G4int fHCID;
    G4int fCellNo;
    std::map<unsigned int, CalorimeterHit*> hitmap;
  };
}
