#pragma once

#include <atomic>
#include "GaudiAlg/GaudiAlgorithm.h"
// FIXME: Get rid of the GenHeader dependence at some point
#include "Event/GenHeader.h"

class IRndSeedingTool;

/** @class GenRndInit GenRndInit.h
 *
 *  First TopAlg for Generator phase of Gaussino.
 *  Initializes random number
 *  It also creates and fill the GenHeader.
 *
 *  @author Dominik Muller
 *  @date   2018-01-29
 */
class GenRndInit : public GaudiAlgorithm {
  public:
  /// Standard constructor
  using GaudiAlgorithm::GaudiAlgorithm;

  virtual StatusCode initialize();
  virtual StatusCode execute();

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

  private:
  Gaudi::Property<int> m_skipFactor{this, "SkipFactor", 0, "skip some random numbers"};
  Gaudi::Property<long long> m_firstEvent{this, "FirstEventNumber", 1, "Number of the first event"};
  Gaudi::Property<unsigned int> m_runNumber{this, "RunNumber", 1, "The run number"};
  Gaudi::Property<std::string> m_RndInitToolName{this, "RndInitToolName", "SeedingTool/SeedingTool", "Name of the tool for initializing the random generator"};
  Gaudi::Property<std::string> m_mcHeader{this, "MCHeader", LHCb::GenHeaderLocation::Default, "Location of the GenHeader"};

  IRndSeedingTool *m_rndtool = nullptr;
  mutable std::atomic_long m_evtCounter{
      0};              ///< Pointer to EventCounter interface
  long m_eventMax{0};  ///< Number of events requested (ApplicationMgr.EvtMax)
  std::string m_appName{""};     ///< Application Name
  std::string m_appVersion{""};  ///< Application Version
};
