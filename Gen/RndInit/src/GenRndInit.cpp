// Include files
#include <cmath>

// local
#include "GenRndInit.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenRndInit
//
// 2018-01-29: Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_COMPONENT( GenRndInit )

StatusCode GenRndInit::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( sc.isFailure() ) return Error( " Fatal error while retrieving Property EvtMax " );
  auto appMgr  = service( "ApplicationMgr" );
  auto propMgr = appMgr.as<IProperty>();
  std::string value;
  sc = propMgr->getProperty( "EvtMax", value );

  m_eventMax = std::atoi( value.c_str() );
  debug() << "Retrieved EvtMax = " << m_eventMax << endmsg;
  return StatusCode::SUCCESS;
}

LHCb::GenHeader GenRndInit::operator()() const
{
  debug() << "==> Execute" << endmsg;

  // Initialize the random number
  longlong eventNumber = m_firstEvent - 1 + this->increaseEventCounter();

  // Configure the event information in the event context
  auto context = Gaudi::Hive::currentContext();
  EventIDBase eventid{};
  eventid.set_event_number(eventNumber);
  eventid.set_run_number(m_runNumber);
  context.setEventID(eventid);

  printEventRun( eventNumber, m_runNumber);

  // Create GenHeader and partially fill it - updated during phase execution
  LHCb::GenHeader header{};
  // FIXME: Application name
  // header->setApplicationName( this->appName() );
  // FIXME: Application Version
  // header->setApplicationVersion( this->appVersion() );
  header.setRunNumber( m_runNumber );
  header.setEvtNumber( eventNumber );
  header.setEvType( 0 );

  return header;
}

void GenRndInit::printEventRun( long long event, int run, std::vector<long int>* seeds ) const
{
  info() << "Evt " << event << ",  Run " << run;
  info() << ",  Nr. in job = " << eventCounter();
  if ( 0 != seeds ) info() << " with seeds " << *seeds;
  info() << endmsg;
}
