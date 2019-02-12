// local
#include "HepMC3ToGeant4Tool.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// Geant4
#include "Geant4/G4Event.hh"
#include "Geant4/G4SystemOfUnits.hh"

// HepMC3
#include "GaudiKernel/Vector4DTypes.h"
#include "HepMC/FourVector.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/Units.h"
#include "HepMCUtils/PrintDecayTree.h"
#include "Math/GenVector/Boost.h"

// Declaration of the Tool
DECLARE_COMPONENT( HepMC3ToGeant4Tool )

std::string PrintPrimaries( G4PrimaryParticle* part, int level = 0, LHCb::IParticlePropertySvc* ppsvc = nullptr )
{
  std::string space = "";
  for ( int i = 0; i < level; i++ ) {
    space += "|---> ";
  }
  std::stringstream outstream;
  outstream << space;
  if ( ppsvc ) {
    outstream << ppsvc->find( LHCb::ParticleID( part->GetPDGcode() ) )->name();
  } else {
    outstream << part->GetPDGcode();
  }
  outstream << "\n";
  auto p = part->GetDaughter();
  while ( p ) {
    outstream << PrintPrimaries( p, level + 1, ppsvc );
    p = p->GetNext();
  }
  return outstream.str();
}

double lifetime( const HepMC::FourVector mom, const HepMC::GenVertexPtr& P, const HepMC::GenVertexPtr& E )
{
  if ( !E ) return 0;
  Gaudi::LorentzVector A( P->position() ), B( E->position() );
  Gaudi::LorentzVector AB = B - A;

  // Switch to mm for time.
  AB.SetE( AB.T() * Gaudi::Units::c_light );

  // Boost displacement 4-vector to rest frame of particle.
  Gaudi::LorentzVector M( mom );
  ROOT::Math::Boost theBoost( M.BoostToCM() );
  Gaudi::LorentzVector ABStar = theBoost( AB );

  // Switch back to time.
  return ABStar.T() / Gaudi::Units::c_light;
}

HepMC3ToGeant4Tool::~HepMC3ToGeant4Tool() {}

StatusCode HepMC3ToGeant4Tool::initialize()
{
  extends::initialize();
  if ( !m_ppsvc ) m_ppsvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc" );
  return StatusCode::SUCCESS;
}

G4Event* HepMC3ToGeant4Tool::g4Event( const std::vector<const HepMC::GenEvent*>& hepmc_events )
{

  // TODO: Make sure the event is deleted somewhere
  if ( hepmc_events.size() == 0 ) {
    return nullptr;
  }
  G4Event* g4Event = new G4Event();
  for ( const auto& genEvt : hepmc_events ) {
    // Adding the primary vertex and then iteratively add children to it
    if ( genEvt->length_unit() != HepMC::Units::MM || genEvt->momentum_unit() != HepMC::Units::MEV ) {
      error() << "Units of HepMC event do not match. Skipping event" << endmsg;
      continue;
    }
    HepMC::GenVertexPtr PV{nullptr};
    auto beam_particles  = genEvt->beam_particles();
    auto fbeam_particles = beam_particles.first;
    if ( fbeam_particles ) {
      auto PV = fbeam_particles->end_vertex();
    }
    if ( !PV ) {
      if ( genEvt->particles_size() == 0 ) {
        continue;
      }
      PV = ( *std::begin( genEvt->particles() ) )->production_vertex();
      if ( !PV ) {
        PV = ( *std::begin( genEvt->particles() ) )->end_vertex();
      }
    }
    HepMC::FourVector PVposition = PV->position();
    G4PrimaryVertex* g4PV =
        new G4PrimaryVertex( PVposition.x() * mm, PVposition.y() * mm, PVposition.z() * mm, PVposition.t() );

    for ( auto& particle : PV->particles( HepMC::children ) ) {
      convert( particle, nullptr, g4PV );
    }
    g4Event->AddPrimaryVertex( g4PV );

    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Geant4 tree for this event: \n";
      auto p = g4PV->GetPrimary();
      while ( p ) {
        debug() << PrintPrimaries( p, 0, m_ppsvc );
        p = p->GetNext();
      }
      debug() << endmsg;
    }
  }
  return g4Event;
}

void HepMC3ToGeant4Tool::convert( const HepMC::GenParticlePtr& hepmc, G4PrimaryParticle* g4parent,
                                  G4PrimaryVertex* g4vertex )
{
  G4PrimaryParticle* g4Particle = nullptr;
  auto PV                       = hepmc->production_vertex();
  auto EV                       = hepmc->end_vertex();
  auto momentum                 = hepmc->momentum(); // In MeV from HepMC, hopefully

  bool togeant4 = IsTraveling( hepmc );
  if ( togeant4 ) {
    if ( !g4parent && msgLevel( MSG::DEBUG ) ) {
      debug() << "Starting a Geant4 decay tree: \n " << PrintDecay( hepmc, 0, m_ppsvc ) << endmsg;
    }
    g4Particle =
        new G4PrimaryParticle( hepmc->pdg_id(), momentum.px() * MeV, momentum.py() * MeV, momentum.pz() * MeV );
    if ( g4parent ) {
      g4parent->SetDaughter( g4Particle );
    }
    if ( g4vertex ) {
      // Attach this particle to the provided vertex and prevent that
      // the children see the vertex at all by setting it to null.
      g4vertex->SetPrimary( g4Particle );
      g4vertex = nullptr;
    }
    if ( EV ) {
      auto dtime = lifetime( momentum, PV, EV );
      g4Particle->SetProperTime( dtime );
    }
  } else {
    // The current particle is supposed to be skipped meaning any children
    // should be attached to the whatever this particle should have been
    // attached to
    g4Particle = g4parent;
  }
  if ( EV ) {
    for ( auto& child : hepmc->children() ) {
      convert( child, g4Particle, g4vertex );
    }
  }
}

bool HepMC3ToGeant4Tool::IsTraveling( const HepMC::GenParticlePtr& part )
{
  // Return for Geant4 tracking if stable.
  auto ev = part->end_vertex();
  if ( !ev ) {
    return true;
  }

  // Determine the travel distance.
  auto pv     = part->production_vertex();
  double dist = ( ev->position() - pv->position() ).p3mod();

  if ( dist < m_travelLimit ) return false;

  return true;
}
