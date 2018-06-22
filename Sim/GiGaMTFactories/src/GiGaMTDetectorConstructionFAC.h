#include "Geant4/G4VUserDetectorConstruction.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

class IGiGaMTGeoSvc;

// Factory class implemented as a GaudiTool that creates and configures the
// GiGaMTRunMangager singleton.

class GiGaMTDetectorConstructionFAC : public extends<GiGaTool, GiGaFactoryBase<G4VUserDetectorConstruction>>
{
public:
  using extends::extends;
  Gaudi::Property<std::string> m_geoSvcName{this, "GiGaMTGeoSvc", "GiGaMTGeo"};

  G4VUserDetectorConstruction* construct() const override;

  virtual StatusCode initialize() override;

  IGiGaMTGeoSvc* m_geoSvc = nullptr;
};
