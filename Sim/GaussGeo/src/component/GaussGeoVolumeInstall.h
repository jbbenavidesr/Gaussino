#ifndef GAUSSGEO_GAUSSGEOVOLUMEINSTALL_H_
#define GAUSSGEO_GAUSSGEOVOLUMEINSTALL_H_

// Standard
#include <string>

// Gaudi
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/StatusCode.h"

class G4LogicalVolume;
class GaussGeoAssembly;
class GaussGeoVolume;
class MsgStream;

namespace GaussGeoVolumeInstall {
  StatusCode installVolume(G4LogicalVolume* child_volume,
                           const std::string& name,
                           const Gaudi::Transform3D& matrix,
                           G4LogicalVolume* mother_volume,
                           MsgStream& msg);

  StatusCode installVolume(const GaussGeoAssembly* child_volume,
                           const std::string& name,
                           const Gaudi::Transform3D& matrix,
                           G4LogicalVolume* mother_volume,
                           MsgStream& msg);

  StatusCode installVolume(const GaussGeoVolume& child_volume,
                           const std::string& name,
                           const Gaudi::Transform3D& matrix,
                           G4LogicalVolume* mother_volume,
                           MsgStream& msg);
}

#endif // GAUSSGEO_GAUSSGEOVOLUMEINSTALL_H_
