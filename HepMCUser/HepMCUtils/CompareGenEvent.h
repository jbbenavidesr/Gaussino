#pragma once

/////////////////////////////////////////////////////////////////////////
// CompareGenEvent.h
//
// garren@fnal.gov, January 2008
// Free functions used to compare two copies of GenEvent
//////////////////////////////////////////////////////////////////////////
//

#include <iostream>

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "range/v3/all.hpp"

namespace HepMC3 {


void printChildren(GenParticlePtr part, int level=0);
bool compareGenEvent( const GenEvent &, const GenEvent & );
bool compareSignalProcessVertex( const GenEvent &, const GenEvent & );
bool compareBeamParticles( const GenEvent &, const GenEvent & );
bool compareWeights( const GenEvent &, const GenEvent & );
bool compareVertices( const GenEvent &, const GenEvent & );
bool compareParticles( const GenEvent &, const GenEvent & );
bool compareVertex( const GenVertex & v1, const GenVertex & v2 );
bool compareParticle( const GenParticle & p1, const GenParticle & p2 );

} // HepMC

inline bool operator==(const HepMC3::GenEvent & left, const HepMC3::GenEvent & right ){
  return HepMC3::compareGenEvent(left, right);
}

inline bool operator==(const HepMC3::GenVertex & left, const HepMC3::GenVertex & right ){
  return HepMC3::compareVertex(left, right);
}

inline bool operator==(const HepMC3::GenParticle & left, const HepMC3::GenParticle & right ){
  return HepMC3::compareParticle(left, right);
}

// And the unequality operators
//
inline bool operator!=(const HepMC3::GenEvent & left, const HepMC3::GenEvent & right ){
  return !(left == right);
}

inline bool operator!=(const HepMC3::GenVertex & left, const HepMC3::GenVertex & right ){
  return !(left == right);
}

inline bool operator!=(const HepMC3::GenParticle & left, const HepMC3::GenParticle & right ){
  return !(left == right);
}

inline std::ostream & operator<<(std::ostream & strm, const HepMC3::GenParticle & particle){
  strm << "PDG ID " << particle.pdg_id() << " #" << particle.id() << "\n";
  strm << "   Momentum [ " << particle.momentum().px() << ", " << particle.momentum().py() << ", "<< particle.momentum().pz() << ", "<< particle.momentum().e() << "]\n";
  return strm;
}

inline std::ostream & operator<<(std::ostream & strm, const HepMC3::GenVertex & vertex){
  strm << " Vertex at [ " << vertex.position().x() << ", "<< vertex.position().y() << ", "<< vertex.position().z() << ", "<< vertex.position().t() << "]\n";
  return strm;
}
