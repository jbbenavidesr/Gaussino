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

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"
// Geant4
#include "Geant4/G4Material.hh"
// GiGaMT
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

namespace ExternalDetector {
  class MaterialFactory : public extends<GiGaTool, GiGaFactoryBase<G4Material>> {
  protected:
    // required
    Gaudi::Property<std::string> m_name{this, "Name", ""};
    Gaudi::Property<double>      m_density{this, "Density", 0., "Density in g/cm3"};
    // optional
    Gaudi::Property<double>      m_pressure{this, "Pressure", Gaudi::Units::STP_Pressure, "Pressure in Pa"};
    Gaudi::Property<double>      m_temperature{this, "Temperature", Gaudi::Units::STP_Temperature, "Temperature in K"};
    Gaudi::Property<std::string> m_stateName{this, "State", "Undefined", "Undefined/Solid/Liquid/Gas"};

    G4State m_state = G4State::kStateUndefined;

  public:
    using extends::extends;
    StatusCode initialize() override;
  };
} // namespace ExternalDetector
