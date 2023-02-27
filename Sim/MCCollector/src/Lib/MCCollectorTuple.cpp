/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// MCCollector
#include "MCCollector/MCCollectorTuple.h"
// LHCb
#include "Event/MCExtendedHit.h"
void MCCollector::TupleAlg::fillCollectorTuple( const LHCb::MCHeader& evt, const LHCb::MCHits& collHits ) const {
  Tuple collT = nTuple( m_tupleName.value() );
  for ( const auto& collHit : collHits ) {
    auto extHit = dynamic_cast<LHCb::MCExtendedHit* const>( collHit );
    auto entry  = extHit->entry();
    collT->column( "Event_ID", evt.evtNumber() ).ignore();
    collT->column( "Entry_X", entry.X() ).ignore();
    collT->column( "Entry_Y", entry.Y() ).ignore();
    collT->column( "Entry_Z", entry.Z() ).ignore();
    collT->column( "Energy", extHit->energy() ).ignore();
    auto mom = extHit->momentum();
    collT->column( "Momentum", extHit->p() ).ignore();
    collT->column( "Momentum_X", mom.X() ).ignore();
    collT->column( "Momentum_Y", mom.Y() ).ignore();
    collT->column( "Momentum_Z", mom.Z() ).ignore();
    auto particle = extHit->mcParticle();
    collT->column( "Particle_Index", particle->index() ).ignore();
    collT->column( "Particle_PID", particle->particleID().pid() ).ignore();
    auto sc = collT->write();
    if ( sc.isFailure() ) { throw GaudiException( "NTuple not wrtitten!", name(), StatusCode::FAILURE ); }
    ++m_collHitsCounter;
  }
}
