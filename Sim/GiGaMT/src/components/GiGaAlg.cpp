// local
#include "GiGaAlg.h"

DECLARE_COMPONENT( GiGaAlg )

G4EventProxies GiGaAlg::operator()( const std::vector<HepMC::GenEvent>& hepmcevents ) const
{
  auto engine = createRndmEngine();
  debug() << "==> Execute" << endmsg;
  return m_gigaSvc->simulate( hepmcevents, engine );
}
