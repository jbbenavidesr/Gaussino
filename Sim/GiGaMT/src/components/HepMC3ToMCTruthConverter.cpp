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
// local
#include "HepMC3ToMCTruthConverter.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// Geant4
#include "Geant4/G4Event.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4ParticleTable.hh"

// HepMC3
#include "GaudiKernel/Vector4DTypes.h"
#include "HepMC3/FourVector.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Relatives.h"
#include "HepMC3/Units.h"
#include "HepMCUtils/PrintDecayTree.h"
#include "Math/GenVector/Boost.h"

#include "Defaults/HepMCAttributes.h"
#include "HepMCUser/Status.h"
#include "HepMCUser/VertexAttribute.h"

// Declaration of the Tool
DECLARE_COMPONENT( HepMC3ToMCTruthConverter )

double lifetime( const HepMC3::FourVector mom, const HepMC3::GenVertexPtr& P, const HepMC3::GenVertexPtr& E ) {
  if ( !E ) return 0;
  Gaudi::LorentzVector A( P->position() ), B( E->position() );
  Gaudi::LorentzVector AB = B - A;

  // Switch to mm for time.
  AB.SetE( AB.T() * Gaudi::Units::c_light );

  // Boost displacement 4-vector to rest frame of particle.
  Gaudi::LorentzVector M( mom );
  ROOT::Math::Boost    theBoost( M.BoostToCM() );
  Gaudi::LorentzVector ABStar = theBoost( AB );

  // Switch back to time.
  return ABStar.T() / Gaudi::Units::c_light;
}

Gaussino::ConversionType HepMC3ToMCTruthConverter::GetConversionType( const HepMC3::ConstGenParticlePtr& particle ) const {
  if(particle->status() == HepMC3::Status::ReDecay){
    return Gaussino::ConversionType::REDECAY;
  } else if (IsTraveling( particle )){
    return Gaussino::ConversionType::G4;
  }
  return  Gaussino::ConversionType::MC;
}

Gaussino::MCTruthConverterPtrs
HepMC3ToMCTruthConverter::BuildConverter( const HepMC3::GenEventPtrs& hepmc_events ) const {
  Gaussino::MCTruthConverterPtrs converters;

  unsigned int iEvent{0};
  for ( auto& genEvt : hepmc_events ) {
    iEvent++;
    if ( msgLevel( MSG::VERBOSE ) ) {
      m_ppSvc.retrieve().ignore();
      verbose() << "Beam particles: " << endmsg;
      for ( auto beam:genEvt->beams() ) {

        if(auto tmp = m_ppSvc->find( LHCb::ParticleID( beam->pdg_id() ) ); tmp){
            verbose() << tmp->name();
        } else {
            verbose() << "UNKNOWN(" << beam->pdg_id() << ")";
        }
        verbose() << " -> #" << beam->id() << ", " << HepMC3::to_status_type(beam->status()) << endmsg;
      }
      for ( size_t ib = 0; ib < genEvt->beams().size(); ib++ ) {
        verbose() << "HepMC event " << iEvent << " dump: beam=" << ib << " \n"
                  << PrintDecay( genEvt->beams().at( ib ), 0, m_ppSvc.get() ) << endmsg;
      }
    }
    auto converter = std::make_unique<Gaussino::MCTruthConverter>();
    if ( genEvt->length_unit() != HepMC3::Units::MM || genEvt->momentum_unit() != HepMC3::Units::MEV ) {
      error() << "Units of HepMC event do not match. Skipping event" << endmsg;
      continue;
    }
    for ( auto& part : genEvt->particles() ) {
      if ( !keep( part ) ) { continue; }
      // We add all the particles here to the container without caring about whether those particles have previously been simulated in another MCTruth object. This will be done during the linking when the container is prepared for Geant4.
      converter->Declare( part, GetConversionType(part) );
    }
    converters.push_back( std::move( converter ) );
  }
  return converters;
}

Gaussino::MCTruthConverterPtr
HepMC3ToMCTruthConverter::BuildConverter( const HepMC3::ConstGenParticlePtr& part ) const {
  auto                           genEvt = part->parent_event();

  if ( genEvt->length_unit() != HepMC3::Units::MM || genEvt->momentum_unit() != HepMC3::Units::MEV ) {
    error() << "Units of HepMC event do not match. Skipping event" << endmsg;
    return nullptr;
  }
  auto converter = std::make_unique<Gaussino::MCTruthConverter>();
  if ( keep( part ) ) { 
    converter->Declare( part, GetConversionType(part) );
  }
  for ( auto& desc : HepMC3::Relatives::DESCENDANTS( part ) ) {
    if ( !keep( desc ) ) { continue; }
    converter->Declare( desc, GetConversionType(desc) );
  }
  return converter;
}

