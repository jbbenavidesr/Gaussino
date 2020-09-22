// $Id: TruthStoringTrackAction.cpp,v 1.9 2008-10-20 08:23:11 robbep Exp $
// Include files

// STD & STL
#include <algorithm>
#include <functional>

// from Gaudi

// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// G4
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4PrimaryParticle.hh"
#include "Geant4/G4TrackingManager.hh"

// Included for vertex types
// FIXME: Need to reorganise when splitting LHCb
#include "Event/MCVertex.h"

// local
#include "GiGaMTCoreTruth/G4TruthParticle.h"
#include "GiGaMTCoreTruth/GaussinoEventInformation.h"
#include "GiGaMTCoreTruth/GaussinoPrimaryParticleInformation.h"
#include "GiGaMTCoreTruth/GaussinoTrackInformation.h"
#include "GiGaMTCoreTruth/TruthStoringTrackAction.h"

/*static*/ std::unordered_set<std::string> TruthStoringTrackAction::m_hadronicProcesses = {
    "KaonPlusInelastic",
    "PionMinusAbsorptionAtRest",
    "KaonZeroLInelastic",
    "KaonZeroSInelastic",
    "MuonMinusCaptureAtRest",
    "TritonInelastic",
    "KaonMinusAbsorption",
    "LambdaInelastic",
    "SigmaMinusInelastic",
    "LCapture",
    "AntiNeutronAnnihilationAtRest",
    "AntiProtonAnnihilationAtRest",
    "AntiLambdaInelastic",
    "AntiXiZeroInelastic",
    "AntiSigmaPlusInelastic",
    "SigmaPlusInelastic",
    "XiMinusInelastic",
    "XiZeroInelastic",
    "AntiSigmaMinusInelastic",
    "AntiXiMinusInelastic",
    "OmegaMinusInelastic",
    "AntiOmegaMinusInelastic",
    "AlphaInelastic"};

template <typename T>
T& operator<<( T& ostr, const HepMC3::FourVector& fv ) {
  ostr << "[" << fv.x() << ", " << fv.y() << ", " << fv.z() << ", " << fv.t() << "]";
  return ostr;
}

void TruthStoringTrackAction::PreUserTrackingAction( const G4Track* track ) {
  // new track is being started
  // we record its initial momentum
  fourmomentum = HepMC3::FourVector( track->GetMomentum().x(), track->GetMomentum().y(), track->GetMomentum().z(),
                                     track->GetTotalEnergy() );
}

void TruthStoringTrackAction::PostUserTrackingAction( const G4Track* track ) {
  if ( !track->GetUserInformation() ) {
    G4cerr << __PRETTY_FUNCTION__
           << " Could not find user track information. Likely wrong order of actions in sequence!" << G4endl;
    return;
  }
  auto event = G4EventManager::GetEventManager()->GetNonconstCurrentEvent();
  if ( !event->GetUserInformation() ) {
    G4cerr << __PRETTY_FUNCTION__ << " Could not find user event information. Check conversion to Geant4!" << G4endl;
    return;
  }

  auto track_info = GaussinoTrackInformation::Get();
  HepMC3::FourVector prodpos( track->GetVertexPosition().x(), track->GetVertexPosition().y(),
                              track->GetVertexPosition().z(), track->GetGlobalTime() - track->GetLocalTime() );
    // get the process type of the origin vertex
    int creatorID = processID( track->GetCreatorProcess() );
    // Get User information from primary particle to set Vertex type
    // OscillatedAndDecay and to set SignalFlag

    auto event_info = GaussinoEventInformation::Get();

    if ( !event_info ) {
      G4cerr << __PRETTY_FUNCTION__ << " no event information. " << G4endl;
      return;
    }

  if ( track_info->storeTruth() ) {
    HepMC3::FourVector endpos( track->GetPosition().x(), track->GetPosition().y(), track->GetPosition().z(),
                               track->GetGlobalTime() );

    // Get the pdgID+LHCb extension
    int pdgID = track->GetDefinition()->GetPDGEncoding();
    if ( 0 == pdgID ) {
      // Use dynamic particle PDG Id in this case (unknown particle)
      if ( track->GetDynamicParticle() && track->GetDynamicParticle()->GetPrimaryParticle() ) {
        pdgID = track->GetDynamicParticle()->GetPrimaryParticle()->GetPDGcode();
        if ( "unknown" == track->GetDefinition()->GetParticleName() ) {
          double ener = sqrt( track->GetDynamicParticle()->GetPrimaryParticle()->GetMomentum().mag2() +
                              track->GetDynamicParticle()->GetPrimaryParticle()->GetMass() *
                                  track->GetDynamicParticle()->GetPrimaryParticle()->GetMass() );
          fourmomentum.setPx( track->GetDynamicParticle()->GetPrimaryParticle()->GetMomentum().x() );
          fourmomentum.setPy( track->GetDynamicParticle()->GetPrimaryParticle()->GetMomentum().y() );
          fourmomentum.setPz( track->GetDynamicParticle()->GetPrimaryParticle()->GetMomentum().z() );
          fourmomentum.setE( ener );
        }
      }
      if ( 0 == pdgID ) {
        // Last chance, use name of particle
        auto id = m_fNameToID( track->GetDefinition()->GetParticleName() );
        if ( id.has_value() ) {
          pdgID = id.value();
        } else {
          std::string message = "PDGEncoding does not exist, G4 name is ";
          message += track->GetDefinition()->GetParticleName();
          G4cerr << __PRETTY_FUNCTION__ << message << G4endl;
          warning( message );
        }
      }
    }

    HepMC3::FourVector final_fourmomentum{track->GetMomentum().x(), track->GetMomentum().y(), track->GetMomentum().z(),
                                          track->GetTotalEnergy()};
    // Skip if the track has already been saved because it was previously suspended
    if ( track_info->isSuspendedAndSaved() ) { return; }
    if ( track->GetTrackStatus() == G4TrackStatus::fSuspend ) { track_info->suspendedAndSaved(); }

    auto particle = new Gaussino::G4TruthParticle{track->GetTrackID(), pdgID,   creatorID, fourmomentum,
                                                  final_fourmomentum,  prodpos, endpos};
    // Now check if the particle is a primary particle, i.e. we have already created
    // a linked particle for it.
    if ( track->GetDynamicParticle() && track->GetDynamicParticle()->GetPrimaryParticle() ) {
      auto primary_info = GaussinoPrimaryParticleInformation::Get( track->GetDynamicParticle()->GetPrimaryParticle() );
      auto linkedparticleID = primary_info->getLinkedID();
      if ( linkedparticleID == 0 ) {
        G4cerr << __PRETTY_FUNCTION__ << " track is primary but user info does not point to a LinkedParticle."
               << G4endl;
      }

      // First check if the G4TruthInformation has already been provided
      // This can happen if the track is suspended during processing, usually to keep
      // the number of optical photons down as much as possible. In this case we ignore
      // the additional truth information provided.
      event_info->TruthTracker()->RegisterPrimary( particle, linkedparticleID );
    } else {
      event_info->TruthTracker()->Declare( particle, track->GetParentID() );
    }
  } else if (addEndVertices && track_info->directParent() && !track_info->isSuspendedAndSaved()){
    // If the particle is not to be saved, add the endvertex with the corresponding process nonetheless
    if ( track->GetTrackStatus() == G4TrackStatus::fSuspend ) { track_info->suspendedAndSaved(); }
    event_info->TruthTracker()->DeclareEnd(prodpos, creatorID, track->GetParentID());
  }

}

