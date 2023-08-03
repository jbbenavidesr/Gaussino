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
#include "ReDecaySkipSimAlg.h"
#include "Defaults/HepMCAttributes.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"
#include "HepMC3/Attribute.h"
#include "HepMC3/GenParticle.h"
#include "HepMCUser/ParticleAttribute.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

DECLARE_COMPONENT( ReDecaySkipSimAlg )

std::tuple<Gaussino::MCTruthPtrs, Gaussino::ReDecay::SignalTruths>
ReDecaySkipSimAlg::operator()( const HepMC3::GenEventPtrs& originalhepmcevents,
                               const HepMC3::GenEventPtrs& signalhepmcevents ) const {
  debug() << "==> Execute" << endmsg;
  auto& token = *m_tokenhandle.get();
  // Now we store a thread-local reference to this token in the service.
  // This enables us to get the current processing phase in any of the
  // subsequent tools without having to change all the interfaces again to explicitly pass
  // it around (this might be changed in the future)
  auto tokenguard = m_redecaysvc->setCurrentToken( token );
  auto engine     = createRndmEngine();
  // Get the individual components, make copies instead of reference because
  // we will modify the g4proxies in a moment.
  Gaussino::GiGaSimReturns ret_tuple;

  if ( m_redecaysvc->isCurrentOriginal() ) {
    if ( msgLevel( MSG::DEBUG ) ) { debug() << "Handling original event " << endmsg; }

    auto                          converters = m_converterTool->BuildConverter( originalhepmcevents );
    Gaussino::MCTruthConverterPtr combined =
        Gaussino::MergeConverters( std::begin( converters ), std::end( converters ) );

    Gaussino::MCTruthTrackerPtr tracker = std::make_unique<Gaussino::MCTruthTracker>( std::move( *combined.get() ) );

    std::get<1>( ret_tuple ).emplace_back( new Gaussino::MCTruth( std::move( *tracker.get() ) ) );
    m_redecaysvc->storeOriginalSimResult( token, ret_tuple );
  } else {
    // In this stip the containers are copied so we can safely append
    // the signal proxies for easier processing
    if ( msgLevel( MSG::DEBUG ) ) { debug() << "Retrieving original event " << endmsg; }
    ret_tuple = m_redecaysvc->getOriginalSimResult( token );
  }
  auto& [g4proxies, mctruths] = ret_tuple;
  // Now loop of all linked particles and find those starting a ReDecay tree.
  // Create a map HepMC -> LP for convenience further down.
  std::map<const HepMC3::GenParticle*, const LinkedParticle*> org_mapping;
  for ( auto& mctruth : mctruths ) {
    for ( auto& lp : mctruth->GetParticles() ) {
      if ( lp->GetType() == Gaussino::ConversionType::REDECAY ) { org_mapping[lp->HepMC().get()] = lp; }
    }
  }

  // Now process the redecay parts. Loop over the signal events and particle to
  // identify the root of a ReDecay tree. Then process the decay trees separately
  // to obtain one MCTruth per tree. This tree is stored in a map that links the
  // LinkedParticle in original MCTruth to the sub MCTruth to be used for this event.
  Gaussino::ReDecay::SignalTruths signal_truths;
  for ( auto& evt : signalhepmcevents ) {
    for ( auto& p : evt->particles() ) {
      if ( auto orgattr =
               p->attribute<HepMC3::ParticleAttribute>( Gaussino::HepMC::Attributes::ReDecayOriginalParticle );
           orgattr ) {
        auto org_part = orgattr->value();
        if ( org_mapping.find( org_part.get() ) == std::end( org_mapping ) ) {
          warning() << "ReDecay particle " << pid_to_name( p->pid() )
                    << " not in underlying event after simulation. Skipping decay tree." << endmsg;
          continue;
        }
        auto org_lp = org_mapping[org_part.get()];
        if ( msgLevel( MSG::DEBUG ) ) {
          debug() << "Going to simulate a redecayed decay for " << pid_to_name( p->pid() ) << endmsg;
        }
        auto                        converter  = m_converterTool->BuildConverter( p );
        Gaussino::MCTruthTrackerPtr tracker    = std::make_unique<Gaussino::MCTruthTracker>( std::move( *converter ) );
        Gaussino::MCTruthPtr        sigmctruth = std::make_shared<Gaussino::MCTruth>( std::move( *tracker ) );
        signal_truths[org_lp]                  = sigmctruth;
      }
    }
  }

  if ( msgLevel( MSG::DEBUG ) ) {
    if ( m_redecaysvc->isCurrentOriginal() ) {
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
    for ( auto& [lp, sigtracker] : signal_truths ) {
      debug() << "Attaching to " << *lp << endmsg;
      sigtracker->DumpToStream( debug(), "", [&]( int i ) -> std::string {
        if ( auto pid = m_ppSvc->find( LHCb::ParticleID( i ) ); pid ) {
          return pid->name();
        } else {
          return "UnknownToLHCb";
        }
      } ) << endmsg;
    }
  }
  return { std::move( mctruths ), std::move( signal_truths ) };
}
