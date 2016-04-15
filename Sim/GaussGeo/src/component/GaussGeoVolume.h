#ifndef GAUSSGEO_GAUSSGEOVOLUME_H_
#define GAUSSGEO_GAUSSGEOVOLUME_H_

class G4LogicalVolume;
class GaussGeoAssembly;

class GaussGeoVolume {
 public:
  GaussGeoVolume(G4LogicalVolume* volume = 0, GaussGeoAssembly* assembly = 0)
    : m_volume(volume),
      m_assembly(assembly) {};

  ~GaussGeoVolume() {}; 

  inline G4LogicalVolume* volume() const { return m_volume; }
  inline GaussGeoAssembly* assembly() const { return m_assembly; }

  inline void setVolume (G4LogicalVolume* volume = 0) { m_volume = volume; }
  inline void setAssembly (GaussGeoAssembly* assembly = 0) { m_assembly = assembly; }

  inline bool isValid() const {
    if (volume() == 0 && assembly() == 0) {
      return false;
    } else if (volume() != 0 && assembly() != 0) {
      return false;
    }
    return true;
  }

private:
  G4LogicalVolume* m_volume;
  GaussGeoAssembly* m_assembly;
};

#endif // GAUSSGEO_GAUSSGEOVOLUME_H_
