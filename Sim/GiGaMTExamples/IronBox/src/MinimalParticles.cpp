#include "MinimalParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MinimalNGenParticles
//
// 2018-09-05 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( MinimalNGenParticles )

bool MinimalNGenParticles::studyFullEvent( std::vector<HepMC::GenEvent>& theEvents, LHCb::GenCollisions& ) const
{
  unsigned int counter = 0;
  for ( auto& evt : theEvents ) {
    counter += evt.particles().size();
  }
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Counted " << counter << " particles" << endmsg;
  }
  return counter >= m_minParticles;
}
