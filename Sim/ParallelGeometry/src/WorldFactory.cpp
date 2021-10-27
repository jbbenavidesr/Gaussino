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
// Gaudi
#include "GaudiAlg/GaudiTool.h"
// GiGa
#include "GiGa/IExternalDetectorEmbedder.h"
#include "GiGa/IGiGaSensDet.h"
#include "GiGa/IParallelWorldFactory.h"
// local
#include "DefaultWorld.h"

namespace ParallelGeometry {

  // G-on-G-style factory
  class WorldFactory : public extends<GaudiTool, IParallelWorldFactory> {

  public:
    using extends::extends;
    inline virtual DefaultWorld* construct() const override;

  private:
    Gaudi::Property<std::string> m_worldMaterial{this, "WorldMaterial", ""};

    // External Detectors
    ToolHandleArray<IExternalDetectorEmbedder> m_ext_dets{this};
    using ExtDets = std::vector<std::string>;
    Gaudi::Property<ExtDets> m_ext_dets_names{this,
                                              "ExternalDetectors",
                                              {},
                                              [=]( auto& ) {
                                                for ( auto& ext_det : m_ext_dets_names.value() ) {
                                                  if ( !ext_det.empty() ) { m_ext_dets.push_back( ext_det ); }
                                                }
                                              },
                                              Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};

    /*
    std::map<std::string, ToolHandle<IGiGaSensDet>> m_vols_to_sensdettools;
    using SensitiveVolumesNames = std::map<std::string, std::string>;
    Gaudi::Property<SensitiveVolumesNames> m_sensdet_names {
      this,
      "SensitiveVolumes",
      {},
      [=]( auto& ) {
        for ( auto const& [log_vol_name, sens_det_name] : m_sensdet_names.value() ) {
          if ( !log_vol_name.empty() && !sens_det_name.empty() ) {
            m_vols_to_sensdettools.emplace( std::piecewise_construct,
                                            std::forward_as_tuple(log_vol_name),
                                            std::forward_as_tuple(sens_det_name, this) );
          }
        }
      },
      Gaudi::Details::Property::ImmediatelyInvokeHandler{true}
    };
    */
  };
} // namespace ParallelGeometry

inline ParallelGeometry::DefaultWorld* ParallelGeometry::WorldFactory::construct() const {

  auto                   world_name = name();
  std::string::size_type posdot     = world_name.find( "." );
  while ( posdot != std::string::npos ) {
    world_name.erase( 0, posdot + 1 );
    posdot = world_name.find( "." );
  }

  auto par_world = new DefaultWorld{world_name};

  debug() << "Constructing world for " << name() << " parallel world" << endmsg;
  par_world->setWorldConstructor( [&]( G4VPhysicalVolume* world ) {
    if ( !m_worldMaterial.value().empty() ) {
      auto lvol     = world->GetLogicalVolume();
      auto material = G4Material::GetMaterial( m_worldMaterial.value(), true );
      if ( material ) {
        debug() << "Setting " << m_worldMaterial.value() << " as the parallel world's material for " << lvol->GetName()
                << endmsg;
        lvol->SetMaterial( material );
      } else {
        warning() << "Material " << m_worldMaterial.value() << " not found. Leaving nullptr." << endmsg;
      }
    }

    // Import external geometry
    for ( auto& embedder : m_ext_dets ) {
      debug() << "Calling embedder" << embedder->name() << " in " << world->GetName() << endmsg;
      embedder->embed( world ).ignore();
    }
  } );

  debug() << "Constructing SD for " << name() << " parallel world" << endmsg;
  par_world->setSDConstructor( [&]() {
    // empty for now

    // SensitiveVolumes vols_to_sensdets;
    // for ( auto const& [log_vol_name, sens_det_tool] : m_vols_to_sensdettools) {
    //  vols_to_sensdets.emplace(log_vol_name, sens_det_tool.get());
    //}
    // world->setSensitiveVolumes(vols_to_sensdets);
  } );

  return par_world;
}

using ParallelWorld = ParallelGeometry::WorldFactory;
DECLARE_COMPONENT_WITH_ID( ParallelWorld, "DefaultParallelWorld" )
