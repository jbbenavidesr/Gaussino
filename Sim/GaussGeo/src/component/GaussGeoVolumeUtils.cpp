// GEANT4
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"

// Local
#include "GaussGeoAssembly.h"
#include "GaussGeoAssemblyStore.h"
#include "GaussGeoVolumeUtils.h"

G4LogicalVolume* GaussGeoVolumeUtils::findLVolume(const std::string& name) {
  G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  if (store == nullptr) {
    return nullptr;
  }

  for (G4LogicalVolumeStore::iterator vol_it = store->begin();
       store->end() != vol_it; ++vol_it)
  {
    if (name == (*vol_it)->GetName()) {
      return *vol_it;
    }
  }

  return (G4LogicalVolume*) 0;
}

G4VPhysicalVolume* GaussGeoVolumeUtils::findPVolume(const std::string& name) {
  G4PhysicalVolumeStore* store = G4PhysicalVolumeStore::GetInstance();
  if (store == nullptr) {
    return nullptr;
  }

  for (G4PhysicalVolumeStore::iterator vol_it = store->begin();
       store->end() != vol_it; ++vol_it)
  {
    if (name == (*vol_it)->GetName()) {
      return *vol_it;
    }
  }

  return (G4VPhysicalVolume*) 0;
}

GaussGeoAssembly* GaussGeoVolumeUtils::findLAssembly(const std::string& name) {
  GaussGeoAssemblyStore* store = GaussGeoAssemblyStore::store();
  if (store != nullptr) {
    return store->assembly(name);
  }

  return (GaussGeoAssembly*) 0;
}

G4Region* GaussGeoVolumeUtils::findRegion(const std::string& name) {
  G4RegionStore* store = G4RegionStore::GetInstance();
  if (store == nullptr) {
    return nullptr;
  }

  return store->GetRegion(name);
}
