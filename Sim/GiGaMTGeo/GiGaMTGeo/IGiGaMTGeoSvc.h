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
 * Definition of the abstract interface to a basic Geometry service
 *
 *  @author Dominik Muller
 */

class IGiGaMTGeoSvc : virtual public IService
{
public:
  /// Retrieve interface ID
  DeclareInterfaceID(IGiGaMTGeoSvc, 1, 0);

  virtual StatusCode initialize() override = 0;

  virtual StatusCode finalize() override = 0;

  virtual G4VPhysicalVolume* constructWorld() = 0;

public:

protected:
  /// virtual destructor
  virtual ~IGiGaMTGeoSvc(){};
};
