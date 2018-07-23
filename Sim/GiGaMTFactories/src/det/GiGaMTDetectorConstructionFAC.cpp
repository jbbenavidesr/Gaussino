#include "GiGaMTDetectorConstructionFAC.h"
#include "GiGaMTCore/GiGaMTDetectorConstruction.h"
#include "GiGaMTGeo/IGiGaMTGeoSvc.h"

DECLARE_COMPONENT( GiGaMTDetectorConstructionFAC )

StatusCode GiGaMTDetectorConstructionFAC::initialize()
{
  GiGaTool::initialize();
  m_geoSvc = svc<IGiGaMTGeoSvc>( m_geoSvcName, true );

  return StatusCode::SUCCESS;
}

G4VUserDetectorConstruction* GiGaMTDetectorConstructionFAC::construct() const
{
  auto detconst = new GiGaMTDetectorConstruction();
  detconst->SetWorld(m_geoSvc->constructWorld());

  return detconst;
}
