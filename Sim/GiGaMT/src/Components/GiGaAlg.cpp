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
// local
#include "GiGaAlg.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

DECLARE_COMPONENT( GiGaAlg )

std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> GiGaAlg::operator()( const HepMC3::GenEventPtrs& hepmcevents ) const {
  auto engine = createRndmEngine();

  debug() << "==> Execute" << endmsg;
  auto ret_tuple = m_gigaSvc->simulate( hepmcevents, engine );

  if ( msgLevel( MSG::DEBUG ) ) {
    auto& trackers = std::get<Gaussino::MCTruthPtrs>( ret_tuple );
    for ( auto& tracker : trackers ) {
      tracker->DumpToStream( debug(), "", [&]( int i ) -> std::string {
        if ( auto pid = m_ppSvc->find( LHCb::ParticleID( i ) ); pid ) {
          return pid->name();
        } else {
          return "UnknownToLHCb";
        }
      } ) << endmsg;
    }
  }
  return ret_tuple;
}