int TruthStoringTrackAction::processID( const G4VProcess* creator ) {
  // FIXME: Need to define those codes somewhere centrally

  int processID = LHCb::MCVertex::Unknown;
  if ( NULL == creator ) {
    processID = LHCb::MCVertex::DecayVertex;
    return processID;
  }

  if ( fDecay == creator->GetProcessType() ) {
    processID = LHCb::MCVertex::DecayVertex;
  } else if ( fHadronic == creator->GetProcessType() ) {
    processID = LHCb::MCVertex::HadronicInteraction;
  } else {
    const std::string& pname = creator->GetProcessName();
    if ( "conv" == pname ) {
      processID = LHCb::MCVertex::PairProduction;
    } else if ( "compt" == pname ) {
      processID = LHCb::MCVertex::Compton;
    } else if ( "eBrem" == pname || "muBrems" == pname ) {
      processID = LHCb::MCVertex::Bremsstrahlung;
    } else if ( "annihil" == pname ) {
      processID = LHCb::MCVertex::Annihilation;
    } else if ( "phot" == pname ) {
      processID = LHCb::MCVertex::PhotoElectric;
    } else if ( "RichHpdPhotoelectricProcess" == pname ) {
      processID = LHCb::MCVertex::RICHPhotoElectric;
    } else if ( "RichPmtPhotoelectricProcess" == pname ) {
      processID = LHCb::MCVertex::RICHPhotoElectric;
    } else if ( "TorchTBMcpPhotoElectricProcess" == pname ) {
      processID = LHCb::MCVertex::RICHPhotoElectric;
    } else if ( "RichG4Cerenkov" == pname ) {
      processID = LHCb::MCVertex::Cerenkov;
    } else if ( "eIoni" == pname || "hIoni" == pname || "ionIoni" == pname || "muIoni" == pname ) {
      processID = LHCb::MCVertex::DeltaRay;
    } else {
      if ( const bool found = m_hadronicProcesses.count( pname ) > 0; found ) {
        processID = LHCb::MCVertex::HadronicInteraction;
      }
    }
  }

  if ( processID == 0 ) {
    // here we have an intertsting situation
    // the process is *KNOWN*, but the vertex type
    // is still 'Unknown'
    std::string message = "The process is known '" + G4VProcess::GetProcessTypeName( creator->GetProcessType() ) + "/" +
                          creator->GetProcessName() + "', but vertex type is still 'Unknown'";
    warning( message );
  }

  return processID;
}

// ============================================================================
