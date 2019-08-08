#pragma once
#include <ostream>

namespace HepMC3{
  /// Status code in HepMC::GenParticle
  namespace Status {
    enum statusType{ Unknown = 0,
      StableInProdGen = 1,
      DecayedByProdGen = 2,
      DocumentationParticle = 3,
      DecayedByDecayGen = 777,
      DecayedByDecayGenAndProducedByProdGen = 888,
      SignalInLabFrame = 889,
      SignalAtRest = 998,
      StableInDecayGen = 999
    };
  }

  inline std::ostream & operator << (std::ostream & s, Status::statusType e) {
    switch (e) {
      case Status::Unknown                               : return s << "Unknown";
      case Status::StableInProdGen                       : return s << "StableInProdGen";
      case Status::DecayedByProdGen                      : return s << "DecayedByProdGen";
      case Status::DocumentationParticle                 : return s << "DocumentationParticle";
      case Status::DecayedByDecayGen                     : return s << "DecayedByDecayGen";
      case Status::DecayedByDecayGenAndProducedByProdGen : return s << "DecayedByDecayGenAndProducedByProdGen";
      case Status::SignalInLabFrame                      : return s << "SignalInLabFrame";
      case Status::SignalAtRest                          : return s << "SignalAtRest";
      case Status::StableInDecayGen                      : return s << "StableInDecayGen";
      default : return s << "ERROR wrong value " << int(e) << " for enum Status::statusType";
    }
  }
  inline Status::statusType to_status_type(int i) {
    switch (i) {
      case Status::Unknown                               : return Status::Unknown                              ;
      case Status::StableInProdGen                       : return Status::StableInProdGen                      ;
      case Status::DecayedByProdGen                      : return Status::DecayedByProdGen                     ;
      case Status::DocumentationParticle                 : return Status::DocumentationParticle                ;
      case Status::DecayedByDecayGen                     : return Status::DecayedByDecayGen                    ;
      case Status::DecayedByDecayGenAndProducedByProdGen : return Status::DecayedByDecayGenAndProducedByProdGen;
      case Status::SignalInLabFrame                      : return Status::SignalInLabFrame                     ;
      case Status::SignalAtRest                          : return Status::SignalAtRest                         ;
      case Status::StableInDecayGen                      : return Status::StableInDecayGen                     ;
    }
    return Status::Unknown;
  }
}
