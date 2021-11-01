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
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"
// Geant4
#include "Geant4/G4Material.hh"
// GiGaMT
#include "GiGaMTCoreDet/IExternalDetectorMaterialEmbedder.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

namespace ExternalDetector {
  class MaterialFactory : public extends<GiGaTool, IMaterialEmbedder> {

    // required
    Gaudi::Property<std::string> m_name{this, "Name", ""};
    Gaudi::Property<double>      m_atomicMass{this, "AtomicMass", 0., "Atomic mass"};
    Gaudi::Property<double>      m_massNumber{this, "MassNumber", 0., "Mass number in g/mole"};
    Gaudi::Property<double>      m_density{this, "Density", 0., "Density in g/cm3"};

    // optional
    Gaudi::Property<double>      m_pressure{this, "Pressure", Gaudi::Units::STP_Pressure, "Pressure in Pa"};
    Gaudi::Property<double>      m_temperature{this, "Temperature", Gaudi::Units::STP_Temperature, "Temperature in K"};
    Gaudi::Property<std::string> m_stateName{this, "State", "Undefined", "Undefined/Solid/Liquid/Gas"};

    G4State m_state = G4State::kStateUndefined;

  public:
    using extends::extends;
    StatusCode initialize() override;
    StatusCode embed() const override;
  };
} // namespace ExternalDetector

DECLARE_COMPONENT_WITH_ID( ExternalDetector::MaterialFactory, "ExternalMaterial" )

StatusCode ExternalDetector::MaterialFactory::initialize() {
  return extends::initialize().andThen( [&]() -> StatusCode {
    if ( m_name.value().empty() ) {
      error() << "External Material must have a unique name" << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_atomicMass.value() <= 0. ) {
      error() << "Invalid atomic mass for the exterial material: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_massNumber.value() <= 0. ) {
      error() << "Invalid mass number for the exterial material: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_density.value() <= 0. ) {
      error() << "Invalid density for the exterial material: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_temperature.value() <= 0. ) {
      error() << "Invalid temperature for the exterial material: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_pressure.value() <= 0. ) {
      error() << "Invalid pressure for the exterial material: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_stateName.value() == "Solid" ) {
      m_state = G4State::kStateSolid;
    } else if ( m_stateName.value() == "Liquid" ) {
      m_state = G4State::kStateLiquid;
    } else if ( m_stateName.value() == "Gas" ) {
      m_state = G4State::kStateGas;
    } else {
      warning() << "Unrecognized state for the exterial material: " << m_name.value() << ". Leaving Undefined"
                << endmsg;
    }

    return StatusCode::SUCCESS;
  } );
}

StatusCode ExternalDetector::MaterialFactory::embed() const {
  debug() << "Embedding external material: " << m_name.value() << endmsg;
  auto g4material =
      new G4Material( m_name.value(), m_atomicMass.value(), m_massNumber.value() * Gaudi::Units::g / Gaudi::Units::mole,
                      m_density.value() * Gaudi::Units::g / Gaudi::Units::cm3, m_state,
                      m_temperature.value() * Gaudi::Units::kelvin, m_pressure.value() * Gaudi::Units::Pa );
  if ( !g4material ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}
