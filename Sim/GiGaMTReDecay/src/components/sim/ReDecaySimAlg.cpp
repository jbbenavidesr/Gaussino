// local
#include "ReDecaySimAlg.h"
#include "Defaults/HepMCAttributes.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"
#include "HepMC3/Attribute.h"
#include "HepMC3/GenParticle.h"
#include "HepMCUser/ParticleAttribute.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

DECLARE_COMPONENT( ReDecaySimAlg )

std::tuple<G4EventProxies, Gaussino::MCTruthPtrs, Gaussino::ReDecay::SignalTruths> ReDecaySimAlg::
                                                                                   operator()( const HepMC3::GenEventPtrs& originalhepmcevents, const HepMC3::GenEventPtrs& signalhepmcevents ) const {
  debug() << "==> Execute" << endmsg;
  auto& token = *m_tokenhandle.get();
  // Now we store a thread-local reference to this token in the service.
  // This enables us to get the current processing phase in any of the
  // subsequent tools without having to change all the interfaces again to explicitly pass
  // it around (this might be changed in the future)
  auto tokenguard = m_redecaysvc->setCurrentToken( token );
  auto engine = createRndmEngine();
  // Get the individual components, make copies instead of reference because
  // we will modify the g4proxies in a moment.
  Gaussino::GiGaSimReturns ret_tuple{};

  if ( m_redecaysvc->isCurrentOriginal() ) {
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Simulating original event " << endmsg;
    }
    ret_tuple = m_gigaSvc->simulate( originalhepmcevents, engine );
    m_redecaysvc->storeOriginalSimResult( token, ret_tuple );
  } else {
    // In this stip the containers are copied so we can safely append
    // the signal proxies for easier processing
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Retrieving original event " << endmsg;
    }
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
        auto [g4proxy, sigmctruth] = m_gigaSvc->simulateDecay( p, engine );
        g4proxies.push_back( g4proxy );
        signal_truths[org_lp] = sigmctruth;
      }
    }
  }

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
    for ( auto& [lp, sigtracker] : signal_truths) {
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
  return {std::move( g4proxies ), std::move( mctruths ), std::move( signal_truths )};
}
