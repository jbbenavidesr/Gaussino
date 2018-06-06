#include "GiGaMTCore/GiGaWorkerPilot.h"
#include "GiGaMTCore/GiGaMTRunManager.h"
#include "GiGaMTCore/GiGaWorkerRunManager.h"

#include "Geant4/G4UserWorkerThreadInitialization.hh"

GiGaWorkerPilot::GiGaWorkerPilot( GiGaWorkerPilot&& right )
{
  right.m_input_queue = this->m_input_queue;
  this->m_input_queue = nullptr;
}

void GiGaWorkerPilot::InitializeWorker()
{
  debug( "Initializing the worker for this thread" );
  auto master_mgr         = GiGaMTRunManager::GetGiGaMTRunManager();
  auto worker_initializer = master_mgr->GetUserWorkerThreadInitialization();
  auto worker_mgr         = worker_initializer->CreateWorkerRunManager();
  // TODO: Actual initialization, copy from G4MTRunManager probably
}

void GiGaWorkerPilot::RunTermination() {}

void GiGaWorkerPilot::operator()()
{
  debug( "Starting up thread ..." );

  // Create and initialize the worker run manager
  InitializeWorker();
  // FIXME: We will probably want some thread barrier sync here
  // to make sure that all threads are done before continuing.

  debug( "Finished initialization, awaiting something to simulate!" );
  // Get the pointer to the GiGaWorkerRunManager
  auto mgr = GiGaWorkerRunManager::GetGiGaWorkerRunManager();

  // Begin the main loop by
  GiGaWorkerPayload payload;
  while ( true ) {
    m_input_queue->wait_dequeue( payload );
    auto & [ evt, engine, ret_promise ] = payload;

    // We treat the case of the G4Event* pointer being a nullptr
    // as the sentinel value and break the loop.
    if ( !evt ) {
      debug( "Sentinel detected, ending loop" );

      // We put the payload back into the queue to trigger a cascading
      // shut down of all threads if one sentinel was pushed into the queue
      m_input_queue->enqueue( payload );
      break;
    }

    // Reset the random number engine of this worker thread
    G4Random::setTheEngine( engine );

    mgr->ProcessEvent( evt );
  }

  // FIXME: Some shut off stuff probably
}
