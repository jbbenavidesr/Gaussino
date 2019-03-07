// local
#include "SkipSimAlg.h"
#include "GiGaMTCore/Truth/GaussinoEventInformation.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

DECLARE_COMPONENT( SkipSimAlg )

G4EventProxies SkipSimAlg::operator()( const std::vector<HepMC::GenEvent>& hepmcevents ) const
{
  G4EventProxies ret;
  G4EventProxy event{new G4Event{}, nullptr};
  auto converters = m_converterTool->BuildConverter( hepmcevents );
  Gaussino::MCTruthConverterPtr combined = Gaussino::MergeConverters(std::begin(converters), std::end(converters));
  Gaussino::MCTruthTrackerPtr tracker =
      std::make_shared<Gaussino::MCTruthTracker>( std::move( *combined.get() ), event.event() );
  tracker->DumpToStream( debug(), [&]( int i ) { return m_ppSvc->find( LHCb::ParticleID( i ) )->name(); } ) << endmsg;
  event->SetUserInformation( new GaussinoEventInformation( tracker ) );
  ret.push_back( std::move( event ) );

  return ret;
}
