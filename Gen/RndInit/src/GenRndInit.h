#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "GaudiAlg/Producer.h"
// FIXME: Get rid of the GenHeader dependence at some point
#include "Event/GenHeader.h"
#include "Defaults/Locations.h"
#include "NewRnd/RndAlgSeeder.h"

/** @class GenRndInit GenRndInit.h
 *
 *  First TopAlg for Generator phase of Gaussino.
 *  Configures the event context run number
 *  It also creates and fill the GenHeader.
 *
 *  @author Dominik Muller
 *  @date   2018-01-29
 */
class GenRndInit : public Gaudi::Functional::Producer< LHCb::GenHeader()>{
  private:
  Gaudi::Property<int> m_skipFactor{this, "SkipFactor", 0, "skip some random numbers"};
  Gaudi::Property<long long> m_firstEvent{this, "FirstEventNumber", 1, "Number of the first event"};
  Gaudi::Property<long long> m_firstTimingEvent{this, "FirstTimingEventNumber", -1, "Number of the event to start the clock"};
  Gaudi::Property<long long> m_lastTimingEvent{this, "LastTimingEventNumber", -1, "Number of the event to stop the clock"};
  Gaudi::Property<unsigned int> m_runNumber{this, "RunNumber", 1, "The run number"};
  Gaudi::Property<std::string> m_mcHeader{this, "MCHeader", LHCb::GenHeaderLocation::Default, "Location of the GenHeader"};
  public:
  /// Standard constructor
  GenRndInit( const std::string& name, ISvcLocator* pSvcLocator )
      : Producer( name, pSvcLocator,
                          {KeyValue{"GenHeaderOutputLocation", Gaussino::GenHeaderLocation::PreGeneration}}
                           )
  {
  }
  using Clock = std::chrono::high_resolution_clock;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual LHCb::GenHeader
  operator()() const override;

  protected:
  /// Return number of events processed
  long increaseEventCounter() const { return m_evtCounter++; }
  /// Return number of events processed
  long eventCounter() const { return m_evtCounter; }

  /** Print the run number, event number and optional vector of seeds
   *  @param[in] evt event number
   *  @param[in] run run number
   *  @param[in] seeds (optional) vector of seeds
   *  @param[in] time (optional) time of the event
   */
  void printEventRun(long long evt, int run,
                     std::vector<long int>* seeds = 0) const;

  mutable std::atomic_long m_evtCounter{
      0};              ///< Pointer to EventCounter interface
  long m_eventMax{0};  ///< Number of events requested (ApplicationMgr.EvtMax)
  std::string m_appName{""};     ///< Application Name
  std::string m_appVersion{""};  ///< Application Version
  mutable Clock::time_point m_start_time;
  class MTBarrier
  {
  private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::size_t m_n_waiting;

  public:
    explicit MTBarrier( std::size_t count ) : m_n_waiting( count ) {}
    void wait()
    {
      std::unique_lock<std::mutex> lock{_mutex};
      if ( --m_n_waiting == 0 ) {
        _cv.notify_all();
      } else {
        _cv.wait( lock, [this] { return m_n_waiting == 0; } );
      }
    }
    MTBarrier()                     = delete;
    MTBarrier( const MTBarrier& ) = delete;
    MTBarrier( MTBarrier&& )      = delete;
  };
  static MTBarrier& GetInitBarrier( std::size_t num_threads = 0 )
  {
    static MTBarrier barrier( num_threads );
    return barrier;
  }
  MTBarrier* m_barrier;
  mutable bool m_wait_at_barrier{true};
  MTBarrier* m_endbarrier;
  mutable bool m_wait_at_endbarrier{true};

};
