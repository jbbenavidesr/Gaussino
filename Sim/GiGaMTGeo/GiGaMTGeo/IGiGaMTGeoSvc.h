#pragma once

#include "GaudiKernel/IService.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
// GiGa

// Forward declaration from G4
class G4PrimaryVertex;
class G4VPhysicalVolume;

/** @class IGiGaMTGeoSvc IGiGaMTGeoSvc.h GiGaMT/IGiGaMTGeoSvc.h
 *
 * Definition of the abstract interface to a basic Geometry service.
 * This service is intended to be used by 
 *
 *  @author Dominik Muller
 */

class IGiGaMTGeoSvc : virtual public IService
{
public:
  /// Retrieve interface ID
  DeclareInterfaceID(IGiGaMTGeoSvc, 1, 0);

  virtual G4VPhysicalVolume* constructWorld() = 0;
  virtual void constructSDandField() = 0;

public:

protected:
  /// virtual destructor
  virtual ~IGiGaMTGeoSvc(){};
};
