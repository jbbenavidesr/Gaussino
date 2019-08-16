// local
#include "GiGaAlg.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"

DECLARE_COMPONENT( GiGaAlg )

std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> GiGaAlg::operator()( const std::vector<HepMC3::GenEvent>& hepmcevents ) const
{
  auto engine = createRndmEngine();

  debug() << "==> Execute" << endmsg;
  return m_gigaSvc->simulate(m_converterTool->BuildConverter(hepmcevents), engine );
}
