#pragma once

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"

/** @class GDMLConstructionFactory SimG4Components/src/GDMLConstructionFactory.h GDMLConstructionFactory.h
 *
 *  Simple tool to create a detector construction which instantiates the geometry from a GDML file.
 *  No support for sensitive detectors or fields
 *
 *  @author Dominik Muller
 */

class GDMLConstructionFactory : public extends<GaudiTool, GiGaFactoryBase<G4VUserDetectorConstruction>>
{
private:
  Gaudi::Property<std::string> m_gdmlFile{this, "GDML", ""};

public:
  using extends::extends;
  G4VUserDetectorConstruction* construct() const override;
};
