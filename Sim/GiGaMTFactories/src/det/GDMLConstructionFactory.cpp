#include "GDMLConstructionFactory.h"

#include "GiGaMTCore/GdmlDetectorConstruction.h"

DECLARE_COMPONENT( GDMLConstructionFactory )

G4VUserDetectorConstruction* GDMLConstructionFactory::construct() const
{
  auto constr = new GDMLDetectorConstruction( m_gdmlFile );
  return constr;
}
