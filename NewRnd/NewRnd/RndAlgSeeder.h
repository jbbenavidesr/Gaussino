#include "CLHEP/Random/MixMaxRng.h"
#include "CLHEP/Random/RanluxEngine.h"
#include "GaudiAlg/GaudiAlgorithm.h"

class EventContext;

class RndAlgSeeder : public GaudiAlgorithm
{
  Gaudi::Property<size_t> m_forcedSeed{this, "ForcedSeed", 0, "Force seed to value if not 0"};
  Gaudi::Property<bool> m_simpleSeed{this, "SimpleSeed", false, "Use simple seed instead of event context"};

public:
  using GaudiAlgorithm::GaudiAlgorithm;
  using GaudiAlgorithm::finalize;
  using GaudiAlgorithm::initialize;

  template <typename T = CLHEP::MixMaxRng>
  //template <typename T = CLHEP::RanluxEngine>
  T createRndmEngine() const;

  mutable std::atomic_uint m_counter{1};
};
