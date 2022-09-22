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
#include "SkipSimAlg.h"
#include "GiGaMTCoreRun/GaussinoEventInformation.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

DECLARE_COMPONENT( SkipSimAlg )

Gaussino::MCTruthPtrs SkipSimAlg::operator()( const HepMC3::GenEventPtrs& hepmcevents ) const
{
  Gaussino::MCTruthPtrs ret;
  auto converters = m_converterTool->BuildConverter( hepmcevents );
  Gaussino::MCTruthConverterPtr combined =
      Gaussino::MergeConverters( std::begin( converters ), std::end( converters ) );
  Gaussino::MCTruthTrackerPtr tracker = std::make_unique<Gaussino::MCTruthTracker>( std::move( *combined.get() ) );
  if ( msgLevel( MSG::DEBUG ) ) {
    tracker->DumpToStream( debug(), "",  [&]( int i ) -> std::string {
      if ( auto pid = m_ppSvc->find( LHCb::ParticleID( i ) ); pid ) {
        return pid->name();
      } else {
        return "UnknownToLHCb";
      }
    } ) << endmsg;
  }
  ret.emplace_back( new Gaussino::MCTruth( std::move( *tracker.get() ) ) );

  return ret;
}
