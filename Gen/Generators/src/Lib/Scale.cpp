// $Id: Scale.cpp,v 1.2 2008-07-24 22:05:38 robbep Exp $
//
// ============================================================================
// Include files 
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
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
void GeneratorUtils::scale
( HepMC::GenEvent* event , 
  const double     mom   , 
  const double     time  ) 
{
  if ( nullptr == event ) { return ; }
  // rescale particles   
  for ( auto & p : event->particles() ) 
  {
    if ( p ) { 
      p->set_momentum( HepMC::FourVector( p->momentum().px() * mom ,
                                          p->momentum().py() * mom , 
                                          p->momentum().pz() * mom , 
                                          p->momentum().e() * mom ) ) ; 
    }  
  }
  // rescale vertices 
  for ( auto & v : event->vertices() ) 
  {
    if ( ! v ) { continue ; }
    HepMC::FourVector newPos = v->position() ;
    newPos.setT ( newPos.t() * time ) ;
    v->set_position ( newPos ) ;
  }   
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

