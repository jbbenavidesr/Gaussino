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

// Geant4
#include "G4TransportationManager.hh"
#include "G4VFastSimSensitiveDetector.hh"
#include "G4VSensitiveDetector.hh"

// Gaussino
#include "CustomSimulation/HitMaker.h"

// FIXME: this is just a dumb copy of G4FastSimHitMaker in order to introduce
// a workaround

Gaussino::CustomSimulation::HitMaker::HitMaker() {
  fTouchableHandle = new G4TouchableHistory();
  fpNavigator      = new G4Navigator();
  fNaviSetup       = false;
  fWorldWithSdName = "";
}

Gaussino::CustomSimulation::HitMaker::~HitMaker() { delete fpNavigator; }

void Gaussino::CustomSimulation::HitMaker::make( const G4FastHit& aHit, const G4FastTrack& aTrack ) {
  // do not make empty deposit
  if ( aHit.GetEnergy() <= 0 ) return;
  // Locate the spot
  if ( !fNaviSetup ) {
    // Choose the world volume that contains the sensitive detector based on its
    // name (empty name for mass geometry)
    G4VPhysicalVolume* worldWithSD = nullptr;
    if ( fWorldWithSdName.empty() ) {
      worldWithSD = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();
    } else {
      worldWithSD = G4TransportationManager::GetTransportationManager()->GetParallelWorld( fWorldWithSdName );
    }
    fpNavigator->SetWorldVolume( worldWithSD );
    // use track global position
    fpNavigator->LocateGlobalPointAndUpdateTouchable( aTrack.GetPrimaryTrack()->GetPosition(), fTouchableHandle(),
                                                      false );
    fNaviSetup = true;
  } else {
    // for further deposits use hit (local) position and local->global
    // transformation
    fpNavigator->LocateGlobalPointAndUpdateTouchable(
        aTrack.GetInverseAffineTransformation()->TransformPoint( aHit.GetPosition() ), fTouchableHandle() );
  }
  G4VPhysicalVolume* currentVolume = fTouchableHandle()->GetVolume();

  G4VSensitiveDetector* sensitive;
  if ( currentVolume != 0 ) {
    // FIXME: here's the workaround
    sensitive = aTrack.GetEnvelopeLogicalVolume()->GetSensitiveDetector();
    // sensitive = currentVolume->GetLogicalVolume()->GetSensitiveDetector();
    G4VFastSimSensitiveDetector* fastSimSensitive = dynamic_cast<G4VFastSimSensitiveDetector*>( sensitive );
    if ( fastSimSensitive ) {
      fastSimSensitive->Hit( &aHit, &aTrack, &fTouchableHandle );
    }

    else if ( sensitive && currentVolume->GetLogicalVolume()->GetFastSimulationManager() ) {
      G4cerr << "ERROR - G4FastSimHitMaker::make()" << G4endl << "        It is required to derive from the " << G4endl
             << "        G4VFastSimSensitiveDetector in " << G4endl
             << "        addition to the usual G4VSensitiveDetector class." << G4endl;
      G4Exception( "G4FastSimHitMaker::make()", "InvalidSetup", FatalException,
                   "G4VFastSimSensitiveDetector interface not implemented." );
    }
  }
}
