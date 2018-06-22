#pragma once

// from Gaudi
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VUserDetectorConstruction.hh"

//@class GiGaMTDetectorConstruction
//@brief Simple wrapper around G4VUserDetectorConstruction
//
// This wraps around G4VUserDetectorConstruction and provides a function
// to set the world volume from an external (i.e. Gaudi service etc) source
// which is then simply returned by the Construct() method.
//
// This is the base class that will be constructed by the factories and
// used throughout GiGaMT.
//
// TODO: This needs be fancied out with SD and field construction.
//
//@author Dominik Muller <dominik.muller@cern.ch>

class GiGaMTDetectorConstruction final : public G4VUserDetectorConstruction
{

public:
  using G4VUserDetectorConstruction::G4VUserDetectorConstruction;

  ~GiGaMTDetectorConstruction(){};

  virtual void SetWorld( G4VPhysicalVolume* _world ) final { m_world = _world; }
  virtual G4VPhysicalVolume* Construct() override final { return m_world; }

protected:
  G4VPhysicalVolume* m_world = nullptr;
};
