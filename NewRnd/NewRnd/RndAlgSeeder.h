#include "CLHEP/Random/MixMaxRng.h"
#include "CLHEP/Random/RanluxEngine.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AnyDataHandle.h"

class EventContext;
namespace Random {
  typedef std::pair<unsigned int, unsigned int> SeedPair;
  const std::string Location = "SeedsInternalUsage";
}

class RndAlgSeeder : public GaudiAlgorithm
{
  Gaudi::Property<size_t> m_forcedSeed{this, "ForcedSeed", 0, "Force seed to value if not 0"};
  Gaudi::Property<bool> m_simpleSeed{this, "SimpleSeed", false, "Use simple seed instead of event context"};

public:
  using GaudiAlgorithm::GaudiAlgorithm;
  using GaudiAlgorithm::finalize;
  using GaudiAlgorithm::initialize;

protected:
  template <typename T = CLHEP::MixMaxRng>
  T createRndmEngine() const;

  mutable std::atomic_uint m_counter{1};

private:
  AnyDataHandle<Random::SeedPair> m_forseed{Random::Location, Gaudi::DataHandle::Reader, this};
};

class RndInitAlg : public GaudiAlgorithm
{

public:
  using GaudiAlgorithm::GaudiAlgorithm;
  using GaudiAlgorithm::finalize;
  using GaudiAlgorithm::initialize;

protected:
  void SetSeedPair(unsigned int val1, unsigned int val2) const {
    m_forseed.put(std::make_pair(val1, val2));
  }

  mutable std::atomic_uint m_counter{1};

private:
  mutable AnyDataHandle<Random::SeedPair> m_forseed{Random::Location, Gaudi::DataHandle::Writer, this};
};
