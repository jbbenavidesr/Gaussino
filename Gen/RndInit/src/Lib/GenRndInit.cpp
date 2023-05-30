/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// Include files
#include <cmath>

// local
#include "GaudiKernel/ConcurrencyFlags.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "RndInit/GenRndInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenRndInit
//
// 2018-01-29: Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

StatusCode GenRndInit::initialize() {
  return Producer::initialize().andThen( [&] {
    auto        appMgr  = service( "ApplicationMgr" );
    auto        propMgr = appMgr.as<IProperty>();
    std::string value;
    if ( propMgr->getProperty( "EvtMax", value ).isFailure() ) {
      error() << "Error fetching EvtMax property from the ApplicationMgr";
      return StatusCode::FAILURE;
    }
    m_eventMax = std::atoi( value.c_str() );
    debug() << "Retrieved EvtMax = " << m_eventMax << endmsg;
    info() << "Setting barrier sync for " << Gaudi::Concurrency::ConcurrencyFlags::numThreads() << endmsg;
    m_barrier = new MTBarrier( Gaudi::Concurrency::ConcurrencyFlags::numThreads() );
    return StatusCode::SUCCESS;
  } );
}

std::tuple<LHCb::GenHeader, LHCb::BeamParameters, LHCb::ODIN> GenRndInit::operator()() const {
  debug() << "==> Execute" << endmsg;
  // Initialize the random number
  auto eventNumber   = m_firstEvent - 1 + this->increaseEventCounter();
  auto barrier_event = m_firstEvent.value() + m_firstTimingEvent.value();

  if ( m_firstTimingEvent.value() > 0 ) {
    if ( eventNumber == barrier_event ) {
      debug() << "Organising timing" << endmsg;
      // Initialising the start time for more precise monitoring
      // when the event loop is in full swing.
      m_barrier->wait();
      m_wait_at_barrier = false;
      m_start_time      = Clock::now();
      info() << "Started loop timing!" << endmsg;
    } else if ( eventNumber > barrier_event && m_wait_at_barrier ) {
      m_barrier->wait();
      m_wait_at_barrier = false;
    }
  }
  if ( eventNumber >= barrier_event ) { m_evtTimingCounter++; }

  auto mem = System::virtualMemory() / 1000.;
  m_totMem += mem;
  m_totMemPerThread += mem / Gaudi::Concurrency::ConcurrencyFlags::numThreads();

  // Configure the event information in the event context
  // Places the event and run number onto the TES for other algorithms
  // to access when configuring their random engines.
  SetSeedPair( eventNumber, m_runNumber.value() );

  printEventRun( eventNumber, m_runNumber.value() );

  // Create GenHeader and partially fill it - updated during phase execution
  LHCb::GenHeader header{};
  // FIXME: Application name
  // header->setApplicationName( this->appName() );
  // FIXME: Application Version
  // header->setApplicationVersion( this->appVersion() );
  header.setRunNumber( m_runNumber.value() );
  header.setEvtNumber( eventNumber );
  header.setEvType( 0 );
  auto beam = createBeamParameters();

  // Create ODIN
  LHCb::ODIN odin{};
  // Fill ODIN from event header
  odin.setRunNumber( m_runNumber.value() );
  odin.setEventNumber( eventNumber );

  return std::make_tuple( header, beam, odin );
}

StatusCode GenRndInit::finalize() {
  delete m_barrier;
  if ( m_firstTimingEvent >= 0 ) {
    auto end_time = Clock::now();
    info() << m_evtCounter << " events processed" << endmsg;
    info() << "Measured event loop time (" << m_evtTimingCounter
           << ") [ns]: " << std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - m_start_time ).count()
           << endmsg;
    info() << "Time per event: "
           << std::chrono::duration_cast<std::chrono::seconds>( end_time - m_start_time ).count() /
                  (double)m_evtTimingCounter
           << " seconds." << endmsg;
  }
  return Producer::finalize();
}

void GenRndInit::printEventRun( long long event, int run, std::vector<long int>* seeds ) const {
  info() << "Evt " << event << ",  Run " << run;
  info() << ",  Nr. in job = " << eventCounter();
  if ( 0 != seeds ) info() << " with seeds " << *seeds;
  info() << endmsg;
}

LHCb::BeamParameters GenRndInit::createBeamParameters() const {
  LHCb::BeamParameters ret{};
  // create beam parameter object
  ret.setEnergy( m_beamInfoSvc->energy() );
  ret.setSigmaS( m_beamInfoSvc->sigmaS() );
  ret.setEpsilonN( m_beamInfoSvc->epsilonN() );
  ret.setTotalXSec( m_beamInfoSvc->totalXSec() );
  ret.setHorizontalCrossingAngle( m_beamInfoSvc->horizontalCrossingAngle() );
  ret.setVerticalCrossingAngle( m_beamInfoSvc->verticalCrossingAngle() );
  ret.setHorizontalBeamlineAngle( m_beamInfoSvc->horizontalBeamlineAngle() );
  ret.setVerticalBeamlineAngle( m_beamInfoSvc->verticalBeamlineAngle() );
  ret.setBetaStar( m_beamInfoSvc->betaStar() );
  ret.setBunchSpacing( m_beamInfoSvc->bunchSpacing() );
  ret.setBeamSpot( m_beamInfoSvc->beamSpot() );
  ret.setLuminosity( m_beamInfoSvc->luminosity() );
  return ret;
}

void GenRndInit::MTBarrier::wait() {
  std::unique_lock<std::mutex> lock{_mutex};
  if ( --m_n_waiting <= 0 ) {
    _cv.notify_all();
  } else {
    _cv.wait( lock, [this] { return m_n_waiting <= 0; } );
  }
}
