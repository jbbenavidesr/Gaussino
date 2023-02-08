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
#include "G4RichTrajectory.hh"
#include "G4SmoothTrajectory.hh"
#include "G4Trajectory.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VHit.hh"
#include "G4VTrajectoryPoint.hh"
// LHCb
#include "Phoenix/Store.h"

// Gaussino
#include "Defaults/Locations.h"
#include "GiGaMTCoreRun/G4EventProxy.h"

namespace nlohmann {

  void to_json( json& j, const G4ThreeVector& point ) { j = {point.x(), point.y(), point.z()}; }

  void to_json( json& j, std::pair<G4VTrajectory*, json> traj_params ) {
    auto                       trajectory            = traj_params.first;
    auto                       trajectory_properties = traj_params.second;
    std::vector<G4ThreeVector> points                = {};
    for ( int i = 0; i < trajectory->GetPointEntries(); i++ ) {
      auto point = trajectory->GetPoint( i )->GetPosition();
      points.emplace_back( point );
    }
    j        = trajectory_properties;
    j["pos"] = points;
  }
} // namespace nlohmann

namespace GiGa::Phoenix {
  class DumpG4Trajectories : public Gaudi::Functional::Consumer<void( const G4EventProxies& )> {

    // FIXME: move it from LHCb
    mutable LHCb::Phoenix::Store              m_store{this, "G4Trajectories_store", "Phoenix:G4Trajectories"};
    Gaudi::Property<std::vector<std::string>> m_trajectory_properties{
        this, "TrajectoryProperties", {"charge", "mom", "pT", "eta"}};
    Gaudi::Property<std::string>                        m_trajectory_model{this, "TrajectoryModel", "DrawByCharge"};
    Gaudi::Property<std::map<std::string, std::string>> m_charge_colors{
        this, "ChargeColors", {{"Positive", "0000FF"}, {"Negative", "FF0000"}, {"Neutral", "00FF00"}}};
    Gaudi::Property<std::map<std::string, std::string>>               m_particle_id_colors{this,
                                                                             "ParticleIDColors",
                                                                             {{"gamma", "00FF00"},
                                                                              {"e-", "FF0000"},
                                                                              {"e+", "0000FF"},
                                                                              {"pi-", "FF00FF"},
                                                                              {"pi+", "FF00FF"},
                                                                              {"proton", "00FFFF"}}};
    Gaudi::Property<std::map<std::string, std::pair<double, double>>> m_interval_colors{
        this,
        "IntervalColors",
        {{"FFAA00", {0 * Gaudi::Units::MeV, 25 * Gaudi::Units::MeV}},
         {"00AA00", {25 * Gaudi::Units::MeV, 100 * Gaudi::Units::MeV}},
         {"000033", {150 * Gaudi::Units::MeV, 1000 * Gaudi::Units::MeV}}}};
    Gaudi::Property<double>              m_min_p{this, "MinP", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double>              m_max_p{this, "MaxP", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double>              m_min_pt{this, "MinPT", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double>              m_max_pt{this, "MaxPT", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double>              m_min_ke{this, "MinKE", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double>              m_max_ke{this, "MaxKE", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double>              m_min_eta{this, "MinEta", 0};
    Gaudi::Property<double>              m_max_eta{this, "MaxEta", 0};
    Gaudi::Property<std::string>         m_trajectory_type{this, "TrajectoryType", ""};
    Gaudi::Property<std::vector<double>> m_accepted_charges{this, "AcceptedCharges", {-1.0, 0.0, 1.0}};

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

        auto hitsColl = event->GetHCofThisEvent();

        if ( !hitsColl ) {
          error() << "Hits collection is a nullptr! "
                  << "Skipping event no. " << eventID << endmsg;
          continue;
        }

        nlohmann::json trajJSON = {};
        for ( auto& trajectory : *( trajCont->GetVector() ) ) {
          auto        traj_momentum = trajectory->GetInitialMomentum();
          double      init_p        = traj_momentum.mag();
          double      init_pt       = traj_momentum.perp();
          double      init_eta      = traj_momentum.pseudoRapidity();
          double      charge        = trajectory->GetCharge();
          double      init_ke       = 0.0;
          std::string pid           = trajectory->GetParticleName();

          if ( m_trajectory_type.value().empty() ) {
            auto normal_trajectory = dynamic_cast<G4Trajectory*>( trajectory );
            if ( normal_trajectory ) init_ke = normal_trajectory->GetInitialKineticEnergy();
          } else if ( m_trajectory_type.value() == "smooth" ) {
            auto smooth_trajectory = dynamic_cast<G4SmoothTrajectory*>( trajectory );
            if ( smooth_trajectory ) init_ke = smooth_trajectory->GetInitialKineticEnergy();
          } else if ( m_trajectory_type.value() == "rich" ) {
            auto rich_trajectory = dynamic_cast<G4RichTrajectory*>( trajectory );
            if ( rich_trajectory ) init_ke = rich_trajectory->GetInitialKineticEnergy();
          }

          if ( m_min_p.value() && init_p < m_min_p.value() ) continue;
          if ( m_max_p.value() && init_p > m_max_p.value() ) continue;
          if ( m_min_pt.value() && init_pt < m_min_pt.value() ) continue;
          if ( m_max_pt.value() && init_pt > m_max_pt.value() ) continue;
          if ( m_min_ke.value() && init_ke < m_min_ke.value() ) continue;
          if ( m_max_ke.value() && init_ke > m_max_ke.value() ) continue;
          if ( m_min_eta.value() && init_eta < m_min_eta.value() ) continue;
          if ( m_max_eta.value() && init_eta > m_max_eta.value() ) continue;
          if ( !std::any_of( m_accepted_charges.value().begin(), m_accepted_charges.value().end(),
                             [&charge]( double i ) { return i == charge; } ) )
            continue;

          nlohmann::json available_trajectory_properties{
              {"charge", (int)std::round( charge )}, {"mom", init_p}, {"pT", init_pt}, {"id", pid}, {"eta", init_eta}};
          nlohmann::json used_trajectory_properties;
          for ( auto& key : m_trajectory_properties ) {
            auto property = available_trajectory_properties.find( key );
            if ( property != available_trajectory_properties.end() ) { used_trajectory_properties[key] = *property; }
          }
          used_trajectory_properties["color"] = GetTrajectoryColor( trajectory );

          trajJSON.emplace_back( std::pair<G4VTrajectory*, nlohmann::json>( trajectory, used_trajectory_properties ) );
        }

        m_store.storeEventData( {{"gps time", 1},   // TODO: for simulations it does not make sense (maybe rethink?)
                                 {"run number", 1}, // TODO: we cannot get it from G4Event... (additional input variable
                                                    // needed)
                                 {"event number", eventID},
                                 {"Content", {{"Tracks", {{"G4Trajectories", trajJSON}}}}}} );
      }
    }

    std::string GetTrajectoryColor( G4VTrajectory* traj ) const {
      if ( m_trajectory_model.value() == "drawByCharge" ) { return GetTrajectoryColorByCharge( traj ); }
      if ( m_trajectory_model.value() == "drawByParticleID" ) { return GetTrajectoryColorByParticleID( traj ); }
      if ( m_trajectory_model.value() == "drawByMomentum" ) {
        auto traj_p = traj->GetInitialMomentum().mag();
        return GetTrajectoryColorByInterval( traj_p );
      }
      if ( m_trajectory_model.value() == "drawByKineticEnergy" ) {
        double traj_ke = .0;
        if ( m_trajectory_type.value().empty() ) {
          auto normal_trajectory = dynamic_cast<G4Trajectory*>( traj );
          if ( normal_trajectory ) traj_ke = normal_trajectory->GetInitialKineticEnergy();
        } else if ( m_trajectory_type.value() == "smooth" ) {
          auto smooth_trajectory = dynamic_cast<G4SmoothTrajectory*>( traj );
          if ( smooth_trajectory ) traj_ke = smooth_trajectory->GetInitialKineticEnergy();
        } else if ( m_trajectory_type.value() == "rich" ) {
          auto rich_trajectory = dynamic_cast<G4RichTrajectory*>( traj );
          if ( rich_trajectory ) traj_ke = rich_trajectory->GetInitialKineticEnergy();
        }
        return GetTrajectoryColorByInterval( traj_ke );
      }

      error() << "Trajectory Model " << m_trajectory_model.value() << " is not implemented yet." << endmsg;
      return "333333";
    }

    std::string GetTrajectoryColorByCharge( G4VTrajectory* traj ) const {
      double charge = traj->GetCharge();
      if ( charge > 0. ) return m_charge_colors.value().find( "Positive" )->second;
      if ( charge < 0. ) return m_charge_colors.value().find( "Negative" )->second;
      return m_charge_colors.value().find( "Neutral" )->second;
    }

    std::string GetTrajectoryColorByParticleID( G4VTrajectory* traj ) const {
      std::string p_id          = traj->GetParticleName();
      auto        color_pointer = m_particle_id_colors.value().find( p_id );
      if ( color_pointer != m_particle_id_colors.value().end() ) return color_pointer->second;
      return "99AAFF";
    }

    std::string GetTrajectoryColorByInterval( double trajAttValue ) const {
      for ( auto interval : m_interval_colors.value() ) {
        if ( trajAttValue >= interval.second.first && trajAttValue <= interval.second.second ) return interval.first;
      }
      return "99AAFF";
    }
  };
} // namespace GiGa::Phoenix

DECLARE_COMPONENT_WITH_ID( GiGa::Phoenix::DumpG4Trajectories, "GiGaPhoenixDumpG4Trajectories" )
