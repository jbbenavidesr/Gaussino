/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// local
#include "Embedder.h"
// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// G4
#include "Geant4/G4Box.hh"

namespace ExternalDetector {
  class CuboidEmbedder : public Embedder<G4Box> {
    // size of the cuboid
    Gaudi::Property<double> m_xSize{this, "xSize", 0. * Gaudi::Units::m};
    Gaudi::Property<double> m_ySize{this, "ySize", 0. * Gaudi::Units::m};
    Gaudi::Property<double> m_zSize{this, "zSize", 0. * Gaudi::Units::m};
    // name of the cuboid box
    Gaudi::Property<std::string> m_boxName{this, "BoxName", "Cuboid"};

  public:
    using Embedder::Embedder;

    inline virtual G4Box* build() const override {
      return new G4Box( m_boxName.value(), m_xSize.value() / 2., m_ySize.value() / 2., m_zSize.value() / 2.);
    }
  };
} // namespace ExternalDetector

DECLARE_COMPONENT_WITH_ID( ExternalDetector::CuboidEmbedder, "CuboidEmbedder" )
