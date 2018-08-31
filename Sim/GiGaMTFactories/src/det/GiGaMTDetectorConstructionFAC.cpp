#include "GiGaMTDetectorConstructionFAC.h"
#include "GiGaMTCore/GiGaMTDetectorConstruction.h"
#include "GiGaMTCore/GiGaMTProxyDetectorConstruction.h"
#include "GiGaMTGeo/IGiGaMTGeoSvc.h"

DECLARE_COMPONENT( GiGaMTDetectorConstructionFAC )
DECLARE_COMPONENT( GiGaMTProxyDetectorConstructionFAC )

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

G4VUserDetectorConstruction* GiGaMTProxyDetectorConstructionFAC::construct() const
{
  auto detconst = new GiGaMTProxyDetectorConstruction();
  detconst->SetWorldConstructor([&](){
      debug() << "Calling world constructor" << endmsg;
      return m_geoSvc->constructWorld();});
  detconst->SetSDConstructor([&](){
      debug() << "Calling SD and Field constructor" << endmsg;
      m_geoSvc->constructSDandField();});

  return detconst;
}
