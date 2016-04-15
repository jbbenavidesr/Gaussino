#ifndef  GAUSSGEO_IGAUSSGEO_H_
#define  GAUSSGEO_IGAUSSGEO_H_

// GaudiKernel
#include "GaudiKernel/IInterface.h"

class G4VPhysicalVolume;

// ============================================================================
// Minimal geometry source interface to GaussGeo
//
// 2015-11-11 : Dmitry Popov
// ============================================================================

class IGaussGeo : virtual public IInterface {
 public:
  // Retrieve unique interface ID
  static const InterfaceID& interfaceID();

  // Retrieve the pointer to the G4 geometry tree root
  virtual G4VPhysicalVolume* world() = 0;
};

#endif  // GAUSSGEO_IGAUSSGEO_H_
