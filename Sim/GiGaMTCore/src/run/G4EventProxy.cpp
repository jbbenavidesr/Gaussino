#include "GiGaMTCoreRun/G4EventProxy.h"
#include "GiGaMTCoreRun/GiGaWorkerPilot.h"

G4EventProxy::~G4EventProxy()
{
  if ( m_vec && m_event ) {
    m_vec->RegisterForCleanUp( m_event );
  } else if (m_event) {
    delete m_event;
  }
}

G4EventProxy::G4EventProxy( G4EventProxy&& right ) noexcept : m_event( right.m_event ), m_vec( right.m_vec ), m_truth(right.m_truth)
{
  right.m_event = nullptr;
  right.m_vec   = nullptr;
  right.m_truth = nullptr;
}
