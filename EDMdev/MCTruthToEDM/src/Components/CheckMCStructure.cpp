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
// Include files

// from Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "CheckMCStructure.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
//-----------------------------------------------------------------------------
// Implementation file for class : CheckMCStructure
//
//
// 2016-03-30 : Dominik Muller
//-----------------------------------------------------------------------------

int CheckMCStructure::printMCParticlesTree( LHCb::MCVertex* vtx, unsigned int& n_particles, unsigned int& n_vertices,
                                            int level, int counter ) const {
  n_vertices++;
  ;
  std::string spacer = "|---";
  std::string space  = "";
  for ( int i = 0; i < level; i++ ) { space += spacer; }
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << counter << " " << space << " [VertexType = " << vtx->type() << "] Pos: " << vtx->position()
            << " Time: " << vtx->time() << endmsg;
  }
  auto prds = vtx->products();
  for ( auto& part : prds ) {
    // Verify that particle's origin vertex is set consistent
    if ( part->originVertex() != vtx ) { error() << "Inconsistent origin vertex link of particle!" << endmsg; }
    n_particles++;
    if ( msgLevel( MSG::DEBUG ) ) {
      auto        partprop = m_ppSvc->find( part->particleID() );
      std::string name;
      if ( partprop ) {
        name = partprop->name();
      } else {
        std::stringstream st;
        st << "Unknown(" << part->particleID().pid() << ")";
        name = st.str();
      }

      debug() << counter << " " << space << " " << name << " (PT, Eta) = (" << part->pt() << ", "
              << part->pseudoRapidity() << ")" << endmsg;
    }
    counter++;
    auto evs = part->endVertices();
    for ( auto& ev : evs ) {
      // Verify the link back to mother was correctly set as well
      if ( ev->mother() != part ) { error() << "Inconsistent mother link of endVertex!" << endmsg; }
      part->mother();
      counter = printMCParticlesTree( ev, n_particles, n_vertices, level + 1, counter );
    }
  }
  return counter;
}

// Declaration of the Algorithm Factory
DECLARE_COMPONENT( CheckMCStructure )

//=============================================================================
// Main execution
//=============================================================================
void CheckMCStructure::operator()( const LHCb::MCParticles& mcparticles, const LHCb::MCVertices& mcvertices,
                                   const LHCb::MCHeader& mcheader ) const {
  unsigned int n_particles = 0;
  unsigned int n_vertices  = 0;
  auto         pvs         = mcheader.primaryVertices();
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Event has " << pvs.size() << " primary vertices" << endmsg;
    debug() << "Event has " << mcparticles.size() << " MCParticles. Will print tarting from the PVs." << endmsg;
  }
  int iPV = 0;
  for ( auto& pv : pvs ) {
    if ( msgLevel( MSG::DEBUG ) ) { debug() << "Starting from PV #" << iPV++ << endmsg; }
    printMCParticlesTree( pv, n_particles, n_vertices );
  }

  if ( n_vertices != mcvertices.size() ) {
    error() << "Could not reach all MCVertices from the PV " << n_vertices << "/" << mcvertices.size() << endmsg;
  } else {
    if ( msgLevel( MSG::DEBUG ) ) { debug() << "All MCVertices are reachable!" << endmsg; }
  }
  if ( n_particles != mcparticles.size() ) {
    error() << "Could not reach all MCParticles from the PV " << n_particles << "/" << mcparticles.size() << endmsg;
  } else {
    if ( msgLevel( MSG::DEBUG ) ) { debug() << "All MCParticles are reachable!" << endmsg; }
  }
}

//=============================================================================
