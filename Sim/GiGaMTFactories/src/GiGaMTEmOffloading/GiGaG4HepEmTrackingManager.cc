/*****************************************************************************\
* (c) Copyright 2025 CERN for the benefit of the LHCb Collaboration           *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// SPDX-FileCopyrightText: 2024 CERN
// SPDX-License-Identifier: Apache-2.0

#include "GiGaG4HepEmTrackingManager.hh"

#include "G4Electron.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Gamma.hh"
#include "G4Positron.hh"
#include "G4VSensitiveDetector.hh"

#include "Gaudi/Property.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"

GiGaG4HepEmTrackingManager::GiGaG4HepEmTrackingManager( G4int verbose ) : G4HepEmTrackingManager( verbose ) {

  // Use the Gaudi service to read in the actual cut values into the member variables

  // 1) get ToolSvc
  SmartIF<IToolSvc> toolSvc = Gaudi::svcLocator()->service<IToolSvc>( "ToolSvc" );
  if ( !toolSvc ) {
    throw GaudiException( "Failed to get ToolSvc", "GiGaG4HepEmTrackingManager", StatusCode::FAILURE );
  }

  // 2) get the tool instance by name
  IAlgTool* tool = nullptr;
  toolSvc->retrieveTool( "TrCutsRunAction", tool ).ignore();
  if ( !tool ) {
    throw GaudiException( "Failed to get TrCutsRunAction tool", "GiGaG4HepEmTrackingManager", StatusCode::FAILURE );
  }

  // 3) view it as IProperty
  SmartIF<IProperty> iprop( tool );

  auto getDoubleProp = [&]( const std::string& name, double& target ) {
    const auto& base = iprop->getProperty( name );
    if ( auto* typed = dynamic_cast<const Gaudi::Property<double>*>( &base ) ) {
      target = typed->value();
    } else {
      target = std::stod( base.toString() );
    }
  };

  // 4) write cut values into members
  getDoubleProp( "ElectronTrCut", fElectronTrackingCut );
  getDoubleProp( "GammaTrCut", fGammaTrackingCut );
  getDoubleProp( "MinX", fMinX );
  getDoubleProp( "MinY", fMinY );
  getDoubleProp( "MinZ", fMinZ );
  getDoubleProp( "MaxX", fMaxX );
  getDoubleProp( "MaxY", fMaxY );
  getDoubleProp( "MaxZ", fMaxZ );

  // 5) release tool
  toolSvc->releaseTool( tool ).ignore();
}

GiGaG4HepEmTrackingManager::~GiGaG4HepEmTrackingManager() {}

/// @brief The function checks within the TrackElectron and TrackGamma calls in G4HepEmTracking manager, if a barrier is
/// hit.
/// Here it is used to implement the custom cuts used in Gauss, namely the WorldCut and the minEkineCut from the
/// SpecialCuts
bool GiGaG4HepEmTrackingManager::CheckEarlyTrackingExit( G4Track* track, G4EventManager* evtMgr,
                                                         G4UserTrackingAction* userTrackingAction,
                                                         G4TrackVector&        secondaries ) const {

  // check for world Cuts and tracking Cut.

  G4ThreeVector pos  = track->GetPosition();
  double        eKin = track->GetKineticEnergy();

  double energyTrackingCut =
      track->GetParticleDefinition() == G4Gamma::Definition() ? fGammaTrackingCut : fElectronTrackingCut;

  if ( pos.x() > fMaxX || pos.x() < fMinX || pos.y() > fMaxY || pos.y() < fMinY || pos.z() > fMaxZ || pos.z() < fMinZ ||
       eKin < energyTrackingCut ) {

    // particle has left the world (for cuts) or is below tracking energy. Call PostUserTrackingAction and kill it.

    // NOTE: When killing positrons, they would undergo annihilation at rest, therefore one must deposit 2 *
    // electron_mass_c2. However, as Gauss' way of cutting particles in SpecialCuts omits this incorrectly, we omit it
    // here too, to get agreement with Gauss. This is not correct! The line below should be used for correctness

    // if (track->GetParticleDefinition() ==  G4Positron::Definition()) eKin += 2 * CLHEP::electron_mass_c2;

    auto* userSteppingAction = evtMgr->GetUserSteppingAction();
    auto* lvol               = track->GetTouchable()->GetVolume()->GetLogicalVolume();

    // get mutable step
    auto step = const_cast<G4Step*>( track->GetStep() );

    step->SetTotalEnergyDeposit( eKin );

    // End of this step: Call sensitive detector and stepping actions.
    if ( step->GetControlFlag() != AvoidHitInvocation ) {
      auto* sensitive = lvol->GetSensitiveDetector();
      if ( sensitive ) { sensitive->Hit( step ); }
    }

    if ( userSteppingAction ) { userSteppingAction->UserSteppingAction( step ); }

    // call PostUserTrackingAction
    if ( userTrackingAction ) { userTrackingAction->PostUserTrackingAction( track ); }

    // // Delete the trajectory object (if the user set any)
    G4VTrajectory* theTrajectory = evtMgr->GetTrackingManager()->GetStoreTrajectory() == 0
                                       ? nullptr
                                       : evtMgr->GetTrackingManager()->GimmeTrajectory();
    if ( theTrajectory != nullptr ) { delete theTrajectory; }

    // Push secondaries
    evtMgr->StackTracks( &secondaries );

    return true;
  } else {
    // Continue tracking with G4HepEmTrackingManager
    return false;
  }
}

void GiGaG4HepEmTrackingManager::HandOverOneTrack( G4Track* aTrack ) {

  // As Gauss resets the G4RandomEngine pointer for each event instead of re-seeding it,
  // the G4HepEm random engine must be rebound to the new G4RandomEngine
  auto eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  if ( eventID != fCurrentEventId ) {
    RebindG4RandomEngine();
    fCurrentEventId = eventID;
  }

  // Rest: just as the base class in G4HepEmTrackingManager::HandOverOneTrack

  const G4ParticleDefinition* part = aTrack->GetParticleDefinition();

  if ( part == G4Electron::Definition() || part == G4Positron::Definition() ) {
    TrackElectron( aTrack );
  } else if ( part == G4Gamma::Definition() ) {
    TrackGamma( aTrack );
  }

  aTrack->SetTrackStatus( fStopAndKill );
  delete aTrack;
}
