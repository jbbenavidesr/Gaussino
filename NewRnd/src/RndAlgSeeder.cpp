#include "NewRnd/RndAlgSeeder.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <type_traits>


HepRandomEnginePtr RndAlgSeeder::createRndmEngine() const
{
  HepRandomEnginePtr ret_ptr{m_engine_tool.get()->construct(), m_engine_tool.get(), name()};
  if(m_forcedSeed != 0){
    ret_ptr->setSeed(m_forcedSeed,0);
  } else {
    auto[event_number, run_number] = *m_forseed.get();
    auto seeds = RndCommon::seedEngine(ret_ptr, event_number, run_number, name());
    debug() << "Seeds: " << seeds << endmsg;
  }
  return ret_ptr;
}