bool HepMC3ToMCTruthConverter::IsTraveling( const HepMC3::ConstGenParticlePtr& part ) const {
  // Return for Geant4 tracking if stable.
  auto ev = part->end_vertex();
  if ( !ev ) {
    if(m_check_particle.value() && !G4ParticleTable::GetParticleTable()->FindParticle(part->pdg_id())){
      PrintDecay(*std::begin(part->parent_event()->beams()));
      std::stringstream msg;
      msg << "Particle " << part->pdg_id() << " has no endvertex but is unknown to Geant4";
      throw GaudiException(msg.str(), "HepMC3ToMCTruthConverter::IsTraveling", StatusCode::FAILURE);
    }
    return true;
  }

  // Determine the travel distance.
  auto   pv   = part->production_vertex();
  double dist = ( ev->position() - pv->position() ).p3mod();

  if ( dist < m_travelLimit ) return false;

  if(m_check_particle.value() && !G4ParticleTable::GetParticleTable()->FindParticle(part->pdg_id())){
    PrintDecay(*std::begin(part->parent_event()->beams()));
    std::stringstream msg;
    msg << "Particle " << part->pdg_id() << " has travels " << dist << "mm but is unknown to Geant4";
    throw GaudiException(msg.str(), "HepMC3ToMCTruthConverter::IsTraveling", StatusCode::FAILURE);
  }

  return true;
}

//=============================================================================
// Decides if a particle should be kept in MCParticles.
//=============================================================================
bool HepMC3ToMCTruthConverter::keep( const HepMC3::ConstGenParticlePtr& particle ) const {
  LHCb::ParticleID pid( particle->pdg_id() );
  // Get the signal process ID as we will need this multiple times.
  // If the IntAttribute for the process ID was not added to the event,
  // a default constructed shared pointer to IntAttribute is returned which
  // by default initialises the process ID to 0 which is identical to the
  // behaviour in HepMC2 where the default value was 0 as well.
  int sig_proc_id{0};
  if ( auto attr =
           particle->parent_event()->attribute<HepMC3::IntAttribute>( Gaussino::HepMC::Attributes::SignalProcessID );
       attr ) {
    sig_proc_id = attr->value();
  }
  switch ( particle->status() ) {
  case HepMC3::Status::StableInProdGen:
    return true;
  case HepMC3::Status::DecayedByDecayGen:
    return true;
  case HepMC3::Status::DecayedByDecayGenAndProducedByProdGen:
    return true;
  case HepMC3::Status::SignalInLabFrame:
    return true;
  case HepMC3::Status::StableInDecayGen:
    return true;
  // Always keep the particle marked for ReDecay
  case HepMC3::Status::ReDecay:
    return true;
  // Reject children of ReDecay particles
  case HepMC3::Status::ChildOfReDecay:
    return false;

    // these act as placeholders before status codes can be put in MCEvent
  case 21:
    return true; // case LHCb::HepMCEvent::PythiaIncomingParton: return true;
  case 22:
    return true; // case LHCb::HepMCEvent::PythiaHardProcess: return true;

  // For some processes the resonance has status 3.
  case HepMC3::Status::DocumentationParticle:
    if ( 24 == sig_proc_id ) {
      if ( 23 == pid.abspid() )
        return true;
      else if ( 25 == pid.abspid() )
        return true;
    } else if ( 26 == sig_proc_id ) {
      if ( 24 == pid.abspid() )
        return true;
      else if ( 25 == pid.abspid() )
        return true;
    } else if ( 102 == sig_proc_id ) {
      if ( 25 == pid.abspid() ) return true;
    } else if ( 6 == pid.abspid() )
      return true;
    return false;
  case HepMC3::Status::Unknown:
    return false;
  case HepMC3::Status::DecayedByProdGen:
    if ( pid.isHadron() ) return true;
    if ( pid.isLepton() ) return true;
    if ( pid.isNucleus() ) return true;
    if ( pid.isDiQuark() ) return false;

    // Store particles of interest.
    switch ( pid.abspid() ) {
    case LHCb::ParticleID::down:
      return false;
    case LHCb::ParticleID::up:
      return false;
    case LHCb::ParticleID::strange:
      return false;
    case LHCb::ParticleID::charm:
      return true;
    case LHCb::ParticleID::bottom:
      return true;
    case LHCb::ParticleID::top:
      return false;
    case 21:
      return false; // Gluon.
    case 22:
      return true; // Photon.
    case 23:       // Z0.
      if ( 24 == sig_proc_id )
        return false;
      else
        return true;
    case 24: // W.
      if ( 26 == sig_proc_id )
        return false;
      else
        return true;
    case 25: // SM Higgs.
      if ( 24 == sig_proc_id || 26 == sig_proc_id || 102 == sig_proc_id )
        return false;
      else
        return true;
    case 32:
      return true; // Z'.
    case 33:
      return true; // Z''.
    case 34:
      return true; // W'.
    case 35:
      return true; // CP-even heavy Higgs (H0/H2).
    case 36:
      return true; // CP-odd Higgs (A0/H3).
    case 37:
      return true; // Charged Higgs (H+).
    // See Table 6 of the Pythia 6 manual (arxiv.org/abs/hep-ph/0603175).
    case 90:
      return false; // System particle.
    case 91:
      return false; // Parton system from cluster fragmentation.
    case 92:
      return false; // Parton system from string fragmentation.
    case 93:
      return false; // Parton system from independent fragmentation.
    case 94:
      return false; // Time-like showering system.
    default:
      return true;
    }
    return true;
  default:
    return false;
  }
  return false;
}
