#pragma once

#include "GiGaMTCore/GiGaWorkerRunManager.h"

#include "GiGaMTCore/GiGaWorkerPayload.h"
#include "GiGaMTCore/IGiGaMessage.h"
#include "GiGaMTCore/GiGaMTUtils.h"

// Main worker thread class that is instantiated in the
// initialize of the main simulation service and given
// to std::thread when creating. Its main functionality
// is implemented in the operator() which is executed in
// the separate thread.
//
// Once deployed in the thread there is no way of changing the configuration
// of Geant4 so hopefully noone ever wants to do that...

class GiGaWorkerPilotFAC;
class G4WorkerThread;

class GiGaWorkerPilot : public GiGaMessage
{
  friend class GiGaWorkerPilotFAC;

public:
  // Cannot copy it, only moving is allowed so
  // object cannot be messed with after handing it over
  // to the thread. Not that it really matters as there
  // is probably almost no state
  GiGaWorkerPilot( const GiGaWorkerPilot& ) = delete;
  GiGaWorkerPilot( GiGaWorkerPilot&& );

  // Main execution executed in a separate thread
  // Does the following:
  //
  // 1. Initializes the thread by creating the GiGaWorkerRunManager instance
  // and configuring it.
  // 2. Waits for events to be simulated by blocking on a queue
  // 3. Destructs the everything as needed when signal for termination is received
  void operator()();

  // Initializes the GiGaWorkerRunManager and some other stuff
  // The initialization of the G4WorkerRunManager base used to happen
  // in G4MTRunManager::StartThread. As we do this ourselfs we take
  // a lot of inspiration from there.
  void InitializeWorker();

  void FinalizeWorker();

  // Does some cleanup after the event loop has terminated
  void RunTermination();

  // Set the queue to retrieve the payload from.
  // This should be be done only once and I really don't know why we don't just simply hardcode this.
  void SetInputQueue( GiGaPayloadQueue* que )
  {
    debug( "Setting input queue" );
    m_input_queue = que;
  }

  // Returns singleton instance of initialization barrier.
  // First call determines the created number of threads that
  // are have to arrive at the barrier before all are given
  // the go-ahead.
  static GiGaMTBarrier & GetInitBarrier(std::size_t num_threads=0) {
    static GiGaMTBarrier barrier(num_threads);
    return barrier;
  }
  static GiGaMTBarrier & GetFinalBarrier(std::size_t num_threads=0) {
    static GiGaMTBarrier barrier(num_threads);
    return barrier;
  }

private:
  // Constructor is private as these objects are supposed to
  // only be created using the corresponding factories
  GiGaWorkerPilot() = default;

  // Pointer to the input queue
  GiGaPayloadQueue* m_input_queue = nullptr;
  G4WorkerThread* m_context = nullptr;

  // Number of worker
  size_t iWorker = 0;
  size_t nWorkers = 0;
};
