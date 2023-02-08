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
#include "GaudiKernel/SystemOfUnits.h"

// G4
#include "G4ModelCommandsT.hh"
#include "G4SmartFilter.hh"
#include "G4UnitsTable.hh"
#include "G4VFilter.hh"
#include "G4VModelFactory.hh"
#include "G4VTrajectory.hh"

// Gaussino
#include "GiGaMTCoreMessage/IGiGaMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

namespace GiGa::TrajectoryInitialPT {
  class Filter : public G4SmartFilter<G4VTrajectory> {
    double m_min_pt = 0.;
    double m_max_pt = 0.;

  public:
    Filter( const std::string& name = "Unspecified", double minMom = 0., double maxMom = 0. );
    inline virtual bool Evaluate( const G4VTrajectory& traj ) const override;
    inline virtual void Print( std::ostream& ) const override;
    inline virtual void Clear() override{};
  };

  using G4TrajFilter        = G4VFilter<G4VTrajectory>;
  using G4TrajFilterFactory = G4VModelFactory<G4TrajFilter>;

  class FilterG4Factory : public G4TrajFilterFactory, public GiGaMessage {
    double m_min_pt = 0.;
    double m_max_pt = 0.;

  public:
    using Messengers         = std::vector<G4UImessenger*>;
    using ModelAndMessengers = std::pair<G4TrajFilter*, Messengers>;
    FilterG4Factory() : G4TrajFilterFactory( "initialPTFilter" ){};
    ModelAndMessengers Create( const G4String&, const G4String& ) override;
    inline void        setMinPt( double min_pt ) { m_min_pt = min_pt; };
    inline void        setMaxPt( double max_pt ) { m_max_pt = max_pt; };
  };

  class FilterFactory : public extends<GiGaTool, GiGaFactoryBase<G4TrajFilterFactory>> {
    Gaudi::Property<double> m_min_pt{this, "MinPT", 0 * Gaudi::Units::MeV};
    Gaudi::Property<double> m_max_pt{this, "MaxPT", 0 * Gaudi::Units::MeV};

  public:
    using extends::extends;
    StatusCode                   initialize() override;
    virtual G4TrajFilterFactory* construct() const override;
  };
} // namespace GiGa::TrajectoryInitialPT

DECLARE_COMPONENT_WITH_ID( GiGa::TrajectoryInitialPT::FilterFactory, "GiGaTrajectoryInitialPTFilter" )

GiGa::TrajectoryInitialPT::Filter::Filter( const std::string& name, double min_pt, double max_pt )
    : G4SmartFilter<G4VTrajectory>( name ), m_min_pt( min_pt ), m_max_pt( max_pt ){};

bool GiGa::TrajectoryInitialPT::Filter::Evaluate( const G4VTrajectory& traj ) const {
  double init_pt = traj.GetInitialMomentum().perp();
  if ( m_min_pt && init_pt < m_min_pt ) return false;
  if ( m_max_pt && init_pt > m_max_pt ) return false;
  if ( GetVerbose() ) { G4cout << "Accepted: " << traj.GetParticleName() << ", pT: " << init_pt << " MeV" << G4endl; }
  return true;
}

void GiGa::TrajectoryInitialPT::Filter::Print( std::ostream& ostr ) const {
  if ( GetVerbose() ) {
    ostr << "Initial momentum filter activated: " << G4endl;
    ostr << " -> min momentum: ";
    if ( m_min_pt ) {
      ostr << G4BestUnit( m_min_pt, "Energy" ) << G4endl;
    } else {
      ostr << "NO FILTER" << G4endl;
    }
    ostr << " -> max momentum: ";
    if ( m_max_pt ) {
      ostr << G4BestUnit( m_max_pt, "Energy" ) << G4endl;
    } else {
      ostr << "NO FILTER" << G4endl;
    }
  }
}

GiGa::TrajectoryInitialPT::FilterG4Factory::ModelAndMessengers
GiGa::TrajectoryInitialPT::FilterG4Factory::Create( const G4String& placement, const G4String& name ) {
  using GiGa::TrajectoryInitialPT::Filter;
  auto model = new Filter{name, m_min_pt, m_max_pt};
  if ( MessageInterfacelevel() < 1 ) { model->SetVerbose( true ); }
  Messengers messengers = {};
  // TODO: here we do not allow setting of the properties through G4 UI,
  // but only through Gaudi properties
  // messengers.push_back(new G4ModelCmdAddString<Filter>(model, placement));
  messengers.push_back( new G4ModelCmdInvert<Filter>( model, placement ) );
  messengers.push_back( new G4ModelCmdActive<Filter>( model, placement ) );
  messengers.push_back( new G4ModelCmdVerbose<Filter>( model, placement ) );
  messengers.push_back( new G4ModelCmdReset<Filter>( model, placement ) );
  return ModelAndMessengers( model, messengers );
}

StatusCode GiGa::TrajectoryInitialPT::FilterFactory::initialize() {
  return extends::initialize().andThen( [&]() -> StatusCode {
    if ( !m_min_pt.value() && !m_max_pt.value() ) {
      error() << "Requested to add a GiGaTrajectoryInitialPTFilter, "
              << "but either min or max initial momentum property must be > 0." << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  } );
}

GiGa::TrajectoryInitialPT::G4TrajFilterFactory* GiGa::TrajectoryInitialPT::FilterFactory::construct() const {
  debug() << "Creating a new G4 vis trajectory filter: GiGaTrajectoryInitialPTFilter" << endmsg;
  auto filter = new GiGa::TrajectoryInitialPT::FilterG4Factory{};
  filter->SetMessageInterface( message_interface() );
  if ( m_min_pt.value() ) {
    debug() << "Adding a minimum value for the initial transverse momentum: " << m_min_pt.value() * Gaudi::Units::MeV
            << " MeV" << endmsg;
    filter->setMinPt( m_min_pt.value() );
  }
  if ( m_max_pt.value() ) {
    debug() << "Adding a maximum value for the initial transverse momentum: " << m_max_pt.value() * Gaudi::Units::MeV
            << " MeV" << endmsg;
    filter->setMaxPt( m_max_pt.value() );
  }
  return filter;
}
