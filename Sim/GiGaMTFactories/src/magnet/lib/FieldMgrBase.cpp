#include "GiGaMTFactories/Magnet/FieldMgrBase.h"
#include "GaudiKernel/GaudiException.h"
#include "Geant4/G4ChordFinder.hh"
#include "Geant4/G4MagIntegratorStepper.hh"
#include "Geant4/G4Mag_UsualEqRhs.hh"
#include "Geant4/G4PropagatorInField.hh"
#include "Geant4/G4TransportationManager.hh"

namespace Gaussino
{

  StatusCode FieldMgrBase::initialize()
  {
    auto sc = GaudiTool::initialize();
    if ( m_StepperName != "" ) {
      m_stepper = tool<GiGaFactoryBase<G4MagIntegratorStepper, G4Mag_EqRhs*>>( m_StepperName, this );
      if ( !m_stepper ) {
        return Error( "Could not get " + m_StepperName );
      }
    }
    return sc;
  }

  G4MagIntegratorStepper* FieldMgrBase::createStepper() const
  {
    MsgStream log( msgSvc(), name() );

    // get magnetic field
    G4MagneticField* mag = field();

    if ( !mag ) {
      throw GaudiException( "createStepper(): invalid magnetic field!", name(), StatusCode::FAILURE );
    }

    G4Mag_UsualEqRhs* equation = new G4Mag_UsualEqRhs( mag );

    G4MagIntegratorStepper* stepper = m_stepper->construct( equation );

    return stepper;
  }

  G4FieldManager* FieldMgrBase::construct() const
  {
    MsgStream log( msgSvc(), name() );

    G4FieldManager* manager{nullptr};

    if ( m_global ) {
      G4TransportationManager* mgr = G4TransportationManager::GetTransportationManager();
      if ( !mgr ) {
        throw GaudiException( "Invalid Transportation manager", name(), StatusCode::FAILURE );
      }
      manager = mgr->GetFieldManager();
    } else {
      manager = new G4FieldManager();
    }

    if ( !manager ) {
      throw GaudiException( "createFieldMgr(): invalid manager!", name(), StatusCode::FAILURE );
    }

    G4MagneticField* mag = field();

    manager->SetDetectorField( mag );

    if ( !mag ) {
      log << MSG::INFO << "createFieldMgr(): null magnetic field" << endmsg;
      return manager;
    }

    G4MagIntegratorStepper* step = createStepper();
    if ( !step ) {
      throw GaudiException( "createFieldMgr(): invalid stepper", name(), StatusCode::FAILURE );
    }

    G4ChordFinder* chordFinder = new G4ChordFinder( mag, m_minStep, step );
    manager->SetChordFinder( chordFinder );
    manager->SetDeltaIntersection( m_deltaintersection );
    manager->SetDeltaOneStep( m_deltaonestep );
    manager->GetChordFinder()->SetDeltaChord( m_deltaChord );
    manager->SetMinimumEpsilonStep( m_minimumEpsilonStep );
    manager->SetMaximumEpsilonStep( m_maximumEpsilonStep );

    // print the value of epsilon step min and max
    log << MSG::INFO << "Minimum and Maximum Epsilon Step : " << manager->GetMinimumEpsilonStep() << " "
        << manager->GetMaximumEpsilonStep() << endmsg;

    return manager;
  }
}
