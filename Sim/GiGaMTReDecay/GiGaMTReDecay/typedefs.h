#include "GiGaMTCoreTruth/LinkedParticle.h"
#include <map>
#include "GiGaMTCoreTruth/MCTruthConverter.h"

namespace Gaussino::ReDecay {
  typedef std::map<const LinkedParticle*, MCTruthPtr> SignalTruths;
}
