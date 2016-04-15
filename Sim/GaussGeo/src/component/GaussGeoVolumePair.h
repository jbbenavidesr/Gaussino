#ifndef GAUSSGEO_GAUSSGEOVOLUMEPAIR_H_ 
#define GAUSSGEO_GAUSSGEOVOLUMEPAIR_H_

// Gaudi
#include "GaudiKernel/Transform3DTypes.h"

// Local
#include "GaussGeoVolume.h"

class GaussGeoVolumePair {
 public:
  GaussGeoVolumePair(const GaussGeoVolume& volume = GaussGeoVolume(),
                     const Gaudi::Transform3D& matrix = Gaudi::Transform3D())
    : m_volume(volume),
      m_matrix(matrix) {};

  ~GaussGeoVolumePair(){};

  inline const GaussGeoVolume& volume() const { return m_volume; }
  inline const Gaudi::Transform3D& matrix () const { return m_matrix; }

  inline void setVolume(const GaussGeoVolume& volume) { m_volume = volume; }
  inline void setMatrix(const Gaudi::Transform3D& matrix = Gaudi::Transform3D()) { m_matrix = matrix; }

private:
  GaussGeoVolume m_volume;
  Gaudi::Transform3D m_matrix;
};

#endif // GAUSSGEO_GAUSSGEOVOLUMEPAIR_H_
