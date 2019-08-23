#include "GiGaMTDetectorConstructionFAC.h"
#include "GiGaMTCoreDet/GiGaMTDetectorConstruction.h"
#include "GiGaMTCoreDet/GiGaMTProxyDetectorConstruction.h"
#include "GiGaMTGeo/IGiGaMTGeoSvc.h"
#include "SimInterfaces/IGaussinoTool.h"

DECLARE_COMPONENT( GiGaMTDetectorConstructionFAC )
DECLARE_COMPONENT( GiGaMTProxyDetectorConstructionFAC )

G4VUserDetectorConstruction* GiGaMTDetectorConstructionFAC::construct() const
{
  auto detconst = new GiGaMTDetectorConstruction();
  detconst->SetWorld( m_geoSvc->constructWorld() );

  return detconst;
}

G4VUserDetectorConstruction* GiGaMTProxyDetectorConstructionFAC::construct() const
{
  auto detconst = new GiGaMTProxyDetectorConstruction();
  detconst->SetWorldConstructor( [&]() {
    debug() << "Calling world constructor" << endmsg;
    auto world = m_geoSvc->constructWorld();
    for ( auto& tool : m_afterGeo ) {
      tool->process();
    }
    return world;
  } );
  detconst->SetSDConstructor( [&]() {
    debug() << "Calling SD and Field constructor" << endmsg;
    m_geoSvc->constructSDandField();
  } );

  return detconst;
}
