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

// local
#include "ExternalDetector/MaterialFactory.h"

namespace ExternalDetector {
  class MaterialFromChemicalPropertiesFactory : public MaterialFactory {

    // required
    Gaudi::Property<double> m_atomicNumber{this, "AtomicNumber", 0., "Atomic number Z"};
    Gaudi::Property<double> m_massNumber{this, "MassNumber", 0., "Mass number A [g/mole]"};

  public:
    using MaterialFactory::MaterialFactory;
    StatusCode  initialize() override;
    G4Material* construct() const override;
  };
} // namespace ExternalDetector

DECLARE_COMPONENT_WITH_ID( ExternalDetector::MaterialFromChemicalPropertiesFactory, "MaterialFromChemicalProperties" )

StatusCode ExternalDetector::MaterialFromChemicalPropertiesFactory::initialize() {
  return MaterialFactory::initialize().andThen( [&]() -> StatusCode {
    if ( m_atomicNumber.value() <= 0. ) {
      error() << "Invalid atomic number for the external MaterialFromChemicalPropertiesFactory: " << m_name.value()
              << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_massNumber.value() <= 0. ) {
      error() << "Invalid mass number for the external MaterialFromChemicalPropertiesFactory: " << m_name.value()
              << endmsg;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  } );
}

G4Material* ExternalDetector::MaterialFromChemicalPropertiesFactory::construct() const {
  debug() << "Constructing external MaterialFromChemicalPropertiesFactory: " << m_name.value() << endmsg;
  auto g4material = new G4Material(
      m_name.value(), m_atomicNumber.value(), m_massNumber.value() * Gaudi::Units::g / Gaudi::Units::mole,
      m_density.value() * Gaudi::Units::g / Gaudi::Units::cm3, m_state, m_temperature.value() * Gaudi::Units::kelvin,
      m_pressure.value() * Gaudi::Units::Pa );
  if ( !g4material ) {
    error() << "Material: " << m_name.value() << " was not constructed" << endmsg;
    return nullptr;
  }
  debug() << "Constructed external MaterialFromElements: " << m_name.value() << endmsg;
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "\n" << *( g4material ) << endmsg;
    // g4material->GetMaterialPropertiesTable()->DumpTable();
  }
  return g4material;
}
