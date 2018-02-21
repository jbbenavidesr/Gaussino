#pragma once

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "GenInterfaces/IPileUpTool.h"

// forward declaration
class IRndmGenSvc;
class ICounterLogFile;

/** @class PoissonPileUp PoissonPileUp.h "PoissonPileUp.h"
 *
 *  Simple tool to generate pile-up numbers from a Poisson with
 *  user-provided mean.
 *
 *  @author Dominik Muller
 *  @date   2018-02-04
 */
class PoissonPileUp : public GaudiTool, virtual public IPileUpTool {
  public:
  /// Standard constructor
  PoissonPileUp(const std::string& type, const std::string& name,
                const IInterface* parent);

  virtual ~PoissonPileUp() = default;

  /// Initialize method
  virtual StatusCode initialize();

  virtual unsigned int numberOfPileUp();

  /// Implements IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters();

  protected:
  private:
  ICounterLogFile* m_xmlLogTool = nullptr;
  IRndmGenSvc* m_randSvc = nullptr;

  std::atomic_long m_numberOfZeroInteraction{0};
  std::atomic_long m_nEvents{0};

  Gaudi::Property<unsigned int> m_mean{this, "PileUpNu", 1, "Pile-up nu"};
};
