#include "Geant4/G4VUserDetectorConstruction.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

class IGiGaMTGeoSvc;
class IGaussinoTool;

// Factory class implemented as a GaudiTool that creates and configures the
// GiGaMTRunMangager singleton.

class GiGaMTDetectorConstructionFAC : public extends<GiGaTool, GiGaFactoryBase<G4VUserDetectorConstruction>>
{
public:
  using extends::extends;

  G4VUserDetectorConstruction* construct() const override;

protected:
  ServiceHandle<IGiGaMTGeoSvc> m_geoSvc{this, "GiGaMTGeoSvc", "GiGaMTGeo"};
  ToolHandleArray<IGaussinoTool> m_afterGeo{this, "AfterGeoConstructionTools", {}};
};

class GiGaMTProxyDetectorConstructionFAC : public GiGaMTDetectorConstructionFAC
{
public:
  using GiGaMTDetectorConstructionFAC::GiGaMTDetectorConstructionFAC;

  G4VUserDetectorConstruction* construct() const override;
};
