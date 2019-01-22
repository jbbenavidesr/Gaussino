#include "NewRnd/RndAlgSeeder.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <type_traits>


HepRandomEnginePtr RndAlgSeeder::createRndmEngine() const
{
  HepRandomEnginePtr ret_ptr{m_engine_tool->construct(), m_engine_tool.get(), name()};
  auto[event_number, run_number] = *m_forseed.get();
  RndCommon::seedEngine(ret_ptr, event_number, run_number, name());

  return ret_ptr;
}

#include "CLHEP/Random/MixMaxRng.h"
#include "CLHEP/Random/RanluxEngine.h"

template <typename ENGINE>
class CLHEP_ENGINE : public extends<GaudiTool, IExtEngine>
{
  public:
  static_assert( std::is_base_of<CLHEP::HepRandomEngine, ENGINE>::value,
                 "Random engine must inherit from CLHEP::HepRandomEngine" );
  using extends::extends;
  virtual CLHEP::HepRandomEngine* construct() const override { return new ENGINE{}; }
};

typedef CLHEP_ENGINE<CLHEP::MixMaxRng> MixMaxRng;
DECLARE_COMPONENT_WITH_ID( MixMaxRng, "MixMaxRng" )
typedef CLHEP_ENGINE<CLHEP::RanluxEngine> RanluxEngine;
DECLARE_COMPONENT_WITH_ID( RanluxEngine, "RanluxEngine" )
