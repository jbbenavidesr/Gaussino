#pragma once

#include <string>

namespace Gaussino {
  namespace HepMC {
    namespace Attributes{
      const std::string GeneratorName = "GeneratorName";
      const std::string SignalProcessID = "SignalProcessID";
      const std::string SignalProcessVertex = "SignalProcessVertex";
      const std::string GaudiEventNumber = "GaudiEventNumber";
      const std::string GaudiRunNumber = "GaudiRunNumber";

      const std::string SimResults = "SimResults";
      const std::string MCTruth = "MCTruth";
      const std::string G4EventProxy = "G4EventProxy";
    }
  }
}
