#include "Generators/Generation.h"
#include "GiGaMTReDecay/Token.h"
#include "GiGaMTReDecay/IRedecaySvc.h"
#include "GiGaMTReDecay/IReDecaySorter.h"

#include "NewRnd/RndGlobal.h"

/** @class Generation Generation.h "Generation.h"
 *
 *  ReDecay signal generation algorithm. Essentially does the same thing as the nominal Generation
 *  algorithm with an additional DataHandle to depend on the rest of the event for scheduling.
 *
 *  @author Dominik Muller
 *  @date 2018-03-16
 */

class ReDecaySignalGeneration: public Generation{
  public:
  using Generation::Generation;
  using Generation::initialize;
  using Generation::finalize;

  virtual std::tuple<std::vector<HepMC3::GenEventPtr>, LHCb::GenCollisions, LHCb::GenHeader>
  operator()( const LHCb::GenHeader& ) const override;

  private:
  AnyDataHandle<Gaussino::ReDecay::Token> m_tokenhandle{Gaussino::ReDecayToken::Default, Gaudi::DataHandle::Reader, this};
  AnyDataHandle<std::vector<HepMC3::GenEventPtr>> m_originaleventhandle{this, Gaudi::DataHandle::Reader, "OriginalEventLocation", Gaussino::HepMCEventLocation::Default};
  ServiceHandle<IReDecaySvc> m_redecaysvc{this, "ReDecaySvc", "ReDecaySvc"};
};

DECLARE_COMPONENT( ReDecaySignalGeneration )




std::tuple<std::vector<HepMC3::GenEventPtr>, LHCb::GenCollisions, LHCb::GenHeader> ReDecaySignalGeneration::
operator()( const LHCb::GenHeader& old_gen_header) const {
  auto engine = createRndmEngine();
  ThreadLocalEngine::Guard rnd_guard(engine);
  auto & token = *m_tokenhandle.get();
  auto tokenguard = m_redecaysvc->setCurrentToken(token);
  return callOperatorImplementation(old_gen_header, engine);
}
