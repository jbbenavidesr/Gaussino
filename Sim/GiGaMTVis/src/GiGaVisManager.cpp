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
#include "G4VisManager.hh"
/*
#include "G4DigiFilterFactories.hh"
#include "G4HitFilterFactories.hh"
#include "G4TrajectoryFilterFactories.hh"
#include "G4TrajectoryModelFactories.hh"
*/
// Gaussino
#include "GiGaMTCoreMessage/IGiGaMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

namespace GiGa {
  class VisManager : public G4VisManager, public GiGaMessage {
  public:
    VisManager( std::string verbosityString ) : G4VisManager( verbosityString ){};
    void RegisterGraphicsSystems() override;
    void RegisterModelFactories() override;
  };

  class VisManagerFactory : public extends<GiGaTool, GiGaFactoryBase<G4VisManager>> {
  public:
    using extends::extends;
    std::string           verbosityString() const;
    virtual G4VisManager* construct() const override;
  };
} // namespace GiGa

DECLARE_COMPONENT_WITH_ID( GiGa::VisManagerFactory, "GiGaVisManager" )

void GiGa::VisManager::RegisterGraphicsSystems() {
  // Graphics Systems not needing external packages or libraries...
  RegisterGraphicsSystem( new G4ASCIITree );
  // debug("You have successfully chosen to use the following graphics systems:");
  // PrintAvailableGraphicsSystems (G4VisManager::Verbosity::all);
}

void GiGa::VisManager::RegisterModelFactories() {
  /*
  // Trajectory draw models
  RegisterModelFactory(new G4TrajectoryGenericDrawerFactory());
  RegisterModelFactory(new G4TrajectoryDrawByAttributeFactory());
  RegisterModelFactory(new G4TrajectoryDrawByChargeFactory());
  RegisterModelFactory(new G4TrajectoryDrawByOriginVolumeFactory());
  RegisterModelFactory(new G4TrajectoryDrawByParticleIDFactory());
  RegisterModelFactory(new G4TrajectoryDrawByEncounteredVolumeFactory());

  // Trajectory filter models
  RegisterModelFactory(new G4TrajectoryAttributeFilterFactory());
  RegisterModelFactory(new G4TrajectoryChargeFilterFactory());
  RegisterModelFactory(new G4TrajectoryOriginVolumeFilterFactory());
  RegisterModelFactory(new G4TrajectoryParticleFilterFactory());
  RegisterModelFactory(new G4TrajectoryEncounteredVolumeFilterFactory());

  // Hit filter models
  RegisterModelFactory(new G4HitAttributeFilterFactory());

  // Digi filter models
  RegisterModelFactory(new G4DigiAttributeFilterFactory());
  */
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
  vis_mgr->Initialize();
  return vis_mgr;
}
