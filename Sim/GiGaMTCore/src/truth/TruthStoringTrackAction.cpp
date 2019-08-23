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
T& operator<<( T& ostr, const HepMC3::FourVector& fv )
{
  ostr << "[" << fv.x() << ", " << fv.y() << ", " << fv.z() << ", " << fv.t() << "]";
  return ostr;
}

void TruthStoringTrackAction::PreUserTrackingAction( const G4Track* track )
{
  // new track is being started
  // we record its initial momentum
  fourmomentum = HepMC3::FourVector( track->GetMomentum().x(), track->GetMomentum().y(), track->GetMomentum().z(),
                                     track->GetTotalEnergy() );
#ifdef TRUTHDEBUG
  if ( printDebug() ) {
    HepMC3::FourVector prodpos( track->GetVertexPosition().x(), track->GetVertexPosition().y(),
                                track->GetVertexPosition().z(), track->GetGlobalTime() - track->GetLocalTime() );
    auto track_info = GaussinoTrackInformation::Get();
    int pdgID       = track->GetDefinition()->GetPDGEncoding();
    HepMC3::FourVector endpos( track->GetPosition().x(), track->GetPosition().y(), track->GetPosition().z(),
                               track->GetGlobalTime() );
    G4cout << "##### STARTING NEW TRACK #####" << G4endl;
    G4cout << "TrackID " << track->GetTrackID() << G4endl;
    G4cout << "ParentID " << track->GetParentID() << G4endl;
    G4cout << "PdgID " << pdgID << G4endl;
    G4cout << "Direct parent converted " << track_info->directParent() << G4endl;
    G4cout << "Current pos" << endpos << G4endl;
    G4cout << "Momentum " << fourmomentum << G4endl;
  }
#endif
}

void TruthStoringTrackAction::PostUserTrackingAction( const G4Track* track )
{
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

  if ( track_info->storeTruth() ) {
    HepMC3::FourVector prodpos( track->GetVertexPosition().x(), track->GetVertexPosition().y(),
                                track->GetVertexPosition().z(), track->GetGlobalTime() - track->GetLocalTime() );
    HepMC3::FourVector endpos( track->GetPosition().x(), track->GetPosition().y(), track->GetPosition().z(),
                               track->GetGlobalTime() );

    // Get the pdgID+LHCb extension
    int pdgID = track->GetDefinition()->GetPDGEncoding();
#ifdef TRUTHDEBUG
    if ( printDebug() ) {
      G4cout << "##### STORING IT #####" << G4endl;
      G4cout << "Storing new track" << G4endl;
      G4cout << "Reason " << track_info->GetStoreReason() << G4endl;
      G4cout << "EndPos " << endpos << G4endl;
      G4cout << "Momentum " << fourmomentum << G4endl;
    }
#endif
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
      // FIXME: Simplified the original code and removed usage of property service. Might need to revisit.
      // if ( 0 == pdgID ) {
      //// Last chance, use name of particle
      // const LHCb::ParticleProperty* pProp =
      // m_ppSvc->find( track->GetDefinition()->GetParticleName() );
      // if( NULL != pProp ) {
      // pdgID = pProp->pdgID().pid();
      //} else {
      // std::string message = "PDGEncoding does not exist, G4 name is ";
      // message += track->GetDefinition()->GetParticleName();
      // Warning( message, StatusCode::SUCCESS, 10 );
      //}
      //}
      if ( 0 == pdgID ) {
        // Last chance, use name of particle
        std::string message = "PDGEncoding does not exist, G4 name is ";
        message += track->GetDefinition()->GetParticleName();
        G4cerr << __PRETTY_FUNCTION__ << message << G4endl;
        warning( message );
      }
    }
    // get the process type of the origin vertex
    int creatorID = processID( track->GetCreatorProcess() );

    // Get User information from primary particle to set Vertex type
    // OscillatedAndDecay and to set SignalFlag

    auto event_info = GaussinoEventInformation::Get();
    if ( !event_info ) {
      G4cerr << __PRETTY_FUNCTION__ << " no event information. " << G4endl;
      return;
    }

    HepMC3::FourVector final_fourmomentum{track->GetMomentum().x(), track->GetMomentum().y(), track->GetMomentum().z(),
                                          track->GetTotalEnergy()};
    auto particle = new Gaussino::G4TruthParticle{track->GetTrackID(), pdgID, creatorID, fourmomentum, final_fourmomentum, prodpos, endpos};
    // Now check if the particle is a primary particle, i.e. we have already created
    // a linked particle for it.
    if ( track->GetDynamicParticle() && track->GetDynamicParticle()->GetPrimaryParticle() ) {
      auto primary_info = GaussinoPrimaryParticleInformation::Get( track->GetDynamicParticle()->GetPrimaryParticle() );
      auto linkedparticleID = primary_info->getLinkedID();
      if ( linkedparticleID == 0 ) {
        G4cerr << __PRETTY_FUNCTION__ << " track is primary but user info does not point to a LinkedParticle."
               << G4endl;
      }
      event_info->TruthTracker()->RegisterPrimary( particle, linkedparticleID );
    } else {
      event_info->TruthTracker()->Declare( particle, track->GetParentID() );
    }
  }
}

int TruthStoringTrackAction::processID( const G4VProcess* creator )
{
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
