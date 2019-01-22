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
  inline G4Event* event() { return m_event; }
  inline G4Event* operator->() { return m_event; }

private:
  friend class GiGaWorkerPilot;
  G4EventProxy( G4Event* event, GiGaWorkerPilot* vec ) : m_event( event ), m_vec( vec ){};
  G4Event* m_event;
  GiGaWorkerPilot* m_vec;
};

typedef std::vector<G4EventProxy> G4EventProxies;
