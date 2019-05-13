#pragma once
#include<string>

namespace Gaussino
{
  namespace HepMCEventLocation
  {
    const std::string Default = "Gen/HepMCEvents";
    const std::string Signal  = "Gen/SignalDecayTree";
    const std::string BInfo   = "Gen/BInfo";
  }

  namespace MCParticleLocation {
    const std::string Default = "MC/Particles";
  }
  
  namespace MCVertexLocation {
    const std::string Default = "MC/Vertices";
  }

  namespace GenHeaderLocation {
    const std::string Default = "Gen/Header";
    const std::string PreGeneration = "Gen/HeaderPreGen";
  }
  namespace G4EventsLocation
  {
    const std::string Default = "Sim/G4Events";
  }
  namespace MCTruthsLocation
  {
    const std::string Default = "MC/Truths";
  }
  namespace LinkedParticleMCParticleLinksLocation
  {
    const std::string Default = "MC/LinkedParticleMCParticleLinksLocation";
  }
}
