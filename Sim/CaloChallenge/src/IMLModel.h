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

#pragma once

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Vector3DTypes.h"

// CLHEP
#include "CLHEP/Random/RandomEngine.h"

namespace Gsino::CaloChallenge {
  /**
   * @brief Interface for ML-based calorimeter hit reconstruction.
   *
   * This interface defines the methods for ML-based calorimeter hit reconstruction.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  struct IMLModel : virtual public extend_interfaces<IAlgTool> {
    using extend_interfaces::extend_interfaces;
    DeclareInterfaceID( IMLModel, 1, 0 );
    virtual void fillShower( std::vector<float>& energies, CLHEP::HepRandomEngine* engine, float particleEnergy,
                             int pid, float theta, float phi ) const = 0;

    void              setMeshNumber( Gaudi::XYZVector meshNumber ) { m_meshNumber = meshNumber; }
    Gaudi::XYZVector& getMeshNumber() { return m_meshNumber; }

  protected:
    Gaudi::XYZVector m_meshNumber{ 0, 0, 0 };
  };
} // namespace Gsino::CaloChallenge
