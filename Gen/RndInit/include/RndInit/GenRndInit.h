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
#pragma once

#include "GaudiAlg/Producer.h"
#include "GenBeam/IBeamInfoSvc.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
// FIXME: Get rid of the GenHeader dependence at some point
#include "Defaults/Locations.h"
#include "Event/BeamParameters.h"
#include "Event/GenHeader.h"
#include "Event/ODIN.h"
#include "NewRnd/RndAlgSeeder.h"
#include "Utils/LocalTL.h"
#include "GaudiKernel/Memory.h"

/** @class GenRndInit GenRndInit.h
 *
 *  First TopAlg for Generator phase of Gaussino.
 *  Configures the event context run number
 *  It also creates and fill the GenHeader.
 *
 *  @author Dominik Muller
 *  @date   2018-01-29
 */
class GenRndInit : public Gaudi::Functional::Producer<std::tuple<LHCb::GenHeader, LHCb::BeamParameters, LHCb::ODIN>(),
                                                      Gaudi::Functional::Traits::BaseClass_t<RndInitAlg>> {
protected:
  Gaudi::Property<long long> m_firstEvent{this, "FirstEventNumber", 1, "Number of the first event"};
  Gaudi::Property<long long> m_firstTimingEvent{this, "TimingSkipAtStart", 1, "Number of the event to start the clock"};
  Gaudi::Property<unsigned int> m_runNumber{this, "RunNumber", 1, "The run number"};

  mutable Gaudi::Accumulators::StatCounter<> m_totMem{this, "Total Memory [MB]"};
  mutable Gaudi::Accumulators::StatCounter<> m_totMemPerThread{this, "Total Memory per Thread [MB]"};

public:
  GenRndInit( const std::string& name, ISvcLocator* pSvcLocator )
      : Producer( name, pSvcLocator,
                  {KeyValue{"GenHeaderOutputLocation", Gaussino::GenHeaderLocation::PreGeneration},
                   KeyValue{"BeamParameters", LHCb::BeamParametersLocation::Default},
                   KeyValue( "ODIN", LHCb::ODINLocation::Default )} ) {}

  using Clock = std::chrono::high_resolution_clock;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual std::tuple<LHCb::GenHeader, LHCb::BeamParameters, LHCb::ODIN> operator()() const override;

protected:
  /// Increment atomic number of events processed and store result
  /// in a thread and instance local variable
  long increaseEventCounter() const {
    m_localCounter = ++m_evtCounter;
    return m_localCounter.get();
  }

  /// Return number of events processed using the thread_local variable
  inline long eventCounter() const { return m_localCounter.get(); }

  /// Fill the BeamParameters object from the BeamInfoSvc
  LHCb::BeamParameters createBeamParameters() const;

  /** Print the run number, event number and optional vector of seeds
   *  @param[in] evt event number
   *  @param[in] run run number
   *  @param[in] seeds (optional) vector of seeds
   */
  virtual void printEventRun( long long evt, int run, std::vector<long int>* seeds = 0 ) const;

  mutable std::atomic_long  m_evtCounter{0};
  mutable std::atomic_long  m_evtTimingCounter{0};
  mutable LocalTL<long>     m_localCounter;
  mutable Clock::time_point m_start_time;
  long                      m_eventMax{0}; ///< Number of events requested (ApplicationMgr.EvtMax)

  class MTBarrier {
    std::mutex              _mutex;
    std::condition_variable _cv;
    std::size_t             m_n_waiting;

  public:
    explicit MTBarrier( std::size_t count ) : m_n_waiting( count ) {}
    MTBarrier()                   = delete;
    MTBarrier( const MTBarrier& ) = delete;
    MTBarrier( MTBarrier&& )      = delete;
    void wait();
  };

  static MTBarrier& GetInitBarrier( std::size_t num_threads = 0 ) {
    static MTBarrier barrier( num_threads );
    return barrier;
  }

  MTBarrier*   m_barrier;
  mutable bool m_wait_at_barrier{true};

private:
  ServiceHandle<IBeamInfoSvc> m_beamInfoSvc{this, "BeamInfoSvc", "BeamInfoSvc"};
};
