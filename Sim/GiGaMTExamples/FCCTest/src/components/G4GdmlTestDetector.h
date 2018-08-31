#pragma once

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"

/** @class G4GdmlTestDetector SimG4Components/src/G4GdmlTestDetector.h G4GdmlTestDetector.h
 *
 *  Detector construction tool using the GDML file.
 *  GDML file name needs to be specified in job options file (\b'gdml').
 *
 *  @author Anna Zaborowska
 *
 *  Modification for MT Gaussino
 *
 *  @author Dominik Muller
 */

class G4GdmlTestDetector : public extends<GaudiTool, GiGaFactoryBase<G4VUserDetectorConstruction>>
{
private:
  Gaudi::Property<std::string> m_gdmlFile{this, "GDML", ""};

public:
  using extends::extends;
  G4VUserDetectorConstruction* construct() const override;
};
