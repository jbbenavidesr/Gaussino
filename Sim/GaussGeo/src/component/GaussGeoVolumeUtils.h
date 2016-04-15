#ifndef GAUSSGEO_GAUSSGEOVOLUMEUTILS_H_
#define GAUSSGEO_GAUSSGEOVOLUMEUTILS_H_

// Standard
#include <string>

class G4LogicalVolume;
class G4Region;
class G4VPhysicalVolume;
class GaussGeoAssembly;

namespace GaussGeoVolumeUtils {
  G4LogicalVolume* findLVolume(const std::string& name);
  G4VPhysicalVolume* findPVolume(const std::string& name);
  GaussGeoAssembly* findLAssembly(const std::string& name);
  G4Region* findRegion(const std::string& name);
}

#endif // GAUSSGEO_GAUSSGEOVOLUMEUTILS_H_
