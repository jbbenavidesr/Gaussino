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

// Gaudi
#include "GaudiAlg/Consumer.h"
#include "GaudiKernel/SystemOfUnits.h"

// Geant4
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectoryPoint.hh"

// LHCb
#include "Phoenix/Store.h"

// Gaussino
#include "Defaults/Locations.h"
#include "GiGaMTCoreRun/G4EventProxy.h"

namespace nlohmann {

  void to_json( json& j, const G4ThreeVector& point ) { j = {point.x(), point.y(), point.z()}; }

  void to_json( json& j, G4VTrajectory* trajectory ) {
    std::vector<G4ThreeVector> points = {};
    for ( int i = 0; i < trajectory->GetPointEntries(); i++ ) {
      auto point = trajectory->GetPoint( i )->GetPosition();
      points.emplace_back( point );
    }
    j = {{"pos", points}};
  }
} // namespace nlohmann

namespace GiGa::Phoenix {
  class DumpG4Trajectories : public Gaudi::Functional::Consumer<void( const G4EventProxies& )> {

    // FIXME: move it from LHCb
    mutable LHCb::Phoenix::Store m_store{this, "G4Trajectories_store", "Phoenix:G4Trajectories"};

    Gaudi::Property<double> m_min_p{this, "MinP", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double> m_max_p{this, "MaxP", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double> m_min_pt{this, "MinPT", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double> m_max_pt{this, "MaxPT", 0 * Gaudi::Units::MeV};

  public:
    DumpG4Trajectories( const std::string& name, ISvcLocator* pSvcLocator )
        : Consumer( name, pSvcLocator, {{"G4EventsLocation", Gaussino::G4EventsLocation::Default}} ) {}

    void operator()( const G4EventProxies& eventproxies ) const override {
      for ( auto& ep : eventproxies ) {
        auto event   = ep->event();
        auto eventID = event->GetEventID();
        // make sure the correct eventID is added
        // see: https://gitlab.cern.ch/Gaussino/Gaussino/-/issues/14

        debug() << "Writing G4Trajectories to JSON "
                << "for event no. " << eventID << endmsg;

        auto trajCont = event->GetTrajectoryContainer();
        if ( !trajCont ) {
          throw GaudiException( "Trajectory container is a nullptr!", "Gaussino::GiGa::Phoenix", StatusCode::FAILURE );
        }

        nlohmann::json trajJSON = {};
        for ( auto& trajectory : *( trajCont->GetVector() ) ) {
          double init_p  = trajectory->GetInitialMomentum().mag();
          double init_pt = trajectory->GetInitialMomentum().perp();
          if ( m_min_p.value() && init_p < m_min_p.value() ) continue;
          if ( m_max_p.value() && init_p > m_max_p.value() ) continue;
          if ( m_min_pt.value() && init_pt < m_min_pt.value() ) continue;
          if ( m_max_pt.value() && init_pt > m_max_pt.value() ) continue;
          trajJSON.emplace_back( trajectory );
        }

        m_store.storeEventData( {{"gps time", 1},   // TODO: for simulations it does not make sense (maybe rethink?)
                                 {"run number", 1}, // TODO: we cannot get it from G4Event... (additional input variable
                                                    // needed)
                                 {"event number", eventID},
                                 {"Content", {{"Tracks", {{"G4Trajectories", trajJSON}}}}}} );
      }
    }
  };
} // namespace GiGa::Phoenix

DECLARE_COMPONENT_WITH_ID( GiGa::Phoenix::DumpG4Trajectories, "GiGaPhoenixDumpG4Trajectories" )
