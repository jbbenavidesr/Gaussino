// Standard
#include <cstdlib>
#include <string>

// MathCore -> CLHEP
#include "ClhepTools/MathCore2Clhep.h"

// Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// GEANT4
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

// Local
#include "GaussGeoAssembly.h"
#include "GaussGeoVolume.h"
#include "GaussGeoVolumeInstall.h"

// ============================================================================
// Install GEANT4 volume inside mother logical volume with given name and position
// ============================================================================
StatusCode GaussGeoVolumeInstall::installVolume(G4LogicalVolume* child_volume,
                                          const std::string& name,
                                          const Gaudi::Transform3D& matrix,
                                          G4LogicalVolume* mother_volume,
                                          MsgStream& msg)
{
  if (child_volume == nullptr) {
      msg << MSG::ERROR
          << " GaussGeoVolumeInstall::installVolume: child_volume is NULL! " << endmsg;

      return StatusCode::FAILURE;
  }

  if (mother_volume == nullptr) {
      msg << MSG::ERROR
          << " GaussGeoVolumeInstall::installVolume: mother_volume is NULL! " << endmsg;

      return StatusCode::FAILURE;
  }

  int ncopy = 1000 + mother_volume->GetNoDaughters();
  {
    const std::string::size_type pos = name.find(':');
    if (std::string::npos != pos) {
      std::string tmp(name, pos + 1, std::string::npos);
      ncopy = atoi(tmp.c_str());
    }
  }

  HepGeom::Transform3D clhepMatrix = LHCb::math2clhep::transform3D(matrix);
  new G4PVPlacement(clhepMatrix.inverse(), child_volume, name, mother_volume, false, ncopy);

  msg << MSG::DEBUG
      << " GaussGeoVolumeInstall:: "
      << " new G4PVPlacement is created with the name '" << name  << "'"
      << " copy number is " << ncopy
      << endmsg;

  return StatusCode::SUCCESS;
}

// ============================================================================
// Install GaussGeo assembly inside mother logical volume with given name and position
// ============================================================================
StatusCode GaussGeoVolumeInstall::installVolume(const GaussGeoAssembly* child_volume,
                                          const std::string& name,
                                          const Gaudi::Transform3D& matrix,
                                          G4LogicalVolume* mother_volume,
                                          MsgStream& msg)
{
  if (child_volume == nullptr) {
    msg << MSG::ERROR << " GaussGeoVolumeInstall::installVolume: child_volume is NULL! " << endmsg;
    return StatusCode::FAILURE;
  }

  if (mother_volume == nullptr) {
    msg << MSG::ERROR << " GaussGeoVolumeInstall::installVolume: mother_volume is NULL! " << endmsg;
    return StatusCode::FAILURE;
  }

  for (GaussGeoAssembly::Volumes::const_iterator ivol = child_volume->volumes().begin();
       child_volume->volumes().end() != ivol; ++ivol)
  {
    G4LogicalVolume* lv = ivol->first.first;
    if(lv == nullptr) {
      msg << MSG::ERROR << " GaussGeoVolumeInstall::installVolume: corrupted assembly '"
          << child_volume->name() << "'" << endmsg;
      return StatusCode::FAILURE;
    }

    StatusCode sc = installVolume(lv, name + "#" + ivol->first.second,
                                  ivol->second * matrix, mother_volume, msg);

    if (sc.isFailure()) {
      msg << MSG::ERROR << " GaussGeoVolumeInstall::installVolume: recursive error! " << endmsg;
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}

// ============================================================================
// Install GaussGeo volume inside mother logical volume with given name and position
// ============================================================================
StatusCode GaussGeoVolumeInstall::installVolume(const GaussGeoVolume& child_volume,
                                          const std::string& name,
                                          const Gaudi::Transform3D& matrix,
                                          G4LogicalVolume* mother_volume,
                                          MsgStream& msg)
{
  if (!child_volume.isValid()) {
    msg << MSG::ERROR << " GaussGeoVolumeInstall::installVolume: child_volume is invalid " << endmsg;
    return StatusCode::FAILURE;
  }

  if (mother_volume == nullptr) {
    msg << MSG::ERROR << " GaussGeoVolumeInstall::installVolume: mother_volume is NULL! " << endmsg;
    return StatusCode::FAILURE;
  }

  if (child_volume.volume() != nullptr) {
    return installVolume(child_volume.volume(), name, matrix, mother_volume, msg);
  } else if(child_volume.assembly() != nullptr) {
    return installVolume(child_volume.assembly(), name, matrix, mother_volume, msg);
  }

  msg << MSG::ERROR << " GaussGeoVolumeInstall::installVolume: fatal error! " << endmsg;

  return StatusCode::FAILURE;
}
