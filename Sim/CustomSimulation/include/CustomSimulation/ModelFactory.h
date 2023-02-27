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

// Geant4
#include "G4VFastSimulationModel.hh"

// Gaussino
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

namespace Gaussino::CustomSimulation {
  template <class TModel>
  class ModelFactory : public extends<GiGaTool, GiGaFactoryBase<G4VFastSimulationModel>> {
    static_assert( std::is_base_of<G4VFastSimulationModel, TModel>::value );

    Gaudi::Property<std::string> m_region{this, "RegionName", ""};
    Gaudi::Property<std::string> m_model{this, "Name", ""};

  public:
    using extends::extends;

    StatusCode initialize() override;
    TModel*    construct() const override;
  };
} // namespace Gaussino::CustomSimulation

#include "ModelFactory.icpp"
