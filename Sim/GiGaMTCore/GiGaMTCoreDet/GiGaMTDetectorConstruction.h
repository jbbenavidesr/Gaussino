#pragma once

// from Gaudi
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include <functional>

//@class GiGaMTDetectorConstruction
//@brief Simple wrapper around G4VUserDetectorConstruction
//
// This wraps around G4VUserDetectorConstruction and acts as a proxy that
// calls externally provided functions to construct the world as well as the
// senstive detectors and fields
//
//@author Dominik Muller <dominik.muller@cern.ch>

class GiGaMTDetectorConstruction final : public G4VUserDetectorConstruction
{
  typedef std::function<G4VPhysicalVolume*()> worldConstructor;
  typedef std::function<void()> sdConstructor;

public:
  using G4VUserDetectorConstruction::G4VUserDetectorConstruction;

  ~GiGaMTDetectorConstruction(){};

  virtual G4VPhysicalVolume* Construct() override final { return m_world_constructor(); }
  virtual void ConstructSDandField() override final { m_sd_constructor(); }

  void SetWorldConstructor(worldConstructor constr){m_world_constructor=constr;}
  void SetSDConstructor(sdConstructor constr){m_sd_constructor=constr;}

private:
  worldConstructor m_world_constructor;
  sdConstructor m_sd_constructor;

};
