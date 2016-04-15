// Local
#include "GaussGeoAssembly.h"
#include "GaussGeoAssemblyStore.h"

GaussGeoAssembly::GaussGeoAssembly(const std::string& Name) : m_name(Name), m_volumes() {
  GaussGeoAssemblyStore::store()->addAssembly(this);
}

GaussGeoAssembly::GaussGeoAssembly(const GaussGeoAssembly& right)
  : m_name(right.m_name),
    m_volumes(right.m_volumes)
{
  GaussGeoAssemblyStore::store()->addAssembly(this);
}

GaussGeoAssembly::~GaussGeoAssembly() {
  m_volumes.clear();
  GaussGeoAssemblyStore::store()->removeAssembly(this);
}

StatusCode GaussGeoAssembly::addVolume(const GaussGeoVolumePair& volume_pair, const std::string& name) {
  const GaussGeoVolume& volume = volume_pair.volume();
  if (!volume.isValid()) {
    return StatusCode::FAILURE;
  }

  if (volume.volume() != nullptr) {
    m_volumes.push_back(Volume(NamedG4LVolume(volume.volume(), name), volume_pair.matrix()));
    return StatusCode::SUCCESS;
  }

  const GaussGeoAssembly* assembly = volume.assembly();
  if (assembly == nullptr) {
    return StatusCode::FAILURE;
  }

  for (Volumes::const_iterator volume_iter = assembly->volumes().begin();
       assembly->volumes().end() != volume_iter;
       ++volume_iter) {
    m_volumes.push_back(Volume(NamedG4LVolume(volume_iter->first.first,
                                              name + "#" + volume_iter->first.second),
                                              volume_iter->second * volume_pair.matrix()));
  }

  return StatusCode::SUCCESS ;
}
