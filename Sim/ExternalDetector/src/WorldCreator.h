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
#pragma once
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/SystemOfUnits.h"
// GiGa
#include "GiGaMTCoreDet/IExternalDetectorEmbedder.h"

namespace ExternalDetector {
  class WorldCreator : public GaudiAlgorithm {

    Gaudi::Property<std::string> m_worldMaterial{this, "WorldMaterial", "/dd/Materials/Air"};
    Gaudi::Property<double>      m_worldSizeX{this, "WorldSizeX", 50. * Gaudi::Units::m};
    Gaudi::Property<double>      m_worldSizeY{this, "WorldSizeY", 50. * Gaudi::Units::m};
    Gaudi::Property<double>      m_worldSizeZ{this, "WorldSizeZ", 50. * Gaudi::Units::m};

    // gdml optional conf, we cannot use GDMLRunAction cause we don't use GiGa here!
    Gaudi::Property<bool>        m_writeGDML{this, "WriteGDML", false};
    Gaudi::Property<std::string> m_schema{this, "Schema", "$GDML_base/src/GDMLSchema/gdml.xsd"};
    Gaudi::Property<std::string> m_outfile{this, "Output", "ExternalWorld.gdml"};

    ToolHandleArray<IExternalDetectorEmbedder> m_ext_dets{this};
    Gaudi::Property<std::vector<std::string>>  m_ext_dets_names{
        this,
        "ExternalDetectors",
        {},
        [=]( auto& ) {
          for ( auto& ext_det : m_ext_dets_names.value() ) {
            if ( !ext_det.empty() ) { m_ext_dets.push_back( ext_det ); }
          }
        },
        Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};

  public:
    using GaudiAlgorithm::GaudiAlgorithm;
    StatusCode initialize() override;
  };
} // namespace ExternalDetector
