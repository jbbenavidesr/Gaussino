#pragma once
// ============================================================================
/// STL
#include <memory>
#include <string>
#include <vector>
/// Geant4
#include "Geant4/G4VUserEventInformation.hh"
#include "GiGaMTTruth/MCTruthConverter.h"

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
  GaussinoEventInformation() { m_truthConverter = new Gaussino::MCTruthConverter{}; }
  /** No copy constructor allowed to avoid two objects referring to the
   * same conversion info by ptr because the G4 event is responsible for deleting this object.
   */
  GaussinoEventInformation( const GaussinoEventInformation& right ) = delete;
  GaussinoEventInformation( GaussinoEventInformation&& right )
  {
    m_truthConverter       = std::move( right.m_truthConverter );
    right.m_truthConverter = nullptr; // FIXME: probably redundant
  };

  // Returns non-owning pointer to truth converter
  Gaussino::MCTruthConverter* GetTruthConverter() { return m_truthConverter.get(); }

private:
  std::unique_ptr<Gaussino::MCTruthConverter> m_truthConverter{nullptr};
};
