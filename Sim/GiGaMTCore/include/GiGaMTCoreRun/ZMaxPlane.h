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
#pragma once

#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

/** ZMaxPlane
 *
 *  @brief Creates a tilted plane at z = zMax
 *  @author Michal Mazurek
 *  @date   2021-05-27
 */

class ZMaxPlane {

  Gaudi::Plane3D m_plane;

public:
  /// applies transformations to a z = 0 plane
  void prepare( double zMax, double tilt, double yShift = 0. );

  /// gives distance between zMax plane and point (0., y, z)
  double Distance( double y, double z ) const;

  /// gives distance along non-normal direction between the zMax plane and a point
  double DistanceAlongDirection( const Gaudi::XYZPoint& point, const Gaudi::XYZVector& direction ) const;
};
