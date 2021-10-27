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
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SystemOfUnits.h"
// GiGa
#include "GiGaMTCoreDet/IExternalDetectorEmbedder.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
// Geant4
#include "Geant4/G4VSensitiveDetector.hh"

class G4VSolid;
class G4VPhysicalVolume;
class G4LogicalVolume;

namespace ExternalDetector {

  template <class Solid>
  class Embedder : public extends<GaudiTool, IEmbedder> {

    static_assert( std::is_base_of<G4VSolid, Solid>::value );

  protected:
    // position of the volume
    Gaudi::Property<double> m_xPos{this, "xPos", 0. * Gaudi::Units::mm};
    Gaudi::Property<double> m_yPos{this, "yPos", 0. * Gaudi::Units::mm};
    Gaudi::Property<double> m_zPos{this, "zPos", 0. * Gaudi::Units::mm};
    // rotation around x axis
    Gaudi::Property<double> m_xAngle{this, "xAngle", 0. * Gaudi::Units::degree};
    // material of the volume
    Gaudi::Property<std::string> m_materialName{this, "MaterialName", std::string()};
    // name of the logical volume
    Gaudi::Property<std::string> m_lVolName{this, "LogicalVolumeName", "CustomLVol"};
    Gaudi::Property<std::string> m_pVolName{this, "PhysicalVolumeName", "CustomPVol"};
    // name of the sensitive detector
    ToolHandle<GiGaFactoryBase<G4VSensitiveDetector>> m_sensDetName{this, "SensDetName", ""};

  public:
    using extends::extends;

    StatusCode initialize() override;

    virtual Solid*             build() const = 0;
    virtual StatusCode         embed( G4VPhysicalVolume* motherVolume ) const override;
    virtual G4VPhysicalVolume* place( G4LogicalVolume* motherLVolume ) const;
  };
} // namespace ExternalDetector
