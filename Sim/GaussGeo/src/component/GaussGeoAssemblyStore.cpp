// Standard
#include <algorithm>

// Local
#include "GaussGeoAssembly.h"
#include "GaussGeoAssemblyStore.h"

GaussGeoAssemblyStore* GaussGeoAssemblyStore::store() 
{
  static GaussGeoAssemblyStore worldStore;
  return &worldStore ;
}

GaussGeoAssemblyStore::GaussGeoAssemblyStore() : m_assemblies() {}

GaussGeoAssemblyStore::~GaussGeoAssemblyStore() {
  clear();
}

GaussGeoAssembly* GaussGeoAssemblyStore::assembly(const std::string& name) {  
  for (Assemblies::reverse_iterator assembly = m_assemblies.rbegin();
       m_assemblies.rend() != assembly;
       ++assembly)
  {
    if (*assembly != nullptr && (*assembly)->name() == name) {
      return *assembly;
    }
  }
  return nullptr;
}

StatusCode GaussGeoAssemblyStore::addAssembly(GaussGeoAssembly* assembly) {
  if (assembly == nullptr) {
    return StatusCode::FAILURE;
  }
  m_assemblies.push_back(assembly);
  return StatusCode::SUCCESS;
}

StatusCode GaussGeoAssemblyStore::removeAssembly(GaussGeoAssembly* assembly) {
  Assemblies::iterator it = std::find(m_assemblies.begin (), m_assemblies.end(), assembly);
  if (m_assemblies.end() != it) {
    m_assemblies.erase(it);
  }
  return StatusCode::SUCCESS;  
}

StatusCode GaussGeoAssemblyStore::clear() {
  while(!m_assemblies.empty()) {
    Assemblies::iterator it = m_assemblies.begin();
    GaussGeoAssembly* assembly = *it;
    m_assemblies.erase(it);
    if (assembly != nullptr) {
      delete assembly;
    }
  }
  return StatusCode::SUCCESS;
}
