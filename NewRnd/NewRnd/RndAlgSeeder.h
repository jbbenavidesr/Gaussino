#include "CLHEP/Random/MixMaxRng.h"
#include "GaudiAlg/GaudiAlgorithm.h"

class EventContext;

class RndAlgSeeder : public GaudiAlgorithm
{
  Gaudi::Property<size_t> m_forcedSeed{this, "ForcedSeed", 0, "Force seed to value if not 0"};

public:
  using GaudiAlgorithm::GaudiAlgorithm;
  using GaudiAlgorithm::finalize;
  using GaudiAlgorithm::initialize;

  template <typename T = CLHEP::MixMaxRng>
  T createRndmEngine() const;
};
