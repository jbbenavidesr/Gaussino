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
#include "MinimalParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MinimalNGenParticles
//
// 2018-09-05 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( MinimalNGenParticles )

bool MinimalNGenParticles::studyFullEvent( const HepMC3::GenEventPtrs& theEvents,
                                           const LHCb::GenCollisions& ) const
{
  unsigned int counter = 0;
  for ( auto& evt : theEvents ) {
    counter += evt->particles().size();
  }
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Counted " << counter << " particles" << endmsg;
  }
  return counter >= m_minParticles;
}
