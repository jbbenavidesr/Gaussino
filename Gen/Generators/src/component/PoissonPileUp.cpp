// local
#include "PoissonPileUp.h"

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// From Generators
#include "GenInterfaces/ICounterLogFile.h"
#include "Generators/GenCounters.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandPoisson.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PoissonPileUp
//
// 2018-02-04 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT(PoissonPileUp)

PoissonPileUp::PoissonPileUp(const std::string& type, const std::string& name,
                             const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IPileUpTool>(this);
}

StatusCode PoissonPileUp::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_xmlLogTool = tool<ICounterLogFile>("XmlCounterLogFile");
  if (!m_xmlLogTool) sc &= Warning("No XML Counter log tool found");

  return sc;
}

unsigned int PoissonPileUp::numberOfPileUp(HepRandomEnginePtr & engine) {
  
  unsigned int result = 0;

  CLHEP::RandPoisson poissonGenerator{engine.getref(), m_mean};

  while (0 == result) {
    m_nEvents++;
    result = (unsigned int)poissonGenerator();
    if (0 == result) {
      m_numberOfZeroInteraction++;
    }
  }
  return result;
}

void PoissonPileUp::printPileUpCounters() {
  using namespace GenCounters;
  printCounter(m_xmlLogTool, "all events (including empty events)", m_nEvents);
  printCounter(m_xmlLogTool, "events with 0 interaction",
               m_numberOfZeroInteraction);
}
