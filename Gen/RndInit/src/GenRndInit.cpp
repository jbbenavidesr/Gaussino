// Include files
#include <cmath>

// local
#include "GaudiKernel/ConcurrencyFlags.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GenRndInit.h"

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
  info() << "Setting barrier sync for " << Gaudi::Concurrency::ConcurrencyFlags::numThreads() << endmsg;
  m_barrier    = new MTBarrier( Gaudi::Concurrency::ConcurrencyFlags::numThreads() - 1 );
  m_endbarrier = new MTBarrier( Gaudi::Concurrency::ConcurrencyFlags::numThreads() - 1 );
  return StatusCode::SUCCESS;
}

LHCb::GenHeader GenRndInit::operator()() const
{
  debug() << "==> Execute" << endmsg;

  // Initialize the random number
  longlong eventNumber = m_firstEvent - 1 + this->increaseEventCounter();
  if ( eventNumber == m_firstTimingEvent ) {
    // Initialising the start time for more precise monitoring
    // when the event loop is in full swing.
    // m_start_time.
    debug() << "Hit it. Waiting at first barrier" << endmsg;
    m_barrier->wait();
    m_wait_at_barrier = false;
    m_start_time = Clock::now();
    info() << "Started loop timing!" << endmsg;
  } else if ( eventNumber > m_firstTimingEvent && m_wait_at_barrier ) {
    debug() << "Larger. Waiting at first barrier" << endmsg;
    m_barrier->wait();
    m_wait_at_barrier = false;
  }
  if ( m_lastTimingEvent > 0 && eventNumber == m_lastTimingEvent ) {
    // Initialising the start time for more precise monitoring
    // when the event loop is in full swing.
    // m_start_time.
    debug() << "Hit it. Waiting at end barrier" << endmsg;
    m_endbarrier->wait();
    m_wait_at_endbarrier = false;
    auto end_time = Clock::now();
    info() << "Measured event loop time [ns]: "
           << std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - m_start_time ).count() << endmsg;
  } else if ( m_lastTimingEvent > 0 && eventNumber > m_lastTimingEvent && m_wait_at_endbarrier ) {
    debug() << "Larger. Waiting at end barrier" << endmsg;
    m_endbarrier->wait();
    m_wait_at_endbarrier = false;
  }

  // Configure the event information in the event context
  auto context = Gaudi::Hive::currentContext();
  EventIDBase eventid{};
  eventid.set_event_number( eventNumber );
  eventid.set_run_number( m_runNumber );
  context.setEventID( eventid );

  printEventRun( eventNumber, m_runNumber );

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

StatusCode GenRndInit::finalize()
{
  delete m_barrier;
  delete m_endbarrier;
  auto end_time = Clock::now();
  info() << "Total event loop time [ns]: "
         << std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - m_start_time ).count() << endmsg;
  return base_class::finalize();
}

void GenRndInit::printEventRun( long long event, int run, std::vector<long int>* seeds ) const
{
  info() << "Evt " << event << ",  Run " << run;
  info() << ",  Nr. in job = " << eventCounter();
  if ( 0 != seeds ) info() << " with seeds " << *seeds;
  info() << endmsg;
}
