#pragma once

#include <map>

class LinkedParticle;
namespace LHCb {
  class MCParticle;
}

// Typedef might be enough, let's see whether something more complicated is needed
typedef std::map<const LinkedParticle*, const LHCb::MCParticle*> LinkedParticleMCParticleLinks;
