#pragma once

namespace Gaussino
{
  namespace HepMCEventLocation
  {
    static const std::string Default = "Gen/HepMCEvents";
    static const std::string Signal  = "Gen/SignalDecayTree";
    static const std::string BInfo   = "Gen/BInfo";
  }

  namespace MCParticleLocation {
    static const std::string Default = "MC/Particles";
  }
  
  namespace MCVertexLocation {
    static const std::string Default = "MC/Vertices";
  }
}
