#include "Geant4/G4UserTrackingAction.hh"
#include "Geant4/G4VProcess.hh"

#include "GiGaMTCoreMessage/IGiGaMessage.h"
#include "HepMC3/FourVector.h"
#include <string>
#include <unordered_set>
// STD & STL
#include <algorithm>
#include <functional>

// from Gaudi

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

// namespace LHCb {
// class IParticlePropertySvc;
//}

/** @class DebugTrackAction DebugTrackAction.h
 *
 *  @author Dominik Muller
 *  @date   28/02/2019
 */

namespace Gaussino
{
  class DebugTrackAction : virtual public G4UserTrackingAction
  {
  public:
    virtual ~DebugTrackAction()                 = default;
    DebugTrackAction()                          = default;
    DebugTrackAction( const DebugTrackAction& ) = delete;

    // We only know whether a track is to be stored at the end
    // of its tracking. However, G4Track only represents the current state.
    // Therefore, this function copies information on the initial state
    // of the track which is later used if the track is to be stored.
    void PreUserTrackingAction( const G4Track* track ) override;

    void PostUserTrackingAction( const G4Track* track ) override;

  protected:
    // Function to return an integer ID for the process. Currently
    // implements the default LHCb behavior and codes but is declared
    // virtual so users can inherit and implement their own version.
    virtual LHCb::MCVertex::MCVertexType processID( const G4VProcess* creator );

    static std::unordered_set<std::string> m_hadronicProcesses;
  };

  /*static*/ std::unordered_set<std::string> DebugTrackAction::m_hadronicProcesses = {"KaonPlusInelastic",
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

  void DebugTrackAction::PreUserTrackingAction( const G4Track* track )
  {
    // new track is being started
    // we record its initial momentum
    int pdgID = track->GetDefinition()->GetPDGEncoding();
    HepMC3::FourVector endpos( track->GetPosition().x(), track->GetPosition().y(), track->GetPosition().z(),
                               track->GetGlobalTime() );

    HepMC3::FourVector fourmomentum ( track->GetMomentum().x(), track->GetMomentum().y(), track->GetMomentum().z(),
                                        track->GetTotalEnergy() );
    G4cout << "##### STARTING NEW TRACK #####" << G4endl;
    G4cout << "TrackID " << track->GetTrackID() << G4endl;
    G4cout << "ParentID " << track->GetParentID() << G4endl;
    G4cout << "PdgID " << pdgID << G4endl;
    G4cout << "Process: " << processID( track->GetCreatorProcess() ) << G4endl;
    G4cout << "Position: " << endpos << G4endl;
    G4cout << "Momentum: " << fourmomentum << G4endl;
  }

  void DebugTrackAction::PostUserTrackingAction( const G4Track* track )
  {
    if ( !track->GetUserInformation() ) {
      G4cerr << __PRETTY_FUNCTION__
             << " Could not find user track information. Likely wrong order of actions in sequence!" << G4endl;
      return;
    }
    HepMC3::FourVector endpos( track->GetPosition().x(), track->GetPosition().y(), track->GetPosition().z(),
                               track->GetGlobalTime() );
    G4cout << "Decay Position: " << endpos << G4endl;

    auto track_info = GaussinoTrackInformation::Get();
    // Now check if the particle is a primary particle, i.e. we have already created
    // a linked particle for it.
    if ( track->GetDynamicParticle() && track->GetDynamicParticle()->GetPrimaryParticle() ) {
      auto primary_info = GaussinoPrimaryParticleInformation::Get( track->GetDynamicParticle()->GetPrimaryParticle() );
      G4cout << *primary_info << G4endl;
    }

    if ( track_info->storeTruth() ) {
      G4cout << "Will be stored" << G4endl;
    }
    G4cout << "##### FINISHED NEW TRACK #####" << G4endl;
  }

  LHCb::MCVertex::MCVertexType DebugTrackAction::processID( const G4VProcess* creator )
  {
    // FIXME: Need to define those codes somewhere centrally

    LHCb::MCVertex::MCVertexType processID = LHCb::MCVertex::Unknown;
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

    return processID;
  }
} // namespace Gaussino

#include "GaudiAlg/GaudiTool.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"

#include "Geant4/G4UserTrackingAction.hh"

class DebugTrackActionFAC : public extends<GaudiTool, GiGaFactoryBase<G4UserTrackingAction>>
{
  using extends::extends;
  virtual G4UserTrackingAction* construct() const override { return new Gaussino::DebugTrackAction{}; }
};

DECLARE_COMPONENT_WITH_ID( DebugTrackActionFAC, "DebugTrackAction" )
