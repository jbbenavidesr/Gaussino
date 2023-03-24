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

// Geant4
#include "G4NistManager.hh"

// Gaussino
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

namespace ExternalDetector {
  class MaterialFromNIST : public extends<GiGaTool, GiGaFactoryBase<G4Material>> {

    Gaudi::Property<std::string> m_name{this, "Name", {}, "Name as in G4/NIST"};

  public:
    using extends::extends;
    StatusCode  initialize() override;
    G4Material* construct() const override;
  };
} // namespace ExternalDetector

DECLARE_COMPONENT_WITH_ID( ExternalDetector::MaterialFromNIST, "MaterialFromNIST" )

StatusCode ExternalDetector::MaterialFromNIST::initialize() {
  return extends::initialize().andThen( [&]() -> StatusCode {
    if ( m_name.value().empty() ) {
      error() << "Empty material name!" << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  } );
}

G4Material* ExternalDetector::MaterialFromNIST::construct() const {
  debug() << "Constructing external MaterialFromNIST: " << m_name << endmsg;

  auto g4material = G4NistManager::Instance()->FindOrBuildMaterial( m_name.value(), false );

  if ( !g4material ) {
    error() << "Material: " << m_name.value() << " was not constructed" << endmsg;
    return nullptr;
  }

  debug() << "Constructed external MaterialFromNIST: " << m_name.value() << endmsg;
  debug() << "\n" << *( g4material ) << endmsg;
  return g4material;
}
