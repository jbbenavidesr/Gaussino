#include "GaudiAlg/GaudiAlgorithm.h"
#include "CLHEP/Random/MixMaxRng.h"

class EventContext;

class RndAlgSeeder: public GaudiAlgorithm{
  public:
  using GaudiAlgorithm::GaudiAlgorithm;
  using GaudiAlgorithm::initialize;
  using GaudiAlgorithm::finalize;

  template<typename T=CLHEP::MixMaxRng> T createRndmEngine() const;
};
