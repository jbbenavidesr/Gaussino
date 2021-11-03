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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utestZMaxTilt
#include "GaudiKernel/SystemOfUnits.h"
#include "GiGaMTCoreTruth/ZMaxPlane.h"
#include <boost/test/unit_test.hpp>
#include <iostream>

double eps = .001;

BOOST_AUTO_TEST_CASE( noTiltTest ) {
  // perpendicular plane at z = 1300.
  auto plane = ZMaxPlane();
  plane.prepare( 1300., 0. * Gaudi::Units::degree );
  // here y does not matter because plane is just in z
  auto dist_right = plane.Distance( 10456., 1456. );
  auto dist_left  = plane.Distance( 10456., 1144. );
  BOOST_CHECK( std::abs( dist_right - 156. ) < eps );
  BOOST_CHECK( std::abs( dist_left + 156. ) < eps );
}

BOOST_AUTO_TEST_CASE( flippedTest ) {
  // tilted plane at z = 1300. by +90.0 degrees
  auto plane = ZMaxPlane();
  plane.prepare( 1300., 90. * Gaudi::Units::degree );
  // here y does matter
  auto dist_left  = plane.Distance( 10456, 1456. );
  auto dist_right = plane.Distance( -10456, 1456. );
  BOOST_CHECK( std::abs( dist_left + 10456. ) < eps );
  BOOST_CHECK( std::abs( dist_right - 10456. ) < eps );
}

BOOST_AUTO_TEST_CASE( withTiltTest ) {
  // tilted plane at z = 1300. by +45.0 degrees
  auto plane = ZMaxPlane();
  plane.prepare( 1300., 45. * Gaudi::Units::degree );
  // here y does matter
  auto dist_left  = plane.Distance( 10456., 1456. );
  auto dist_right = plane.Distance( -10456., 1456. );
  auto pred_left  = -( 10456. - 156. ) * std::sqrt( 2. ) / 2.;
  auto pred_right = ( 10456. + 156. ) * std::sqrt( 2. ) / 2.;
  BOOST_CHECK( std::abs( dist_left - pred_left ) < eps );
  BOOST_CHECK( std::abs( dist_right - pred_right ) < eps );
}
