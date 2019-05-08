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
#pragma once

// Include files
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Relatives.h"

#include "Defaults/Enums.h"

#include <set>

/** @namespace HepMCUtils HepMCUtils.h GenEvent/HepMCUtils.h
 *
 *  Utility functions to use HepMC Events
 *
 *  @author Patrick Robbe
 *  @date   2006-02-14
 */

namespace HepMCUtils
{
  /// Returns true if trees of vertices V1 and V2 belong to the same tree
  bool commonTrees( const HepMC3::ConstGenVertexPtr& V1, const HepMC3::ConstGenVertexPtr& V2 );

  /// Compare 2 HepMC GenParticle according to their barcode
  bool compareHepMCParticles( const HepMC3::GenParticlePtr& part1, const HepMC3::GenParticlePtr& part2 );
  bool compareConstHepMCParticles( const HepMC3::ConstGenParticlePtr& part1, const HepMC3::ConstGenParticlePtr& part2 );

  /** Check if a particle is before or after oscillation.
   *  In HepMC description, the mixing is seen as a decay B0 -> B0bar. In this
   *  case, the B0 is said to be the BAtProduction contrary to the B0bar.
   *  @param[in] thePart  Particle to check.
   *  @return true if the particle is the particle before osillation.
   */
  bool IsBAtProduction( const HepMC3::ConstGenParticlePtr& thePart );

  /// Remove all daughters of a particle
  void RemoveDaughters( HepMC3::GenParticlePtr& thePart );

  /// Comparison function as structure
  struct particleOrder {
    bool operator()( const HepMC3::GenParticle* part1, const HepMC3::GenParticle* part2 ) const
    {
      return ( part1->id() < part2->id() );
    }
  };

  /// Type of HepMC particles container ordered with barcodes
  typedef std::set<HepMC3::GenParticle*, particleOrder> ParticleSet;
} // namespace HepMCUtils

//=============================================================================
// Inline functions
//=============================================================================

//=============================================================================
// Function to test if vertices are in the same decay family
//=============================================================================
inline bool HepMCUtils::commonTrees( const HepMC3::ConstGenVertexPtr& V1, const HepMC3::ConstGenVertexPtr& V2 )
{
  if ( !V2 ) return false;
  if ( !V1 ) return false;
  if ( V1 == V2 ) return true;
  for ( auto& anc : HepMC3::Relatives::ANCESTORS( V1 ) ) {
    if ( auto pv = anc->production_vertex(); pv ) {
      if ( V2 == pv ) return true;
    }
  }
  for ( auto& desc : HepMC3::Relatives::DESCENDANTS( V1 ) ) {
    if ( auto ev = desc->end_vertex(); ev ) {
      if ( V2 == ev ) return true;
    }
  }
  return false;
}

//=============================================================================
// Function to sort HepMC3::GenParticles according to their barcode
//=============================================================================
inline bool HepMCUtils::compareHepMCParticles( const HepMC3::GenParticlePtr& part1,
                                               const HepMC3::GenParticlePtr& part2 )
{
  return ( part1->id() < part2->id() );
}
inline bool HepMCUtils::compareConstHepMCParticles( const HepMC3::ConstGenParticlePtr& part1,
                                               const HepMC3::ConstGenParticlePtr& part2 )
{
  return ( part1->id() < part2->id() );
}

//=============================================================================
// Returns true if B is first B (removing oscillation B) and false
// if the B is the B after oscillation
//=============================================================================
inline bool HepMCUtils::IsBAtProduction( const HepMC3::ConstGenParticlePtr& thePart )
{
  if ( ( abs( thePart->pdg_id() ) != 511 ) && ( abs( thePart->pdg_id() ) != 531 ) ) return true;
  if ( !thePart->production_vertex() ) return true;
  HepMC3::ConstGenVertexPtr theVertex = thePart->production_vertex();
  if ( 1 != theVertex->particles_in().size() ) return true;
  HepMC3::ConstGenParticlePtr theMother = ( *std::begin( theVertex->particles_in() ) );
  if ( theMother->pdg_id() == -thePart->pdg_id() ) return false;
  return true;
}

//=============================================================================
// Erase the daughters of one particle
//=============================================================================
inline void HepMCUtils::RemoveDaughters( HepMC3::GenParticlePtr& theParticle )
{
  if ( 0 == theParticle ) return;

  auto&& EV = theParticle->end_vertex();

  if ( 0 == EV ) return;

  theParticle->set_status( Gaussino::GenStatus::StableInProdGen );
  HepMC3::GenEvent* theEvent = theParticle->parent_event();

  std::vector<HepMC3::GenVertexPtr> tempList;

  tempList.push_back( EV );

  for ( auto& part : HepMC3::Relatives::DESCENDANTS( EV ) ) {
    if ( 0 != part->end_vertex() ) tempList.push_back( part->end_vertex() );
  }

  for ( auto& iter : tempList ) {
    theEvent->remove_vertex( iter );
  }
}
