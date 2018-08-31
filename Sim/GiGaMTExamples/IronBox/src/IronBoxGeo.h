#pragma once

// from Gaudi
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"

// from GiGa
#include "GiGaMTGeo/IGiGaMTGeoSvc.h"

// Forwad declarations
//template <class TYPE>
//class SvcFactory;

// from G4
class G4VPhysicalVolume;

/**  @class IronBoxGeo IronBoxGeo.h
 *
 *   Simple geometry service that constructs an iron box.
 *
 *    @author: Dominik Muller
 */

class IronBoxGeo : public Service, virtual public IGiGaMTGeoSvc
{

  /// friend factory
  //friend class SvcFactory<IronBoxGeo>;

protected:
  using Service::Service;

  virtual ~IronBoxGeo(){};

public:
  StatusCode initialize() override {return Service::initialize();}
  StatusCode finalize() override {return Service::finalize();}


  virtual G4VPhysicalVolume* constructWorld() override;
  virtual void constructSDandField() override {};
  virtual StatusCode queryInterface( const InterfaceID& iid, void** pI ) override;
};
