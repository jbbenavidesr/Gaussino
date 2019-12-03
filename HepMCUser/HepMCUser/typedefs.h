#include "HepMC3/GenEvent.h"
#include <vector>

namespace HepMC3
{
  typedef std::shared_ptr<GenEvent> GenEventPtr;
  typedef std::vector<GenEventPtr> GenEventPtrs;
} // End HepMC namespace
