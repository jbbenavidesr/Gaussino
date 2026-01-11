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
#include <fmt/core.h>

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
    if ( m_firstTimingEvent > m_eventMax ) {
      error() << "First timing event is larger than EvtMax (=" << m_eventMax << ")!" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_firstTimingEvent != 1 ) {
      if ( m_firstTimingEvent > m_eventMax ) {
        error() << "FirstTimingEvent is larger than EvtMax (=" << m_eventMax << ")!" << endmsg;
        return StatusCode::FAILURE;
      }
      if ( m_firstTimingEvent < Gaudi::Concurrency::ConcurrencyFlags::numThreads() + 2 ) {
        error() << "FirstTimingEvent is < number of threads + 2 (="
                << Gaudi::Concurrency::ConcurrencyFlags::numThreads() + 2 << ")!" << endmsg;
        return StatusCode::FAILURE;
      }
    }
    info() << "Setting timing barrier at " << m_firstTimingEvent << endmsg;
    return StatusCode::SUCCESS;
  } );
}

std::tuple<LHCb::GenHeader, LHCb::BeamParameters, LHCb::ODIN, Gsino::SimHeader> GenRndInit::operator()() const {
  debug() << "==> Execute" << endmsg;

  // Timing information
  auto eventNumber = m_firstEvent.value() + this->increaseEventCounter() - 1;
  if ( m_timingStarted ) {
    m_evtTimingCounter++;
  } else {
    auto barrierEvent = m_firstEvent.value() + m_firstTimingEvent.value() - 1;
    if ( eventNumber == barrierEvent ) {
      std::lock_guard<std::mutex> lock( m_timingBarrierMutex );
      // Initialising the start time for more precise monitoring
      // when the event loop is in full swing.
      m_start_time = Clock::now();
      info() << "Started loop timing!" << endmsg;
      m_timingStarted = true;
      m_timingBarrierCV.notify_all();
      m_evtTimingCounter++;
    } else if ( eventNumber > barrierEvent - (int)Gaudi::Concurrency::ConcurrencyFlags::numThreads() ) {
      std::unique_lock<std::mutex> lock( m_timingBarrierMutex );
      m_timingBarrierCV.wait( lock, [&] { return m_timingStarted; } );
      m_evtTimingCounter++;
    }
  }

  // Virtual memory counter
  m_totMem += System::virtualMemory() / 1e3;

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

  // Create SimHeader
  Gsino::SimHeader theSimHeader{};
  theSimHeader.setEvtNumber( eventNumber );
  theSimHeader.setRunNumber( m_runNumber.value() );

  return std::make_tuple( header, beam, odin, theSimHeader );
}

StatusCode GenRndInit::finalize() {
  auto threadsNo = Gaudi::Concurrency::ConcurrencyFlags::numThreads();
  info() << m_evtCounter << " events processed" << endmsg;
  info() << threadsNo << " Gaudi threads were used" << endmsg;
  auto duration     = std::chrono::duration_cast<std::chrono::nanoseconds>( Clock::now() - m_start_time ).count();
  auto timePerEvent = duration / 1e9 / m_evtTimingCounter;
  auto throughput   = 1. / timePerEvent;
  info() << "Measured event loop time"
         << " [ns]: " << fmt::format( "{:.3e}", (double)duration ) << endmsg;
  info() << "Time per event [s]: " << fmt::format( "{:.3e}", timePerEvent ) << endmsg;
  info() << "Throughput [1/s]: " << fmt::format( "{:.3e}", throughput ) << endmsg;
  if ( m_firstTimingEvent > 1 ) {
    // print additional information when using timing barrier
    info() << "-> measured with the last " << m_evtTimingCounter << " / " << m_eventMax << " events i.e. " << m_eventMax
           << " (EvtMax) - " << m_firstTimingEvent.value() - 1 << " (FirstTimingEvent - 1) + " << threadsNo - 1
           << " (ThreadsNo - 1)" << endmsg;
  }
  return Producer::finalize();
}

void GenRndInit::printEventRun( long long event, int run ) const {
  info() << "Evt " << event << ",  Run " << run << ",  Nr. in job = " << eventCounter() << endmsg;
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
