#pragma once
// ============================================================================
/// STL
#include <string>
#include <vector>
/// Geant4
#include "Geant4/G4VUserTrackInformation.hh"
#include "Geant4/G4Allocator.hh"
#include "Geant4/G4TrackingManager.hh"
#include "Geant4/G4EventManager.hh"
/// GaussTools
#include "GiGaMTTruth/DetTrackInfo.h"
#include "GiGaMTCore/GaussHitBase.h"

/** @class GaussinoTrackInformation GaussinoTrackInformation.h
 *
 *  Class which is used to store auxilliary information about G4Track,
 *
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Dominik Muller dominik.muller@cern.ch
 *  @date    06/02/2019
 */

///
class GaussinoTrackInformation : public G4VUserTrackInformation {
  public:
  // the actual tyep of hit conatiner
  typedef std::vector<Gaussino::HitBase*> Hits;

  public:
  GaussinoTrackInformation() = default;
  /** copy constructor
   *  @param right object to be copied
   */
  GaussinoTrackInformation(const GaussinoTrackInformation& right);

  /// destructor
  virtual ~GaussinoTrackInformation(){ delete m_detInfo; };

  /// clone (virtual constructor)
  virtual GaussinoTrackInformation* clone() const;

  /// overloaded operator new
  inline void* operator new(size_t);

  /// overloaded operator delete
  inline void operator delete(void*);

  /// needed by base class
  void Print() const override{};

  /// flag to append the step
  inline bool appendStep() const { return m_appendStep; }

  /** set new value for flag to append step
   *  @param value new value of the flag
   */
  inline GaussinoTrackInformation& setAppendStep(const bool value) {
    m_appendStep = value;
    return *this;
  }

  /// flag to force the saving of track into traectory
  inline bool toBeStored() const { return m_toBeStored; }

  /** set new value for flag to force the saving track into trajectory
   *  @param value new value of the flag
   */
  inline GaussinoTrackInformation& setToBeStored(const bool value) {
    m_toBeStored = value;
    return *this;
  }

  /// track created a hit
  inline bool createdHit() const { return m_createdHit; }

  /** set new value for flag
   *  @param value new value of the flag
   */
  inline GaussinoTrackInformation& setCreatedHit(const bool value) {
    m_createdHit = value;
    return *this;
  }

  // Retrieve if direct parent particle has been stored or not
  inline bool directParent() const { return m_directParent; }

  // Set if direct parent particle has been stored or not
  inline GaussinoTrackInformation& setDirectParent(const bool value) {
    m_directParent = value;
    return *this;
  }

  /// Get flag to store or not in internal HepMC structure
  inline bool storeHepMC() const { return m_storeHepMC; }

  /// Set value of flag to store or not in internal HepMC structure
  inline GaussinoTrackInformation& setStoreHepMC(const bool value) {
    m_storeHepMC = value;
    return *this;
  }

  /** add hit pointer
   *  @param hit hit to be added into list of connected hits
   */
  GaussinoTrackInformation& addHit(Gaussino::HitBase* hit) {
    if (0 != hit) {
      m_hits.push_back(hit);
    };
    return *this;
  }

  /** add hit pointer
   *  @param hit hit to be added into list of connected hits
   */
  GaussinoTrackInformation& addToHits(Gaussino::HitBase* hit) { return addHit(hit); }

  // get the container of hits
  const Hits& hits() const { return m_hits; }

  /** update Tracks IDs in hits
   *  (set the new track ID for all connected hits)
   *  @param trackID new value of trackID
   */
  GaussinoTrackInformation& updateHitsTrackID(G4int trackID) {
    for (Hits::iterator ihit = m_hits.begin(); m_hits.end() != ihit; ++ihit) {
      Gaussino::HitBase* hit = *ihit;
      if (0 != hit) {
        hit->setTrackID(trackID);
      }
    }
    return *this;
  }

  /** get the pointer to the detInfo
   */
  DetTrackInfo* detInfo() const { return m_detInfo; }

  /** set the pointer to the detInfo
   *  @param aDetInfo pointer to DetTrackInfo
   */
  void setDetInfo(DetTrackInfo* aDetInfo) { m_detInfo = aDetInfo; }

  inline static G4Allocator<GaussinoTrackInformation>*
  GaussinoTrackInformationAllocator();

  /** Get the GaussinoTrackInformation object for a track. If no track
   * object has been assigned yet one will be created and assigned
   *  @param aDetInfo pointer to DetTrackInfo
   */
  inline static GaussinoTrackInformation* Get( G4Track* track = nullptr )
  {
    if ( !track ) {
      track = G4EventManager::GetEventManager()->GetTrackingManager()->GetTrack();
    }
    auto info = track->GetUserInformation();
    if ( !info ) {
      info = new GaussinoTrackInformation{};
      track->SetUserInformation( info );
    }
    GaussinoTrackInformation* finfo{nullptr};
    if ( info ) {
      finfo = dynamic_cast<GaussinoTrackInformation*>( info );
      if ( !finfo ) {
        // If cast failed we delete the existing info and create the correct one.
        // Though this indicates some problem.
        G4cerr << "Failed to cast existing UserTrackInformation. Overwriting existing one!";
        delete info;
      } else {
        return finfo;
      }
    }
    info = new GaussinoTrackInformation{};
    track->SetUserInformation( info );
    return dynamic_cast<GaussinoTrackInformation*>( info );
  }

private:
  /// flag indicating that TrajectoryPoint should be appended
  bool m_appendStep{false};
  /// flag indicating that track is forced to be stored into trajectory
  bool m_toBeStored{false};
  /// flag indicating that track created a hit
  bool m_createdHit{false};
  /// flag indicating that the direct parent particle was not stored
  /// in HepMC event this will be represented by a special 'dummy' link
  bool m_directParent{true};
  // flag indicating that the track should be stored in HepMC record
  bool m_storeHepMC{false};

  /// vector of pointers to hits created by that track
  Hits m_hits{};

  /// pointer to a specialised DetTrackInfo object containing detector-specific
  /// track information
  DetTrackInfo* m_detInfo{nullptr};
};

inline G4Allocator<GaussinoTrackInformation>*
GaussinoTrackInformation::GaussinoTrackInformationAllocator() {
  thread_local auto hitAllocator = G4Allocator<GaussinoTrackInformation>{};
  return &hitAllocator;
}

inline void* GaussinoTrackInformation::operator new(size_t) {
  return (void*)GaussinoTrackInformationAllocator()->MallocSingle();
}

inline void GaussinoTrackInformation::operator delete(void* info) {
  GaussinoTrackInformationAllocator()->FreeSingle((GaussinoTrackInformation*)info);
}
