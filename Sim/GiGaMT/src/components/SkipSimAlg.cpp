// local
#include "SkipSimAlg.h"
#include "GiGaMTCore/Truth/GaussinoEventInformation.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

DECLARE_COMPONENT( SkipSimAlg )

Gaussino::MCTruthPtrs SkipSimAlg::operator()( const std::vector<HepMC3::GenEvent>& hepmcevents ) const
{
  Gaussino::MCTruthPtrs ret;
  auto converters = m_converterTool->BuildConverter( hepmcevents );
  Gaussino::MCTruthConverterPtr combined =
      Gaussino::MergeConverters( std::begin( converters ), std::end( converters ) );
  Gaussino::MCTruthTrackerPtr tracker = std::make_unique<Gaussino::MCTruthTracker>( std::move( *combined.get() ) );
  if ( msgLevel( MSG::DEBUG ) ) {
    tracker->DumpToStream( debug(), [&]( int i ) -> std::string {
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
