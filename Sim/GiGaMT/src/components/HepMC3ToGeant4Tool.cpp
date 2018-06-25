// local
#include "HepMC3ToGeant4Tool.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// Geant4
#include "Geant4/G4Event.hh"
#include "Geant4/G4SystemOfUnits.hh"

// HepMC3
#include "HepMC/FourVector.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/Units.h"

// Declaration of the Tool
DECLARE_COMPONENT( HepMC3ToGeant4Tool )

HepMC3ToGeant4Tool::~HepMC3ToGeant4Tool() {}

G4Event* HepMC3ToGeant4Tool::g4Event( const std::vector<HepMC::GenEvent>& hepmc_events )
{
  // TODO: Make sure the event is deleted somewhere
  if ( hepmc_events.size() == 0 ) {
    return nullptr;
  }
  G4Event* g4Event = new G4Event();
  HepMC::FourVector tmp;
  for ( const auto& genEvt : hepmc_events ) {
    // Adding one particle per one vertex => vertices repeated

    for ( auto& vertex : genEvt.vertices() ) {
      for ( auto& particle : vertex->particles( HepMC::children ) ) {
        // take only final state particles
        if ( particle->end_vertex() ) continue;
        tmp = vertex->position();
        HepMC::Units::convert( tmp, genEvt.length_unit(), HepMC::Units::MM );
        G4PrimaryVertex* g4Vertex = new G4PrimaryVertex( tmp.x() * mm, tmp.y() * mm, tmp.z() * mm, tmp.t() );

        tmp = particle->momentum();
        //HepMC::Units::convert( tmp, genEvt.momentum_unit(), HepMC::Units::MEV );
        G4PrimaryParticle* g4Particle =
            new G4PrimaryParticle( particle->pdg_id(), tmp.px() * MeV, tmp.py() * MeV, tmp.pz() * MeV );
        g4Vertex->SetPrimary( g4Particle );
        g4Event->AddPrimaryVertex( g4Vertex );
      }
    }
  }
  return g4Event;
}
