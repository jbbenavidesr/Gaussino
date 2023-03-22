/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

// Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/SystemOfUnits.h"

// Gaussino
#include "GiGaMTMagnetFactories/MagneticField.h"

std::once_flag probed;

void Gaussino::MagneticField::Field::GetFieldValue( const double Point[4], double* B ) const {
  if ( !Point ) { throw GaudiException( "GetFieldValue: Point = 0 !", "MagneticField", StatusCode::FAILURE ); }
  if ( !B ) { throw GaudiException( "GetFieldValue: B     = 0 !", "MagneticField", StatusCode::FAILURE ); }
  const Gaudi::XYZPoint point( Point[0], Point[1], Point[2] );
  auto                  field = fieldVector( point );
  *( B + 0 )                  = field.x();
  *( B + 1 )                  = field.y();
  *( B + 2 )                  = field.z();
  std::call_once( probed, [&]() {
    std::stringstream msg;
    msg << "Probing the magnetic field at: [";
    msg << " x: " << point.x() / Gaudi::Units::mm << ",";
    msg << " y: " << point.y() / Gaudi::Units::mm << ",";
    msg << " z: " << point.z() / Gaudi::Units::mm << "]";
    msg << " (mm) is: [";
    msg << " B_x: " << field.x() / Gaudi::Units::tesla << ",";
    msg << " B_y: " << field.y() / Gaudi::Units::tesla << ",";
    msg << " B_z: " << field.z() / Gaudi::Units::tesla << "] (T)";
    debug( msg.str() );
  } );
}