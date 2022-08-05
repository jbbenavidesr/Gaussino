/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

#include "GiGaMTCoreRun/GiGaWorkerRunManager.h"

#include "GiGaMTCoreUtils/GiGaMTUtils.h"
#include "GiGaMTCoreRun/GiGaWorkerPayload.h"
#include "GiGaMTCoreMessage/IGiGaMessage.h"
#include "GiGaMTCoreTruth/MCTruthConverter.h"

#include "G4Event.hh"
#include "HepMC3/GenEvent.h"

// Main worker thread class that is instantiated in the
// initialize of the main simulation service and given
// to std::thread when creating. Its main functionality
// is implemented in the operator() which is executed in
// the separate thread.
//
// Once deployed in the thread there is no way of changing the configuration
// of Geant4 so hopefully noone ever wants to do that...

class GiGaWorkerPilotFAC;
class G4EventProxy;
class G4WorkerThread;

class GiGaWorkerPilot : public GiGaMessage
{
  friend class GiGaWorkerPilotFAC;
  friend class G4EventProxy;

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
  // 1. Initializes the thread by creating the Run/GiGaWorkerRunManager instance
  // and configuring it.
  // 2. Waits for events to be simulated by blocking on a queue
  // 3. Destructs the everything as needed when signal for termination is received
  void operator()();

  // Initializes the Run/GiGaWorkerRunManager and some other stuff
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

  void setPostProcessing (bool postprocessing) { m_postprocessing = postprocessing; }

  // Returns singleton instance of initialization barrier.
  // First call determines the created number of threads that
  // are have to arrive at the barrier before all are given
  // the go-ahead.
  static GiGaMTBarrier& GetInitBarrier( std::size_t num_threads = 0 )
  {
    static GiGaMTBarrier barrier( num_threads );
    return barrier;
  }
  static GiGaMTBarrier& GetFinalBarrier( std::size_t num_threads = 0 )
  {
    static GiGaMTBarrier barrier( num_threads );
    return barrier;
  }

private:
  // Adds the event to the internal cleanup list of this pilot.
  // Necessary to ensure that the correct G4 worker thread deletes the
  // G4 event data objects
  //
  // Marked private so only the Proxy objects have access to this function.
  // FIXME: ugly sadface
  void RegisterForCleanUp( G4Event* evt );
  void CleanUp();

  // Constructor is private as these objects are supposed to
  // only be created using the corresponding factories
  GiGaWorkerPilot() = default;

  // Pointer to the input queue
  GiGaPayloadQueue* m_input_queue = nullptr;
  G4WorkerThread* m_context       = nullptr;

  // Events post-processing
  bool m_postprocessing = false;

  // Number of worker
  size_t iWorker  = 0;
  size_t nWorkers = 0;
  size_t nDeleted = 0;
  size_t nCreated = 0;
  size_t nKept = 0;
  // Internal strings to store different states of the processed event
  // FIXME: These should not be used in production version
  std::string m_before_sim, m_after_sim, m_after_cleanup;
  bool m_track_eventstructure{false};
  std::vector<G4Event*> m_for_cleanup{};
  std::mutex m_cleanup_lock{};
};
