#pragma once
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include <sstream>

// from Gaudi
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

std::string PrintDecay( const HepMC::GenParticlePtr& part, int level = 0, LHCb::IParticlePropertySvc* ppsvc = nullptr )
{
  std::string space = "";
  for ( int i = 0; i < level; i++ ) {
    space += "|---> ";
  }
  std::stringstream outstream;
  outstream << space;
  if ( ppsvc ) {
    outstream << ppsvc->find( LHCb::ParticleID( part->pdg_id() ) )->name();
  } else {
    outstream << part->pdg_id();
  }
  outstream << " -> #" << part->id() << ", " << part->status() << "\n";
  if ( part->end_vertex() ) {
    for ( auto p : part->children() ) {
      outstream << PrintDecay( p, level + 1, ppsvc );
    }
  }
  return outstream.str();
}
