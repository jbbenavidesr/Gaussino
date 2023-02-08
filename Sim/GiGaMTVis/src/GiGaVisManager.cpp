/*****************************************************************************\
* (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// G4
#include "G4ASCIITree.hh"
#include "G4DAWNFILE.hh"
#include "G4HepRepFile.hh"
#include "G4VisManager.hh"
#ifdef G4VIS_USE_OPENGLX
#  include "G4OpenGLImmediateX.hh"
#  include "G4OpenGLStoredX.hh"
#endif

#include "G4DigiFilterFactories.hh"
#include "G4HitFilterFactories.hh"
#include "G4TrajectoryFilterFactories.hh"
#include "G4TrajectoryModelFactories.hh"

#include "G4VFilter.hh"
#include "G4VModelFactory.hh"
#include "G4VTrajectory.hh"
// Gaussino
#include "GiGaMTCoreMessage/IGiGaMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"
#include "Utils/ToolProperty.h"

namespace GiGa {
  class VisManagerFactory;
  class VisManager : public G4VisManager, public GiGaMessage {
    friend class VisManagerFactory;

  public:
    using G4TrajFilterFactory = G4VModelFactory<G4VFilter<G4VTrajectory>>;
    VisManager( std::string verbosityString ) : G4VisManager( verbosityString ){};
    void RegisterGraphicsSystems() override;
    void RegisterModelFactories() override;

  private:
    std::vector<G4TrajFilterFactory*> m_traj_factories  = {};
    std::string                       m_required_driver = "";
  };

  class VisManagerFactory : public extends<GiGaTool, GiGaFactoryBase<G4VisManager>> {
    ToolHandleArray<GiGaFactoryBase<G4TrajFilterFactory>> m_traj_factories{this};
    Gaudi::Property<std::vector<std::string>>             m_traj_factories_names{
        this,
        "TrajectoryFactories",
        {},
        tool_array_setter( m_traj_factories, m_traj_factories_names ),
        Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};
    Gaudi::Property<std::string> m_required_driver{this, "RequiredDriver", ""};

  public:
    using extends::extends;
    std::string           verbosityString() const;
    virtual G4VisManager* construct() const override;
  };
} // namespace GiGa

DECLARE_COMPONENT_WITH_ID( GiGa::VisManagerFactory, "GiGaVisManager" )

void GiGa::VisManager::RegisterGraphicsSystems() {
  // Graphics Systems not needing external packages or libraries...
  if ( m_required_driver == "ASCIITree" ) {
    RegisterGraphicsSystem( new G4ASCIITree );
  } else if ( m_required_driver == "DAWNFILE" ) {
    RegisterGraphicsSystem( new G4DAWNFILE );
  } else if ( m_required_driver == "HepRep" ) {
    RegisterGraphicsSystem( new G4HepRepFile );
  } else if ( m_required_driver == "OpenGLImmediateX" ) {
#ifdef G4VIS_USE_OPENGLX
    RegisterGraphicsSystem( new G4OpenGLImmediateX );
#else
    G4Exception( "GiGa::VisManager::RegisterGraphicsSystems()", "PART112", FatalException,
                 "G4 not built with OPENGLX" );
#endif
  } else if ( m_required_driver == "OpenGLStoredX" ) {
#ifdef G4VIS_USE_OPENGLX
    RegisterGraphicsSystem( new G4OpenGLStoredX );
#else
    G4Exception( "GiGa::VisManager::RegisterGraphicsSystems()", "PART112", FatalException,
                 "G4 not built with OPENGLX" );
#endif
  } else {
    G4cout << "Requested driver is not available!" << G4endl;
  }
  // debug("You have successfully chosen to use the following graphics systems:");
  // PrintAvailableGraphicsSystems (G4VisManager::Verbosity::all);
}

void GiGa::VisManager::RegisterModelFactories() {

  // Trajectory draw models
  RegisterModelFactory( new G4TrajectoryGenericDrawerFactory() );
  RegisterModelFactory( new G4TrajectoryDrawByAttributeFactory() );
  RegisterModelFactory( new G4TrajectoryDrawByChargeFactory() );
  RegisterModelFactory( new G4TrajectoryDrawByOriginVolumeFactory() );
  RegisterModelFactory( new G4TrajectoryDrawByParticleIDFactory() );
  RegisterModelFactory( new G4TrajectoryDrawByEncounteredVolumeFactory() );

  // Trajectory filter models
  RegisterModelFactory( new G4TrajectoryAttributeFilterFactory() );
  RegisterModelFactory( new G4TrajectoryChargeFilterFactory() );
  RegisterModelFactory( new G4TrajectoryOriginVolumeFilterFactory() );
  RegisterModelFactory( new G4TrajectoryParticleFilterFactory() );
  RegisterModelFactory( new G4TrajectoryEncounteredVolumeFilterFactory() );

  // Hit filter models
  RegisterModelFactory( new G4HitAttributeFilterFactory() );
  /*
  // Digi filter models
  RegisterModelFactory(new G4DigiAttributeFilterFactory());
  */

  for ( auto& factory : m_traj_factories ) { RegisterModelFactory( factory ); }
}

std::string GiGa::VisManagerFactory::verbosityString() const {
  switch ( message_interface()->level() ) {
  case 0:
    return "quiet";
  case 5:
    return "errors";
  case 3:
  case 2:
  case 1:
    return "all";
  }
  return "warnings";
}

G4VisManager* GiGa::VisManagerFactory::construct() const {
  auto vis_mgr = new VisManager{verbosityString()};
  vis_mgr->SetMessageInterface( message_interface() );
  vis_mgr->m_required_driver = m_required_driver;
  for ( auto& factory : m_traj_factories ) { vis_mgr->m_traj_factories.emplace_back( factory->construct() ); }
  vis_mgr->Initialize();
  return vis_mgr;
}
