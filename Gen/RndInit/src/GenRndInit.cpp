// Include files
#include <cmath>

// local
#include "GenRndInit.h"
#include "RndInit/IRndSeedingTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenRndInit
//
// 2018-01-29: Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_COMPONENT(GenRndInit)

StatusCode GenRndInit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_rndtool = tool< IRndSeedingTool>( m_RndInitToolName, this );

  if (sc.isFailure())
    return Error(" Fatal error while retrieving Property EvtMax ");
  auto appMgr = service("ApplicationMgr");
  auto propMgr = appMgr.as<IProperty>();
  std::string value;
  sc = propMgr->getProperty("EvtMax", value);

  m_eventMax = std::atoi(value.c_str());
  debug() << "Retrieved EvtMax = " << m_eventMax << endmsg;
  return StatusCode::SUCCESS;
}

  LHCb::GenHeader
  GenRndInit::operator()() const {
  debug() << "==> Execute" << endmsg;

  // Initialize the random number
  longlong eventNumber = m_firstEvent - 1 + this->increaseEventCounter();

  auto seeds = std::make_shared<std::vector<long int>>();
  m_rndtool->seed(m_runNumber, eventNumber, seeds);
  // std::vector<long int> seeds = getSeeds( m_runNumber, eventNumber );
  // sc = this->initRndm( seeds );
  // if ( sc.isFailure() ) return sc;  // error printed already by initRndm
  printEventRun( eventNumber, m_runNumber, seeds.get());

  // Create GenHeader and partially fill it - updated during phase execution
  LHCb::GenHeader header{};
  // FIXME: Application name
  // header->setApplicationName( this->appName() );
  // FIXME: Application Version
  // header->setApplicationVersion( this->appVersion() );
  header.setRunNumber(m_runNumber);
  header.setEvtNumber(eventNumber);
  header.setEvType(0);

  return header;
}

void GenRndInit::printEventRun(long long event, int run,
                               std::vector<long int>* seeds) const {
  info() << "Evt " << event << ",  Run " << run;
  info() << ",  Nr. in job = " << eventCounter();
  if (0 != seeds) info() << " with seeds " << *seeds;
  info() << endmsg;
}
