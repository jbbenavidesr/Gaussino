#include "GiGaMTCoreTruth/MCTruthConverter.h"
#include "GiGaMTCoreRun/G4EventProxy.h"

namespace Gaussino {
  typedef std::tuple<G4EventProxyPtr, Gaussino::MCTruthPtr> GiGaSimReturn;
  typedef std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> GiGaSimReturns;
}
