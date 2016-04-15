#ifndef GAUSSGEO_GAUSSGEOASSEMBLYSTORE_H_
#define GAUSSGEO_GAUSSGEOASSEMBLYSTORE_H_

// Standard
#include <vector>

// Gaudi
#include "GaudiKernel/StatusCode.h"

class GaussGeoAssembly;

class GaussGeoAssemblyStore {
 public:
  friend class GaussGeoAssembly;
  typedef std::vector<GaussGeoAssembly*> Assemblies;

 protected:
  GaussGeoAssemblyStore();
  StatusCode addAssembly(GaussGeoAssembly* assembly);
  StatusCode removeAssembly(GaussGeoAssembly* assembly);

 public:
  ~GaussGeoAssemblyStore();

  static GaussGeoAssemblyStore* store();

  GaussGeoAssembly* assembly(const std::string& name);
  StatusCode clear();

 private:
  GaussGeoAssemblyStore(const GaussGeoAssemblyStore&);
  GaussGeoAssemblyStore& operator=(const GaussGeoAssemblyStore&);

  Assemblies m_assemblies;
};

#endif // GAUSSGEO_GAUSSGEOASSEMBLYSTORE_H_
