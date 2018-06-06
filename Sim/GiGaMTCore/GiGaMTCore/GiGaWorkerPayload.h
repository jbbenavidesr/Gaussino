#pragma once

#include <future>
#include <vector>
#include "blockingconcurrentqueue.h"

// Collection of wrapper classes to pass information between the
// Gaudi and G4 threads mediated by a queue, somehow, magically ...

class G4Event;
namespace CLHEP {
  class HepRandomEngine;
}

// Simulation return dummy class to continue with the infrastructure
// prototyping
class DummyReturn {
  std::vector<double> energies;
};

// Wrapping tuple for the information that needs to be passed 
typedef std::tuple<G4Event*, CLHEP::HepRandomEngine*, std::promise<DummyReturn>> GiGaWorkerPayload;
typedef moodycamel::BlockingConcurrentQueue<GiGaWorkerPayload> GiGaPayloadQueue;
