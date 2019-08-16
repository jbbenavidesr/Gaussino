/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
/// Geant4
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4PrimaryParticle.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4TrackVector.hh"
#include "Geant4/G4TrackingManager.hh"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// GiGa
//#include "GiGaCnv/GiGaPrimaryParticleInformation.h"
// GaussTools
//#include "GaussTools/GaussTrajectory.h"
#include "GiGaMTCore/Truth/GaussinoTrackInformation.h"
//#include "GiGaMTCore/Truth/GaussinoPrimaryParticleInformation.h"
/// local
#include "GiGaMTCore/Truth/TruthFlaggingTrackAction.h"

// ============================================================================
/** initialize the track action
 *  @see GiGaTrackActionBase
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
void TruthFlaggingTrackAction::Setup()
{
  //
  if ( storeByOwnType ) {
    ownStoredTypes.clear();
    G4ParticleTable* table = G4ParticleTable::GetParticleTable();
    if ( 0 == table ) {
      G4cerr << __PRETTY_FUNCTION__ << "G4ParticleTable* points to NULL!" << G4endl;
      return;
    }
    for ( auto name : ownStoredTypesNames ) {
      const G4ParticleDefinition* pd = table->FindParticle( name );
      if ( 0 == pd ) {
        G4cerr << __PRETTY_FUNCTION__ << std::string( "Could not find G4ParticleDefinition" ) + "for particle name='" + name + "'!"
               << G4endl;
      }
      ownStoredTypes.insert( pd );
    }
    //
    if ( ownStoredTypes.empty() ) {
      G4cerr << __PRETTY_FUNCTION__ << std::string( "OwnTypesContainer is empty! Deactivate the " ) + "'StoreByOwnType' option!"
             << G4endl;
      storeByOwnType = false; ///< NB !!!
    }
  }
  //
  if ( storeByChildType ) {
    childStoredTypes.clear();
    G4ParticleTable* table = G4ParticleTable::GetParticleTable();
    if ( 0 == table ) {
      G4cerr << __PRETTY_FUNCTION__ << "G4ParticleTable* points to NULL!" << G4endl;
      return;
    }
    for ( auto name : childStoredTypesNames ) {
      const G4ParticleDefinition* pd = table->FindParticle( name );
      if ( 0 == pd ) {
        G4cerr << std::string( "Could not find G4ParticleDefinition " ) + "for particle name='" + name + "'!" << G4endl;
      }
      childStoredTypes.insert( pd );
    }
    //
    if ( childStoredTypes.empty() ) {
      G4cerr << std::string( "ChildTypesContainer" ) + " is empty! Deactivate the 'StoreByChildType' option!" << G4endl;
      storeByChildType = false; ///< NB !!!
    }
  }
  ///
  return verbose( "Setup successfully" );
}

void TruthFlaggingTrackAction::PreUserTrackingAction( const G4Track* track )
{
  std::call_once( run_setup_flag, [this]() { this->Setup(); } );
  auto trackMgr = G4UserTrackingAction::fpTrackingManager;
  // Is the track valid? Is tracking manager valid?
  if ( !track ) {
    G4cerr << __PRETTY_FUNCTION__ << "Failed to cast existing UserTrackInformation. Overwriting existing one!" << G4endl;
    return;
  }
  if ( !trackMgr ) {
    G4cerr << __PRETTY_FUNCTION__ << "Failed to cast existing UserTrackInformation. Overwriting existing one!" << G4endl;
    return;
  }

  // Get the GaussinoTrackInformation object. Will create and attach it if not
  // already done. Uses the Eventmanager -> trackmanager -> track to attach
  // the information as this gives a non-const track
  auto ti = GaussinoTrackInformation::Get();

  if ( storeByOwnEnergy && ( track->GetKineticEnergy() > ownEnergyThreshold ) ) {
    if ( storeUpToZmax && ( track->GetVertexPosition().z() > zMaxToStore ) ) {
      return;
    }
    #ifdef TRUTHDEBUG
    ti->SetStoreReason("storeByOwnEnergy");
    #endif
    ti->setToStoreTruth( true );
  }
}

void TruthFlaggingTrackAction::PostUserTrackingAction( const G4Track* track )
{
  std::call_once( run_setup_flag, [this]() { this->Setup(); } );
  auto trackMgr = G4UserTrackingAction::fpTrackingManager;
  // Is the track valid? Is tracking manager valid?
  if ( !track ) {
    G4cerr << __PRETTY_FUNCTION__ << "Failed to cast existing UserTrackInformation. Overwriting existing one!" << G4endl;
    return;
  }
  if ( !trackMgr ) {
    G4cerr << __PRETTY_FUNCTION__ << "Failed to cast existing UserTrackInformation. Overwriting existing one!" << G4endl;
    return;
  }

  auto this_track_info = GaussinoTrackInformation::Get();

  // if only to a certain z, check z and set flag
  bool zstore = true;
  if ( storeUpToZmax && ( track->GetVertexPosition().z() > zMaxToStore ) ) {
    zstore = false;
  }

  if ( rejectRICHphe ) {
    const G4VProcess* process = track->GetCreatorProcess();
    if ( 0 != process ) {
      if ( ( "RichHpdPhotoelectricProcess" == process->GetProcessName() ) ||
           ( "RichHpdSiEnergyLossProcess" == process->GetProcessName() ) ||
           ( "RichPmtPhotoelectricProcess" == process->GetProcessName() ) ||
           ( "RichPmtSiEnergyLossProcess" == process->GetProcessName() ) ||
           ( "TorchTBMcpPhotoElectricProcess" == process->GetProcessName() ) ||
           ( "TorchTBMcpEnergyLossProcess" == process->GetProcessName() ) ) {
        warning( "RichHpd/Pmt/Mcp PhotoelectricProcess  RichHpd/Pmt/Mcp SiEnergyLossProcess particles not kept" );
        return;
      }
    }
  }

  if ( rejectOptPhot ) {
    if ( track->GetDefinition()->GetParticleName() == "opticalphoton" ) {
      return;
    }
  }

  // if reject rich photoelectrons check and store
  // (3) store  all     particles ?
  if ( storeAll && zstore ) {
    // FIXME: trackMgr->SetStoreTrajectory( true );
    this_track_info->setToStoreTruth( true ); // flag for storing in HepMC (Witek)
    #ifdef TRUTHDEBUG
    this_track_info->SetStoreReason("storeAll");
    #endif
    // FIXME: setProcess( track );
    // FIXME: fillGaussTrackInformation( track );
    return;
  } /// RETURN !!!

  // (3.5) store forced-decay products
  if ( storeDecayProducts && track->GetDynamicParticle()->GetPreAssignedDecayProducts() ) {
    if ( 0 != trackMgr->GimmeSecondaries() ) {
      G4TrackVector* childrens = trackMgr->GimmeSecondaries();

      for ( unsigned int index = 0; index < childrens->size(); ++index ) {
        G4Track* dtr = ( *childrens )[index];
        if ( !dtr ) {
          continue;
        }
        if ( !( dtr->GetDynamicParticle()->GetPreAssignedDecayProducts() ) ) {
          auto child_track_info = GaussinoTrackInformation::Get( dtr );
          child_track_info->setToStoreTruth( true );
          #ifdef TRUTHDEBUG
          child_track_info->SetStoreReason("isForcedDecayProduct");
          #endif
        }
      }
      // FIXME: trackMgr->SetStoreTrajectory( true );
      this_track_info->setToStoreTruth( true ); // flag for storing in HepMC (Witek)
      #ifdef TRUTHDEBUG
      this_track_info->SetStoreReason("hasForcedDecayProducts");
      #endif

      // FIXME: setProcess( track );
      // FIXME: fillGaussTrackInformation( track );
      return;
    }
  }

  // (4) store  primary particles ?
  if ( storePrimaries && 0 == track->GetParentID() ) {
    // FIXME: setProcess( track );
    // FIXME: trackMgr->SetStoreTrajectory( true );
    this_track_info->setToStoreTruth( true ); // flag for storing in HepMC (Witek)
    #ifdef TRUTHDEBUG
    this_track_info->SetStoreReason("isPrimary");
    #endif

    // FIXME: fillGaussTrackInformation( track );
    return;
  } /// RETURN !!!

  // (5) store particles with kinetic energy over the threshold value.
  //     See also PreAction
  if ( storeByOwnEnergy && ( track->GetKineticEnergy() > ownEnergyThreshold ) && zstore ) {
    // FIXME: setProcess( track );
    // FIXME: fillGaussTrackInformation( track );
    // FIXME: trackMgr()->SetStoreTrajectory( true );
    this_track_info->setToStoreTruth( true ); // flag for storing in HepMC (Witek)
    #ifdef TRUTHDEBUG
    this_track_info->SetStoreReason("storeByOwnEnergy");
    #endif

    return;
  } /// RETURN !!!

  // (6) store all predefined particle types:
  if ( storeByOwnType && ( ownStoredTypes.count( track->GetDefinition() ) > 0 ) && zstore ) {
    // FIXME: setProcess( track );
    // FIXME: fillGaussTrackInformation( track );
    // FIXME: trackMgr()->SetStoreTrajectory( true );
    this_track_info->setToStoreTruth( true ); // flag for storing in HepMC (Witek)
    #ifdef TRUTHDEBUG
    this_track_info->SetStoreReason("storeByOwnType");
    #endif
    return;
  } /// RETURN !!!

  // (7) store the particle if it has a certain type of daughter particle
  //     or at least one from secondaries  particle have kinetic energy over
  //     threshold
  if ( storeByChildType || ( storeByChildEnergy && 0 != trackMgr->GimmeSecondaries() && zstore ) ) {
    const G4TrackVector* childrens = trackMgr->GimmeSecondaries();
    for ( unsigned int index = 0; index < childrens->size(); ++index ) {
      const G4Track* tr = ( *childrens )[index];
      if ( 0 == tr ) {
        continue;
      }
      //
      if ( storeByChildEnergy && ( tr->GetKineticEnergy() > childEnergyThreshold ) ) {
        // FIXME: setProcess( track );
        // FIXME: fillGaussTrackInformation( track );
        // FIXME: trackMgr()->SetStoreTrajectory( true );
        this_track_info->setToStoreTruth( true ); // flag for storing in HepMC (Witek)
        #ifdef TRUTHDEBUG
        this_track_info->SetStoreReason("storeByChildEnergy");
        #endif
        return;
      } /// RETURN
        //
      if ( storeByChildType && ( childStoredTypes.count( tr->GetDefinition() ) > 0 ) && zstore ) {
        // FIXME: setProcess( track );
        // FIXME: fillGaussTrackInformation( track );
        // FIXME: trackMgr()->SetStoreTrajectory( true );
        this_track_info->setToStoreTruth( true ); // flag for storing in HepMC (Witek)
        #ifdef TRUTHDEBUG
        this_track_info->SetStoreReason("storeByChildType");
        #endif
        return;
      }
    }
  }

  // (7.5) store tracks according to creator process of its daughters
  if ( storeBySecondariesProcess && 0 != trackMgr->GimmeSecondaries() && zstore ) {
    const G4TrackVector* childrens = trackMgr->GimmeSecondaries();
    for ( unsigned int index = 0; index < childrens->size(); ++index ) {
      const G4Track* tr = ( *childrens )[index];
      if ( 0 == tr ) {
        continue;
      }
      //
      if ( childStoredProcess.count( tr->GetCreatorProcess()->GetProcessName() ) > 0 ) {
        // FIXME: setProcess( track );
        // FIXME: fillGaussTrackInformation( track );
        // FIXME: trackMgr()->SetStoreTrajectory( true );
        this_track_info->setToStoreTruth( true ); // flag for storing in HepMC (Witek)
        #ifdef TRUTHDEBUG
        this_track_info->SetStoreReason("storeBySecondariesProcess");
        #endif
        return;
      }
    }
  }

  // (7.6) store tracks according to creator process
  if ( storeByOwnProcess && zstore ) {
    if ( ownStoredProcess.count( track->GetCreatorProcess()->GetProcessName() ) > 0 ) {
      // FIXME: setProcess( track );
      // FIXME: fillGaussTrackInformation( track );
      // FIXME: trackMgr()->SetStoreTrajectory( true );
      this_track_info->setToStoreTruth( true ); // flag for storing in HepMC (Witek)
      #ifdef TRUTHDEBUG
      this_track_info->SetStoreReason("storeByOwnProcess");
      #endif
      return;
    }
  }

  // check if track is to be stored ???????????????????????????????????????
  // FIXME: This isn't doing anything right now as we right now do not support
  // saving of the trajectories.
  if ( trackMgr->GetStoreTrajectory() ) {
    this_track_info->setToStoreTruth( true );
    return;
  }

  // if track is not to be stored, propagate it's parent ID (stored) to its
  // secondaries
  if ( !( this_track_info->storeTruth() ) ) {
    if ( trackMgr->GimmeSecondaries() ) {
      if ( 0 == track->GetParentID() ) {
        G4cerr << __PRETTY_FUNCTION__ << " Dangerous: Primary Particle is not requested to be stored" << G4endl;
      }

      G4TrackVector* childrens = trackMgr->GimmeSecondaries();
      for ( unsigned int index = 0; index < childrens->size(); ++index ) {
        G4Track* child_track = ( *childrens )[index];
        if ( !child_track ) {
          continue;
        }

        if ( child_track->GetParentID() != track->GetTrackID() ) {
          G4cerr << __PRETTY_FUNCTION__ << " Child ID is not equal to track ID" << G4endl;
        }
        //
        child_track->SetParentID( track->GetParentID() );
        // set the flag saying that the direct mother is not stored
        auto child_track_info = GaussinoTrackInformation::Get( child_track );
        // FIXME: Is this really necessary? We shall see ...
        child_track_info->setDirectParent( false );
      }
    }
    // also update the trackID in the hits. This loops over all hits associated
    // with the this track and changes the ID of the track that caused them
    // to the parent. In most sensitive detector implementations, the track that
    // created a hit will be flagged to be stored so this part here rarely triggers
    this_track_info->updateHitsTrackID( track->GetParentID() );
  }

  // delete the trajectory by hand
  // TODO: Not sure what this is supposed to do
  //
  // if ( !trackMgr->GetStoreTrajectory() ) {
  // G4VTrajectory* traj = trackMgr()->GimmeTrajectory();
  // if ( 0 != traj ) {
  // delete traj;
  //}
  // trackMgr()->SetTrajectory( 0 );
  //}
}

// ============================================================================
// Fill Track User information from Primary Info from Generator part
// (oscillation, signal barcode, HepMC Event link, etc...
// ============================================================================
// void TruthFlaggingTrackAction::fillGaussTrackInformation( const G4Track* track ) const
//{
// const G4DynamicParticle* dynamicParticle = track->GetDynamicParticle();
// if ( 0 == dynamicParticle ) return;

// G4PrimaryParticle* primaryParticle = dynamicParticle->GetPrimaryParticle();
// if ( 0 == primaryParticle ) return;

// G4VUserPrimaryParticleInformation* g4UserInfo =
// track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation();
// if ( 0 == g4UserInfo ) return;

// GiGaPrimaryParticleInformation* uInf = (GiGaPrimaryParticleInformation*)g4UserInfo;
// GaussTrajectory* traj                = (GaussTrajectory*)( trackMgr()->GimmeTrajectory() );

// if ( uInf->hasOscillated() ) traj->setHasOscillated( true );
// if ( uInf->isSignal() ) traj->setIsSignal( true );
// traj->setSignalBarcode( uInf->signalBarcode() );
// traj->setHepMCEvent( uInf->pHepMCEvent() );
// traj->setMotherMCParticle( uInf->motherMCParticle() );
//}

// void TruthFlaggingTrackAction::setProcess( const G4Track* track ) const
//{
// const G4VProcess* process = track->GetCreatorProcess();
// if ( 0 == process ) return;
// GaussTrajectory* traj = (GaussTrajectory*)( trackMgr()->GimmeTrajectory() );
// traj->setProcessName( process->GetProcessName() );
//}
