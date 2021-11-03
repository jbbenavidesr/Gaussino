/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Transform3DTypes.h"
// local
#include "GiGaMTCoreTruth/ZMaxPlane.h"

void ZMaxPlane::prepare( double zMax, double tilt, double yShift ) {
  // apply a rotation around X axis by tilt (radians)
  auto rotation = Gaudi::RotationX( tilt );
  // apply a translation in z by zMax
  auto translation = Gaudi::TranslationXYZ( 0., yShift, zMax );
  this->m_plane    = Gaudi::Transform3D( rotation, translation )( Gaudi::Plane3D() );
}

double ZMaxPlane::Distance( double y, double z ) const {
  auto point = Gaudi::Plane3D::Point( 0., y, z );
  // distance is (+) at the side where the normal vector is
  // and (-) on the other
  return this->m_plane.Distance( point );
}
