// local
#include "PoissonPileUp.h"

// from Gaudi
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// From Generators
#include "GenInterfaces/ICounterLogFile.h"
#include "Generators/GenCounters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PoissonPileUp
//
// 2018-02-04 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY(PoissonPileUp)

PoissonPileUp::PoissonPileUp(const std::string& type, const std::string& name,
                             const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IPileUpTool>(this);
}

StatusCode PoissonPileUp::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  if (!m_randSvc) return Error("Could not get RndmGenSvc");
  m_xmlLogTool = tool<ICounterLogFile>("XmlCounterLogFile");
  if (!m_xmlLogTool) Warning("No XML Counter log tool found");

  return sc;
}

unsigned int PoissonPileUp::numberOfPileUp() {
  Rndm::Numbers poissonGenerator{};
  unsigned int result = 0;

  if (poissonGenerator.initialize(m_randSvc, Rndm::Poisson(m_mean))
          .isSuccess()) {
    while (0 == result) {
      m_nEvents++;
      result = (unsigned int)poissonGenerator();
      if (0 == result) {
        m_numberOfZeroInteraction++;
      }
    }
  } else {
    Error("Could not initialize Poisson random numbers!");
  }
  return result;
}

void PoissonPileUp::printPileUpCounters() {
  using namespace GenCounters;
  printCounter(m_xmlLogTool, "all events (including empty events)", m_nEvents);
  printCounter(m_xmlLogTool, "events with 0 interaction",
               m_numberOfZeroInteraction);
}
