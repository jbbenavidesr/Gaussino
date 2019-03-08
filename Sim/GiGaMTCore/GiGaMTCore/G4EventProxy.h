#pragma once

#include "Geant4/G4Event.hh"
#include <vector>

class GiGaWorkerPilot;

class G4EventProxy
{
public:
  G4EventProxy()                      = delete;
  G4EventProxy( const G4EventProxy& ) = delete;
  ~G4EventProxy();
  G4EventProxy( G4EventProxy&& ) noexcept;
  // Construct the proxy. If a GiGaWorkerPilot is provided, the G4Event will
  // enqueued with the respective worker for deletion. If not, destructor
  // of the proxy will delete the event.
  G4EventProxy( G4Event* event, GiGaWorkerPilot* vec = nullptr ) : m_event( event ), m_vec( vec ){};
  inline G4Event* event() { return m_event; }
  inline G4Event* operator->() { return m_event; }

private:
  G4Event* m_event;
  GiGaWorkerPilot* m_vec;
};

typedef std::vector<G4EventProxy> G4EventProxies;
