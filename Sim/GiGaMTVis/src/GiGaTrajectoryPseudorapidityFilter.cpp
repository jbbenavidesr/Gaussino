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

#include <cmath>
// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// G4
#include "G4ModelCommandsT.hh"
#include "G4SmartFilter.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"
#include "G4VFilter.hh"
#include "G4VModelFactory.hh"
#include "G4VTrajectory.hh"

// Gaussino
#include "GiGaMTCoreMessage/IMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

namespace GiGa::TrajectoryInitialEta {
  class Filter : public G4SmartFilter<G4VTrajectory> {
    double m_min_eta = 0.;
    double m_max_eta = 0.;

  public:
    Filter( const std::string& name = "Unspecified", double min_eta = 0., double max_eta = 0. );
    inline virtual bool Evaluate( const G4VTrajectory& traj ) const;
    inline virtual void Print( std::ostream& ) const;
    inline virtual void Clear(){};
  };

  using G4TrajFilter        = G4VFilter<G4VTrajectory>;
  using G4TrajFilterFactory = G4VModelFactory<G4TrajFilter>;

  class FilterG4Factory : public G4TrajFilterFactory, public Gsino::Message {
    double m_min_eta = 0.;
    double m_max_eta = 0.;

  public:
    using Messengers         = std::vector<G4UImessenger*>;
    using ModelAndMessengers = std::pair<G4TrajFilter*, Messengers>;
    FilterG4Factory() : G4TrajFilterFactory( "initialEtaFilter" ){};
    ModelAndMessengers Create( const G4String&, const G4String& );
    inline void        setMinEta( double min_eta ) { m_min_eta = min_eta; };
    inline void        setMaxEta( double max_eta ) { m_max_eta = max_eta; };
  };

  class FilterFactory : public extends<GiGaTool, GiGaFactoryBase<G4TrajFilterFactory>> {
    Gaudi::Property<double> m_min_eta{ this, "MinEta", 0. };
    Gaudi::Property<double> m_max_eta{ this, "MaxEta", 0. };

  public:
    using extends::extends;
    StatusCode                   initialize() override;
    virtual G4TrajFilterFactory* construct() const override;
  };
} // namespace GiGa::TrajectoryInitialEta

DECLARE_COMPONENT_WITH_ID( GiGa::TrajectoryInitialEta::FilterFactory, "GiGaTrajectoryInitialEtaFilter" )

GiGa::TrajectoryInitialEta::Filter::Filter( const std::string& name, double min_eta, double max_eta )
    : G4SmartFilter<G4VTrajectory>( name ), m_min_eta( min_eta ), m_max_eta( max_eta ){};

bool GiGa::TrajectoryInitialEta::Filter::Evaluate( const G4VTrajectory& traj ) const {
  double init_eta = traj.GetInitialMomentum().getEta();

  if ( m_min_eta && init_eta < m_min_eta ) return false;
  if ( m_max_eta && init_eta > m_max_eta ) return false;
  if ( GetVerbose() ) { G4cout << "Accepted: " << traj.GetParticleName() << ", Eta: " << init_eta << G4endl; }
  return true;
}

void GiGa::TrajectoryInitialEta::Filter::Print( std::ostream& ostr ) const {
  if ( GetVerbose() ) {
    ostr << "Initial pseudorapidity filter activated: " << G4endl;
    ostr << " -> min pseudorapidity: ";
    if ( m_min_eta ) {
      ostr << m_min_eta << G4endl;
    } else {
      ostr << "NO FILTER" << G4endl;
    }
    ostr << " -> max pseudorapidity: ";
    if ( m_max_eta ) {
      ostr << m_max_eta << G4endl;
    } else {
      ostr << "NO FILTER" << G4endl;
    }
  }
}

GiGa::TrajectoryInitialEta::FilterG4Factory::ModelAndMessengers
GiGa::TrajectoryInitialEta::FilterG4Factory::Create( const G4String& placement, const G4String& name ) {
  using GiGa::TrajectoryInitialEta::Filter;
  auto model = new Filter{ name, m_min_eta, m_max_eta };
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

StatusCode GiGa::TrajectoryInitialEta::FilterFactory::initialize() {
  return extends::initialize().andThen( [&]() -> StatusCode {
    if ( !m_min_eta.value() && !m_max_eta.value() ) {
      error() << "Requested to add a GiGaTrajectoryInitialEtaFilter, "
              << "but either min or max initial pseudorapidity property must be > 0." << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  } );
}

GiGa::TrajectoryInitialEta::G4TrajFilterFactory* GiGa::TrajectoryInitialEta::FilterFactory::construct() const {
  debug() << "Creating a new G4 vis trajectory filter: GiGaTrajectoryInitialEtaFilter" << endmsg;
  auto filter = new GiGa::TrajectoryInitialEta::FilterG4Factory{};
  filter->SetMessageInterface( message_interface() );
  if ( m_min_eta.value() ) {
    debug() << "Adding a minimum value for the initial pseudorapidity: " << m_min_eta.value() << endmsg;
    filter->setMinEta( m_min_eta.value() );
  }
  if ( m_max_eta.value() ) {
    debug() << "Adding a maximum value for the initial pseudorapidity: " << m_max_eta.value() << endmsg;
    filter->setMaxEta( m_max_eta.value() );
  }
  return filter;
}
