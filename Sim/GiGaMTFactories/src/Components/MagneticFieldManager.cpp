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

// Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolHandle.h"

// Geant4
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagneticField.hh"
#include "G4PropagatorInField.hh"
#include "G4TransportationManager.hh"

// Gaussino
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

/** @class MagneticFieldManager
 *
 * Implements the basic functionality to construct and configure
 * the G4FieldManager. A purely virtual function remains which is used
 * to construct the G4MagneticField that can be overloaded.
 *
 *  @author Dominik Muller
 *  @date   2019-06-03
 *  @author Michał Mazurek (revised)
 *  @date   2023-02-10
 *
 */

namespace Gaussino::MagneticField {
  class FieldManagerFactory : public extends<GiGaTool, GiGaFactoryBase<G4FieldManager, bool>> {
  public:
    using extends::extends;

    StatusCode               initialize() override;
    virtual G4MagneticField* field() const { return m_field_factory->construct(); };
    virtual G4FieldManager*  construct( const bool& ) const override;

  private:
    ToolHandle<GiGaFactoryBase<G4MagIntegratorStepper, G4Mag_EqRhs*>> m_stepper_factory{ this, "StepperFactory",
                                                                                         "G4ClassicalRK4" };
    ToolHandle<GiGaFactoryBase<G4MagneticField>> m_field_factory{ this, "FieldFactory", "UniformMagneticField" };

    Gaudi::Property<double> m_minStep{ this, "MinStep", 0.01 * Gaudi::Units::mm };
    Gaudi::Property<double> m_deltaChord{ this, "DeltaChord", 0.25 * Gaudi::Units::mm };
    Gaudi::Property<double> m_deltaintersection{ this, "DeltaIntersection", 1e-05 * Gaudi::Units::mm };
    Gaudi::Property<double> m_deltaonestep{ this, "DeltaOneStep", 0.001 * Gaudi::Units::mm };
    Gaudi::Property<double> m_minimumEpsilonStep{ this, "MinEpsilonStep", 5e-05 * Gaudi::Units::mm };
    Gaudi::Property<double> m_maximumEpsilonStep{ this, "MaxEpsilonStep", 0.001 * Gaudi::Units::mm };
  };
} // namespace Gaussino::MagneticField

StatusCode Gaussino::MagneticField::FieldManagerFactory::initialize() {
  auto sc = extends::initialize();
  sc &= m_stepper_factory.retrieve();
  return sc;
}

G4FieldManager* Gaussino::MagneticField::FieldManagerFactory::construct( const bool& global ) const {
  G4FieldManager* manager{ nullptr };
  if ( global ) {
    auto mgr = G4TransportationManager::GetTransportationManager();
    if ( !mgr ) { throw GaudiException( "Invalid Transportation manager", name(), StatusCode::FAILURE ); }
    manager = mgr->GetFieldManager();
  } else {
    manager = new G4FieldManager();
  }
  if ( !manager ) { throw GaudiException( "createFieldMgr(): invalid manager!", name(), StatusCode::FAILURE ); }

  // construct the G4MagneticField
  auto mag = field();
  if ( !mag ) {
    // this might be the case for ZeroMagneticField
    info() << "createFieldMgr(): null magnetic field!" << endmsg;
  }
  manager->SetDetectorField( mag );

  // construct the stepper
  auto equation = new G4Mag_UsualEqRhs( mag );
  auto step     = m_stepper_factory->construct( equation );
  if ( !step ) { throw GaudiException( "createFieldMgr(): invalid stepper", name(), StatusCode::FAILURE ); }

  // construct the G4ChordFinder
  auto chordFinder = new G4ChordFinder( mag, m_minStep, step );
  manager->SetChordFinder( chordFinder );
  manager->SetDeltaIntersection( m_deltaintersection );
  manager->SetDeltaOneStep( m_deltaonestep );
  manager->GetChordFinder()->SetDeltaChord( m_deltaChord );
  manager->SetMinimumEpsilonStep( m_minimumEpsilonStep );
  manager->SetMaximumEpsilonStep( m_maximumEpsilonStep );

  // print the value of epsilon step min and max
  debug() << "Minimum and Maximum Epsilon Step : " << manager->GetMinimumEpsilonStep() << " "
          << manager->GetMaximumEpsilonStep() << endmsg;

  return manager;
}

DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::FieldManagerFactory, "MagneticFieldManager" )
