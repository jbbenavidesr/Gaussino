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
#include "MinimalStableInLHCb.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MinimalStableInLHCb
//
// 2018-09-05 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( MinimalStableInLHCb )

bool MinimalStableInLHCb::studyFullEvent( const HepMC3::GenEventPtrs& theEvents, const LHCb::GenCollisions& ) const
{
  unsigned int counter = 0;

  for ( auto& hepmcevent : theEvents ) {

    // Get the signal process ID from the attributes

    for ( auto& hepMCpart : hepmcevent->particles() ) {

      // Note that the following is really multiplicity of particle defined
      // as stable by Pythia just after hadronization: all particles known by
      // EvtGen are defined stable for Pythia (it will count rho and pi0
      // and gamma all togheter as stable ...
      if (!hepMCpart->end_vertex() ) {
        double pseudoRap = hepMCpart->momentum().pseudoRapidity();
        // in LHCb acceptance
        if ( ( pseudoRap > m_minEta ) && ( pseudoRap < m_maxEta ) ) {
          ++counter;
        }
      }
    }
  }
  return counter >= m_minParticles;
}
