#pragma once

#include "Geant4/G4Event.hh"
#include "Geant4/G4VHitsCollection.hh"
#include <vector>

class GiGaWorkerPilot;
namespace Gaussino
{
  class MCTruth;
}

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
  G4EventProxy( G4Event* event, Gaussino::MCTruth* truth, GiGaWorkerPilot* vec = nullptr )
      : m_event( event ), m_vec( vec ), m_truth( truth ){};
  inline G4Event* event() { return m_event; }
  inline const G4Event* event() const { return m_event; }
  inline Gaussino::MCTruth* truth() { return m_truth; }
  inline const Gaussino::MCTruth* truth() const { return m_truth; }
  inline G4Event* operator->() { return m_event; }
  inline const G4Event* operator->() const { return m_event; }
  template <typename COL>
  COL* GetHitCollection( std::string colname ) const
  {
    G4int i = -1;
    if ( colname.find( "/" ) == std::string::npos ) // HCname only
    {
      for ( G4int j = 0; j < m_event->GetHCofThisEvent()->GetCapacity(); j++ ) {
        auto col = m_event->GetHCofThisEvent()->GetHC( j );
        if ( !col ) {
          continue;
        }
        if ( std::string( col->GetName() ) == colname ) {
          if ( i >= 0 ) return nullptr;
          i = j;
        }
      }
    } else {
      for ( G4int j = 0; j < m_event->GetHCofThisEvent()->GetCapacity(); j++ ) {
        auto col = m_event->GetHCofThisEvent()->GetHC( j );
        if ( !col ) {
          continue;
        }
        std::string tgt = col->GetSDname();
        tgt += "/";
        tgt += col->GetName();
        if ( tgt == colname ) {
          if ( i >= 0 ) return nullptr;
          i = j;
        }
      }
    }
    if ( i < 0 ) {
      return nullptr;
    }
    return dynamic_cast<COL*>( m_event->GetHCofThisEvent()->GetHC( i ) );
  }

private:
  G4Event* m_event;
  GiGaWorkerPilot* m_vec{nullptr};
  Gaussino::MCTruth* m_truth{nullptr};
};

typedef std::vector<G4EventProxy> G4EventProxies;
