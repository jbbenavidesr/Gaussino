#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include <vector>

namespace HepMC3
{
  typedef std::shared_ptr<GenEvent> GenEventPtr;
  typedef std::vector<GenEventPtr> GenEventPtrs;
  typedef std::vector<GenParticlePtr> GenParticlePtrs;
} // End HepMC namespace
