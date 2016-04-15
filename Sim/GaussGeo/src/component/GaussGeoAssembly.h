#ifndef GAUSSGEO_GAUSSGEOASSEMBLY_H_
#define GAUSSGEO_GAUSSGEOASSEMBLY_H_

// Standard
#include <vector>
#include <string>

// Gaudi
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/StatusCode.h" 

// Local
#include "GaussGeoVolumePair.h"

class GaussGeoAssembly {
 public:
  typedef std::pair<G4LogicalVolume*, std::string> NamedG4LVolume;
  typedef std::pair<NamedG4LVolume, Gaudi::Transform3D> Volume;
  typedef std::vector<Volume> Volumes;

  GaussGeoAssembly (const std::string& name = "");
  GaussGeoAssembly (const GaussGeoAssembly& right);

  virtual ~GaussGeoAssembly ();

  inline const Volumes& volumes() const { return m_volumes; }

  const std::string& name() const { return m_name; }

  inline void setName(const std::string& name = "") { m_name = name; }

  StatusCode addVolume(const GaussGeoVolumePair& volume_pair, const std::string& name = "");

 private:
  std::string m_name;
  Volumes m_volumes;
};

#endif // GAUSSGEO_GAUSSGEOASSEMBLY_H_
