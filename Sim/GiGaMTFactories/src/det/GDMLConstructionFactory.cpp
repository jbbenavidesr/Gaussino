#include "GDMLConstructionFactory.h"

#include "GiGaMTCoreDet/GdmlDetectorConstruction.h"

DECLARE_COMPONENT( GDMLConstructionFactory )

G4VUserDetectorConstruction* GDMLConstructionFactory::construct() const
{
  auto constr = new GDMLDetectorConstruction( m_gdmlFile );
  return constr;
}
