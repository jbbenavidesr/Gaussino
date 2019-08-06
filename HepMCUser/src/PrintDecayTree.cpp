#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include <sstream>
#include "HepMCUtils/PrintDecayTree.h"

// from Gaudi
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "HepMCUser/Status.h"

std::string PrintDecay( const HepMC3::ConstGenParticlePtr& part, int level, const LHCb::IParticlePropertySvc* ppsvc)
{
  std::string space = "";
  for ( int i = 0; i < level; i++ ) {
    space += "|---> ";
  }
  using HepMC3::operator<<;
  std::stringstream outstream;
  outstream << space;
  if ( ppsvc ) {
    if(auto tmp = ppsvc->find( LHCb::ParticleID( part->pdg_id() ) ); tmp){
        outstream << tmp->name();
    } else {
        outstream << "UNKNOWN(" << part->pdg_id() << ")";
    }
  } else {
    outstream << part->pdg_id();
  }
  outstream << " -> #" << part->id() << ", " << HepMC3::to_status_type(part->status()) << "\n";
  if ( part->end_vertex() ) {
    for ( auto p : part->children() ) {
      outstream << PrintDecay( p, level + 1, ppsvc );
    }
  }
  return outstream.str();
}
