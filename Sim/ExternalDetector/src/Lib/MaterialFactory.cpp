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

#include "ExternalDetector/MaterialFactory.h"

StatusCode ExternalDetector::MaterialFactory::initialize() {
  return extends::initialize().andThen( [&]() -> StatusCode {
    if ( m_name.value().empty() ) {
      error() << "External Material must have a unique name" << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_density.value() <= 0. ) {
      error() << "Invalid density for the external material: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_temperature.value() <= 0. ) {
      error() << "Invalid temperature for the external material: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_pressure.value() <= 0. ) {
      error() << "Invalid pressure for the external material: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_stateName.value() == "Solid" ) {
      m_state = G4State::kStateSolid;
    } else if ( m_stateName.value() == "Liquid" ) {
      m_state = G4State::kStateLiquid;
    } else if ( m_stateName.value() == "Gas" ) {
      m_state = G4State::kStateGas;
    } else {
      warning() << "Unrecognized state for the external material: " << m_name.value() << ". Leaving Undefined"
                << endmsg;
    }

    return StatusCode::SUCCESS;
  } );
}
