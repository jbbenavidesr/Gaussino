// local
#include "GiGaAlg.h"
#include "GiGaMT/IGiGaMTSvc.h"

DECLARE_COMPONENT( GiGaAlg )

StatusCode GiGaAlg::initialize()
{
  StatusCode sc = Consumer::initialize();
  if ( sc.isFailure() ) return sc;
  debug() << "==> Initialize" << endmsg;

  m_gigaSvc = svc<IGiGaMTSvc>(m_gigaMTname);
  if(!m_gigaSvc){
    return Error("Could not find GiGaMT service instance");
  }
  return StatusCode::SUCCESS;
}

void GiGaAlg::operator()( const std::vector<HepMC::GenEvent>& hepmcevents ) const
{
  auto engine = createRndmEngine();
  debug() << "==> Execute" << endmsg;
  m_gigaSvc->simulate(hepmcevents, engine);

}

StatusCode GiGaAlg::finalize()
{
  debug() << "==> Finalize" << endmsg;
  m_gigaSvc->release();
  return Consumer::finalize();
}
