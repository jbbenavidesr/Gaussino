#pragma once

#include "blockingconcurrentqueue.h"
#include <future>
#include <vector>

// Collection of wrapper classes to pass information between the
// Gaudi and G4 threads mediated by a queue, somehow, magically ...

class G4Event;
namespace CLHEP
{
  class HepRandomEngine;
}

// Simulation return dummy class to continue with the infrastructure
// prototyping
class DummyReturn
{
  std::vector<double> energies;
};

// Wrapping tuple for the information that needs to be passed
// typedef std::tuple<G4Event*, CLHEP::HepRandomEngine*, std::promise<DummyReturn>> GiGaWorkerPayload;
// TODO: Just wrapping it into a tuple does not seem to work because it does not find a suitable copy/move constructor for the queue. This is, I think, caused by the default copy constructor of a tuple failing to be valid due to promise which cannot be copied and the default move constructor is not noexcept. So we are trying a pointer instead...
typedef std::tuple<G4Event*, CLHEP::HepRandomEngine*, std::promise<DummyReturn>*> GiGaWorkerPayload;
//struct GiGaWorkerPayload {
  //GiGaWorkerPayload( G4Event* _event, CLHEP::HepRandomEngine* _engine, std::promise<DummyReturn>&& _prom )
      //: event( _event ), engine( _engine ), ret_promise( std::move( _prom ) )
  //{
  //}
  //GiGaWorkerPayload( const GiGaWorkerPayload& ) = delete;
  //GiGaWorkerPayload( GiGaWorkerPayload&& other ) noexcept
      //: event( other.event ), engine( other.engine ), ret_promise( std::move( other.ret_promise ) )
  //{
  //}

  //G4Event* event{nullptr};
  //CLHEP::HepRandomEngine* engine{nullptr};
  //std::promise<DummyReturn> ret_promise;
//};
typedef moodycamel::BlockingConcurrentQueue<GiGaWorkerPayload> GiGaPayloadQueue;
