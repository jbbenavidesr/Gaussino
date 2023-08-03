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
// $Id: Scale.cpp,v 1.2 2008-07-24 22:05:38 robbep Exp $
//
// ============================================================================
// Include files
// ============================================================================
// HepMC
// ============================================================================
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
// ============================================================================
// Local
// ============================================================================
#include "Generators/Scale.h"
// ============================================================================
/** @file
 *  implementation of simple functon to rescale
 *  HepMCEvent in between Pythia and LHCb units
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-05
 */
// ============================================================================
void GeneratorUtils::scale( HepMC3::GenEvent* event, const double mom, const double time ) {
  if ( nullptr == event ) { return; }
  // rescale particles
  for ( auto& p : event->particles() ) {
    if ( p ) {
      p->set_momentum( HepMC3::FourVector( p->momentum().px() * mom, p->momentum().py() * mom, p->momentum().pz() * mom,
                                           p->momentum().e() * mom ) );
    }
  }
  // rescale vertices
  for ( auto& v : event->vertices() ) {
    if ( !v ) { continue; }
    HepMC3::FourVector newPos = v->position();
    newPos.setT( newPos.t() * time );
    v->set_position( newPos );
  }
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
