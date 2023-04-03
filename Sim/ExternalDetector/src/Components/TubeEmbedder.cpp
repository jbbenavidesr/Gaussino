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
// local
#include "ExternalDetector/Embedder.h"
// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// G4
#include "G4Tubs.hh"

namespace ExternalDetector {
  class TubeEmbedder : public Embedder<G4Tubs> {
    // properties of the tube
    Gaudi::Property<double>      m_rMin{this, "RMin", 0. * Gaudi::Units::m, "Inner radius"};
    Gaudi::Property<double>      m_rMax{this, "RMax", 0. * Gaudi::Units::m, "Outer radius"};
    Gaudi::Property<double>      m_Dz{this, "Dz", 0. * Gaudi::Units::m, "Half length in z"};
    Gaudi::Property<double>      m_sPhi{this, "SPhi", 0. * Gaudi::Units::radian, "The starting phi angle in radians"};
    Gaudi::Property<double>      m_dPhi{this, "DPhi", 0. * Gaudi::Units::radian, "Delta angle of the segment"};
    Gaudi::Property<std::string> m_name{this, "TubeName", "ExternalTube"};

  public:
    using Embedder::Embedder;

    inline virtual G4Tubs* build() const override {
      return new G4Tubs( m_name.value(), m_rMin.value(), m_rMax.value(), m_Dz.value(), m_sPhi.value(), m_dPhi.value() );
    }
  };
} // namespace ExternalDetector

DECLARE_COMPONENT_WITH_ID( ExternalDetector::TubeEmbedder, "TubeEmbedder" )
