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
  class MaterialFromElements : public MaterialFactory {

    // required
    Gaudi::Property<std::vector<std::string>> m_symbols{ this, "Symbols", {}, "Symbols of the elements" };
    Gaudi::Property<std::vector<double>>      m_fractions{
        this, "MassFractions", {}, "Mass fractions of each element [0.-1.]" };
    Gaudi::Property<std::vector<double>> m_atomicNumbers{
        this, "AtomicNumbers", {}, "Atomic numbers Z of each element" };
    Gaudi::Property<std::vector<double>> m_massNumbers{
        this, "MassNumbers", {}, "Mass numbers A [g/mole] of each element" };

  public:
    using MaterialFactory::MaterialFactory;
    StatusCode  initialize() override;
    G4Material* construct() const override;
  };
} // namespace ExternalDetector

DECLARE_COMPONENT_WITH_ID( ExternalDetector::MaterialFromElements, "MaterialFromElements" )

StatusCode ExternalDetector::MaterialFromElements::initialize() {
  return MaterialFactory::initialize().andThen( [&]() -> StatusCode {
    if ( m_symbols.value().empty() ) {
      error() << "Empty symbol list for the external MaterialFromElements: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_fractions.value().size() != m_symbols.value().size() ) {
      error() << "Mass fractions list must match the size of the symbol list "
              << "for the external MaterialFromElements: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_atomicNumbers.value().size() != m_symbols.value().size() ) {
      error() << "Atomic number list must match the size of the symbol list "
              << "for the external MaterialFromElements: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_massNumbers.value().size() != m_symbols.value().size() ) {
      error() << "Mass number list must match the size of the symbol list "
              << "for the external MaterialFromElements: " << m_name.value() << endmsg;
      return StatusCode::FAILURE;
    }

    for ( auto& fraction : m_fractions.value() ) {
      if ( fraction < 0. || fraction > 1. ) {
        error() << "Invalid mass fraction of one of the elements "
                << "for the external MaterialFromElements: " << m_name.value() << endmsg;
        return StatusCode::FAILURE;
      }
    }

    for ( auto& atomicNumber : m_atomicNumbers.value() ) {
      if ( atomicNumber <= 0 ) {
        error() << "Invalid atomic number of one of the elements "
                << "for the external MaterialFromElements: " << m_name.value() << endmsg;
        return StatusCode::FAILURE;
      }
    }

    for ( auto& massNumber : m_massNumbers.value() ) {
      if ( massNumber <= 0 ) {
        error() << "Invalid mass number of one of the elements "
                << "for the external MaterialFromElements: " << m_name.value() << endmsg;
        return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
  } );
}

G4Material* ExternalDetector::MaterialFromElements::construct() const {
  debug() << "Constructing external MaterialFromElements: " << m_name.value() << endmsg;
  auto g4material =
      new G4Material( m_name.value(), m_density.value(), m_symbols.value().size(), m_state,
                      m_temperature.value() * Gaudi::Units::kelvin, m_pressure.value() * Gaudi::Units::Pa );

  debug() << "Constructing elements for the external MaterialFromElements: " << m_name.value() << endmsg;
  for ( int elID = 0; elID < (int)m_symbols.size(); elID++ ) {
    auto name = m_symbols.value()[elID];
    debug() << "Constructing element " << name << endmsg;
    auto g4element = new G4Element( name, name, m_atomicNumbers.value()[elID], m_massNumbers.value()[elID] );
    if ( !g4element ) {
      error() << "Element " << name << " not constructed." << endmsg;
      return nullptr;
    }
    g4material->AddElement( g4element, m_fractions.value()[elID] );
    debug() << "Constructed element " << name << endmsg;
  }
  if ( !g4material ) {
    error() << "Material: " << m_name.value() << " was not constructed" << endmsg;
    return nullptr;
  }
  debug() << "Constructed external MaterialFromElements: " << m_name.value() << endmsg;
  debug() << "\n" << *( g4material ) << endmsg;
  return g4material;
}
