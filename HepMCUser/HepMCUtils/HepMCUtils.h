/*****************************************************************************\
* (c) Copyright 2000-2018 CERN for the benefit of the LHCb Collaboration      *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#ifndef GENEVENT_HEPMCUTILS_H
#define GENEVENT_HEPMCUTILS_H 1

// Include files
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

#include "Defaults/Enums.h"

#include<set>

/** @namespace HepMCUtils HepMCUtils.h GenEvent/HepMCUtils.h
 *
 *  Utility functions to use HepMC Events
 *
 *  @author Patrick Robbe
 *  @date   2006-02-14
 */

namespace HepMCUtils {
  /// Returns true if trees of vertices V1 and V2 belong to the same tree
  bool commonTrees( const HepMC::GenVertexPtr& V1, const HepMC::GenVertexPtr & V2 );

  /// Compare 2 HepMC GenParticle according to their barcode
  bool compareHepMCParticles( const HepMC::GenParticlePtr& part1, const HepMC::GenParticlePtr& part2 );

  /** Check if a particle is before or after oscillation.
   *  In HepMC description, the mixing is seen as a decay B0 -> B0bar. In this
   *  case, the B0 is said to be the BAtProduction contrary to the B0bar.
   *  @param[in] thePart  Particle to check.
   *  @return true if the particle is the particle before osillation.
   */
  bool IsBAtProduction( const HepMC::GenParticlePtr & thePart );

  /// Remove all daughters of a particle
  void RemoveDaughters( HepMC::GenParticlePtr& thePart );

  /// Comparison function as structure
  struct particleOrder {
    bool operator()( const HepMC::GenParticlePtr& part1, const HepMC::GenParticlePtr& part2 ) const {
      return ( part1->id() < part2->id() );
    }
  };

  /// Type of HepMC particles container ordered with barcodes
  typedef std::set<HepMC::GenParticle*, particleOrder> ParticleSet;
} // namespace HepMCUtils

//=============================================================================
// Inline functions
//=============================================================================

//=============================================================================
// Function to test if vertices are in the same decay family
//=============================================================================
inline bool HepMCUtils::commonTrees( const HepMC::GenVertexPtr & V1, const HepMC::GenVertexPtr& V2 ) {
  if ( !V2 ) return false;
  if ( !V1 ) return false;
  if ( V1 == V2 ) return true;
  for ( auto & iter : V1->particles( HepMC::ancestors) ) {
    if(auto & ev = iter->production_vertex(); ev){
      if ( V2 == ev ) return true;
    }
  }
  for ( auto & iter : V1->particles( HepMC::descendants) ) {
    if(auto & ev = iter->end_vertex(); ev){
      if ( V2 == ev ) return true;
    }
  }
  return false;
}

//=============================================================================
// Function to sort HepMC::GenParticles according to their barcode
//=============================================================================
inline bool HepMCUtils::compareHepMCParticles( const HepMC::GenParticlePtr& part1, const HepMC::GenParticlePtr& part2 ) {
  return ( part1->id() < part2->id() );
}

//=============================================================================
// Returns true if B is first B (removing oscillation B) and false
// if the B is the B after oscillation
//=============================================================================
inline bool HepMCUtils::IsBAtProduction( const HepMC::GenParticlePtr& thePart ) {
  if ( ( abs( thePart->pdg_id() ) != 511 ) && ( abs( thePart->pdg_id() ) != 531 ) ) return true;
  if ( !thePart->production_vertex() ) return true;
  HepMC::GenVertexPtr theVertex = thePart->production_vertex();
  if ( 1 != theVertex->particles_in_size() ) return true;
  HepMC::GenParticlePtr theMother = ( *theVertex->particles_in_const_begin() );
  if ( theMother->pdg_id() == -thePart->pdg_id() ) return false;
  return true;
}

//=============================================================================
// Erase the daughters of one particle
//=============================================================================
inline void HepMCUtils::RemoveDaughters( HepMC::GenParticlePtr& theParticle ) {
  if ( 0 == theParticle ) return;

  auto && EV = theParticle->end_vertex();

  if ( 0 == EV ) return;

  theParticle->set_status( Gaussino::GenStatus::StableInProdGen );
  HepMC::GenEvent* theEvent = theParticle->parent_event();

  std::vector<HepMC::GenVertex*>      tempList;
  HepMC::GenVertex::particle_iterator iterDes;

  tempList.push_back( EV );

  for ( iterDes = EV->particles_begin( HepMC::descendants ); iterDes != EV->particles_end( HepMC::descendants );
        ++iterDes ) {
    if ( 0 != ( *iterDes )->end_vertex() ) tempList.push_back( ( *iterDes )->end_vertex() );
  }

  std::vector<HepMC::GenVertex*>::iterator iter;
  for ( iter = tempList.begin(); iter != tempList.end(); ++iter ) {
    theEvent->remove_vertex( *iter );
    delete ( *iter );
  }
}

#endif // GENEVENT_HEPMCUTILS_H
