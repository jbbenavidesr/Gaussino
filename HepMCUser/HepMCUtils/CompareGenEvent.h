//--------------------------------------------------------------------------
#ifndef HEPMC_COMPARE_GENEVENT_H
#define HEPMC_COMPARE_GENEVENT_H

/////////////////////////////////////////////////////////////////////////
// CompareGenEvent.h
//
// garren@fnal.gov, January 2008
// Free functions used to compare two copies of GenEvent
//////////////////////////////////////////////////////////////////////////
//

#include <iostream>

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "range/v3/all.hpp"

namespace HepMC {


void printChildren(HepMC::GenParticlePtr part, int level=0);
bool compareGenEvent( const GenEvent &, const GenEvent & );
bool compareSignalProcessVertex( const GenEvent &, const GenEvent & );
bool compareBeamParticles( const GenEvent &, const GenEvent & );
bool compareWeights( const GenEvent &, const GenEvent & );
bool compareVertices( const GenEvent &, const GenEvent & );
bool compareParticles( const GenEvent &, const GenEvent & );
bool compareVertex( const GenVertex & v1, const GenVertex & v2 );
bool compareParticle( const GenParticle & p1, const GenParticle & p2 );

} // HepMC

inline bool operator==(const HepMC::GenEvent & left, const HepMC::GenEvent & right ){
  return HepMC::compareGenEvent(left, right);
}

inline bool operator==(const HepMC::GenVertex & left, const HepMC::GenVertex & right ){
  return HepMC::compareVertex(left, right);
}

inline bool operator==(const HepMC::GenParticle & left, const HepMC::GenParticle & right ){
  return HepMC::compareParticle(left, right);
}

// And the unequality operators
//
inline bool operator!=(const HepMC::GenEvent & left, const HepMC::GenEvent & right ){
  return !(left == right);
}

inline bool operator!=(const HepMC::GenVertex & left, const HepMC::GenVertex & right ){
  return !(left == right);
}

inline bool operator!=(const HepMC::GenParticle & left, const HepMC::GenParticle & right ){
  return !(left == right);
}

inline std::ostream & operator<<(std::ostream & strm, const HepMC::GenParticle & particle){
  strm << "PDG ID " << particle.pdg_id() << " #" << particle.id() << "\n";
  strm << "   Momentum [ " << particle.momentum().px() << ", " << particle.momentum().py() << ", "<< particle.momentum().pz() << ", "<< particle.momentum().e() << "]\n";
  return strm;
}

inline std::ostream & operator<<(std::ostream & strm, const HepMC::GenVertex & vertex){
  strm << " Vertex at [ " << vertex.position().x() << ", "<< vertex.position().y() << ", "<< vertex.position().z() << ", "<< vertex.position().t() << "]\n";
  return strm;
}

#endif  // HEPMC_COMPARE_GENEVENT_H
//--------------------------------------------------------------------------
