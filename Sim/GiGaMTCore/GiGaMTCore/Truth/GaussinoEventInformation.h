#pragma once
// ============================================================================
/// STL
#include <memory>
#include <string>
#include <vector>
/// Geant4
#include "Geant4/G4EventManager.hh"
#include "Geant4/G4VUserEventInformation.hh"
#include "GiGaMTCore/Truth/MCTruthConverter.h"

/** @class GaussinoTrackInformation GaussinoTrackInformation.h
 *
 *  Class to store auxilary information, mainly related to the creation
 *  and management of the truth record from Geant4. Replaces the use of
 *  singletons in old Gauss to achieve thread safety and data local to the
 *  event it refers to.
 *
 *  @author  Dominik Muller dominik.muller@cern.ch
 *  @date    07/02/2019
 */

class GaussinoEventInformation : public G4VUserEventInformation
{
public:
  GaussinoEventInformation( Gaussino::MCTruthTrackerPtr& converter ) : m_truthTrackerPtr{converter} {}
  /** No copy constructor allowed to avoid two objects referring to the
   * same conversion info by ptr because the G4 event is responsible for deleting this object.
   */
  GaussinoEventInformation( const GaussinoEventInformation& right ) = delete;
  GaussinoEventInformation( GaussinoEventInformation&& right ) : m_truthTrackerPtr{right.m_truthTrackerPtr} {};

  // Returns non-owning pointer to truth converter
  Gaussino::MCTruthTrackerPtr &TruthTracker() { return m_truthTrackerPtr; }

  inline static GaussinoEventInformation* Get( G4Event* event = nullptr )
  {
    if ( !event ) {
      event = G4EventManager::GetEventManager()->GetNonconstCurrentEvent();
    }
    auto info = event->GetUserInformation();
    if ( !info ) {
      G4cerr << "No G4UserEventInformation set. Returning a nullptr. Good luck.";
      return nullptr;
    }
    GaussinoEventInformation* finfo{nullptr};
    finfo = dynamic_cast<GaussinoEventInformation*>( info );
    if ( !finfo ) {
      // If cast failed we delete the existing info and create the correct one.
      // Though this indicates some problem.
      G4cerr << "Failed to cast G4UserEventInformation to GaussinoEventInformation. Returning a nullptr. Good luck.";
      return nullptr;
    } else {
      return finfo;
    }
  }

  virtual void Print() const override {};

private:
  Gaussino::MCTruthTrackerPtr m_truthTrackerPtr;
};
