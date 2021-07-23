/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#include "GiGaMTFactories/Magnet/FieldMgrBase.h"
#include "GaudiKernel/GaudiException.h"
#include "Geant4/G4ChordFinder.hh"
#include "Geant4/G4MagIntegratorStepper.hh"
#include "Geant4/G4Mag_UsualEqRhs.hh"
#include "Geant4/G4PropagatorInField.hh"
#include "Geant4/G4TransportationManager.hh"

namespace Gaussino
{

  G4FieldManager* FieldMgrBase::construct(const bool & global) const
  {
    MsgStream log( msgSvc(), name() );


    G4FieldManager* manager{nullptr};

    if ( global ) {
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

    // get magnetic field
    G4Mag_UsualEqRhs* equation = new G4Mag_UsualEqRhs( mag );

    if ( !mag ) {
      log << MSG::INFO << "createFieldMgr(): null magnetic field" << endmsg;
      return manager;
    }

    G4MagIntegratorStepper* step = m_stepper->construct( equation );
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
