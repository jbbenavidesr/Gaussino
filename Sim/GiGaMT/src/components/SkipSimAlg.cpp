// local
#include "SkipSimAlg.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"
#include "GiGaMTCore/Truth/GaussinoEventInformation.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

DECLARE_COMPONENT( SkipSimAlg )

G4EventProxies SkipSimAlg::operator()( const std::vector<HepMC::GenEvent>& hepmcevents ) const
{
  debug() << "==> Execute" << endmsg;
  G4EventProxies ret;
  G4EventProxy event{new G4Event{}, nullptr};
  auto converters = m_converterTool->BuildConverter( hepmcevents );
  Gaussino::MCTruthTrackerPtrs trackers;
  for ( auto& converter : converters ) {
    Gaussino::MCTruthTrackerPtr tracker = std::make_shared<Gaussino::MCTruthTracker>( std::move( *converter.get()), event.event()  );
    trackers.push_back( tracker );
    tracker->DumpToStream(debug(),   [&](int i){
        return m_ppSvc->find(LHCb::ParticleID(i))->name();
        } ) << endmsg;
  }
  event->SetUserInformation(new GaussinoEventInformation(trackers));
  ret.push_back( std::move( event ) );

  return ret;
}
