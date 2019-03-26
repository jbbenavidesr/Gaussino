#pragma once
namespace Gaussino::GenStatus
{
  enum statusType {
    Unknown                               = 0,
    StableInProdGen                       = 1,
    DecayedByProdGen                      = 2,
    DocumentationParticle                 = 3,
    DecayedByDecayGen                     = 777,
    DecayedByDecayGenAndProducedByProdGen = 888,
    SignalInLabFrame                      = 889,
    SignalAtRest                          = 998,
    StableInDecayGen                      = 999
  };
}
