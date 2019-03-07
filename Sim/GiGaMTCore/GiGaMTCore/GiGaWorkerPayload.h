#pragma once

#include "GiGaMTCore/G4EventProxy.h"
#include "GiGaMTCore/Truth/MCTruthConverter.h"
#include "blockingconcurrentqueue.h"
#include <future>
#include <vector>
#include <optional>
#include "NewRnd/RndCommon.h"

// Collection of wrapper classes to pass information between the
// Gaudi and G4 threads mediated by a queue, somehow, magically ...

namespace HepMC
{
  class GenEvent;
} /* HepMC */
namespace CLHEP
{
  class HepRandomEngine;
} /* CLHEP */

// TODO: Just wrapping it into a tuple does not seem to work because it does not find a suitable copy/move constructor
// for the queue. This is, I think, caused by the default copy constructor of a tuple failing to be valid due to promise
// which cannot be copied and the default move constructor is not noexcept. So we are trying a pointer instead...
typedef std::tuple<Gaussino::MCTruthConverterPtr, HepRandomEnginePtr, std::promise<G4EventProxy>*> GiGaWorkerPayload;

typedef std::optional<GiGaWorkerPayload> GiGaWorkerPayloadOpt;

typedef moodycamel::BlockingConcurrentQueue<GiGaWorkerPayloadOpt> GiGaPayloadQueue;
