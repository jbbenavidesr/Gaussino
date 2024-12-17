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
// ============================================================================
/// STL
#include <string>
#include <vector>
/// Geant4
#include "G4Allocator.hh"
#include "G4EventManager.hh"
#include "G4TrackingManager.hh"
#include "G4VUserTrackInformation.hh"
/// GaussTools
#include "GiGaMTCoreDet/GaussHitBase.h"
#include "GiGaMTCoreRun/DetTrackInfo.h"

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
  GaussinoTrackInformation( const GaussinoTrackInformation& right );

  /// destructor
  virtual ~GaussinoTrackInformation() { delete m_detInfo; };

  /// overloaded operator new
  inline void* operator new( size_t );

  /// overloaded operator delete
  inline void operator delete( void* );

  /// needed by base class
  void Print() const override{};

  /// flag to append the step
  inline bool appendStep() const { return m_appendStep; }

  /** set new value for flag to append step
   *  @param value new value of the flag
   */
  inline GaussinoTrackInformation& setAppendStep( const bool value ) {
    m_appendStep = value;
    return *this;
  }

  /// flag to force the saving of track into traectory
  inline bool                      storeTruth() const { return m_storeTruth; }
  inline GaussinoTrackInformation& setToStoreTruth( const bool value ) {
    m_storeTruth = value;
    return *this;
  }

  /// Preliminary flag to store the truth, used when flagging children while
  // processing the parent track
  inline bool                      prelStoreTruth() const { return m_prelStoreTruth; }
  inline GaussinoTrackInformation& setToPrelStoreTruth( const bool value ) {
    m_prelStoreTruth = value;
    return *this;
  }

  /// track created a hit
  inline bool createdHit() const { return m_createdHit; }

  /** set new value for flag
   *  @param value new value of the flag
   */
  inline GaussinoTrackInformation& setCreatedHit( const bool value ) {
    m_createdHit = value;
    return *this;
  }

  // Retrieve if direct parent particle has been stored or not
  inline bool directParent() const { return m_directParent; }

  // Set if direct parent particle has been stored or not
  inline GaussinoTrackInformation& setDirectParent( const bool value ) {
    m_directParent = value;
    return *this;
  }

  /** add hit pointer
   *  @param hit hit to be added into list of connected hits
   */
  GaussinoTrackInformation& addHit( Gaussino::HitBase* hit ) {
    if ( 0 != hit ) { m_hits.push_back( hit ); };
    return *this;
  }

  /** add hit pointer
   *  @param hit hit to be added into list of connected hits
   */
  GaussinoTrackInformation& addToHits( Gaussino::HitBase* hit ) { return addHit( hit ); }

  // get the container of hits
  const Hits& hits() const { return m_hits; }

  /** update Tracks IDs in hits
   *  (set the new track ID for all connected hits)
   *  @param trackID new value of trackID
   */
  GaussinoTrackInformation& updateHitsTrackID( G4int trackID ) {
    for ( Hits::iterator ihit = m_hits.begin(); m_hits.end() != ihit; ++ihit ) {
      Gaussino::HitBase* hit = *ihit;
      if ( 0 != hit ) { hit->setTrackID( trackID ); }
    }
    return *this;
  }

  /** get the pointer to the detInfo
   */
  DetTrackInfo* detInfo() const { return m_detInfo; }

  /** set the pointer to the detInfo
   *  @param aDetInfo pointer to DetTrackInfo
   */
  void setDetInfo( DetTrackInfo* aDetInfo ) { m_detInfo = aDetInfo; }

  bool isSuspendedAndSaved() const { return m_wasSuspended; }
  void suspendedAndSaved() { m_wasSuspended = true; }

  inline static G4Allocator<GaussinoTrackInformation>* GaussinoTrackInformationAllocator();

  /** Get the GaussinoTrackInformation object for a track. If no track
   * object has been assigned yet one will be created and assigned
   *  @param aDetInfo pointer to DetTrackInfo
   */
  inline static GaussinoTrackInformation* Get( const G4Track* track = nullptr ) {
    if ( !track ) { track = G4EventManager::GetEventManager()->GetTrackingManager()->GetTrack(); }
    auto                      info = track->GetUserInformation();
    GaussinoTrackInformation* finfo{ nullptr };
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
  bool m_appendStep{ false };
  /// flag indicating that track is forced to be stored into trajectory
  bool m_storeTruth{ false };
  /// flag indicating that track is forced to be stored into trajectory
  bool m_prelStoreTruth{ false };
  /// flag indicating that track created a hit
  bool m_createdHit{ false };
  /// flag indicating that the direct parent particle was not stored
  /// in HepMC event this will be represented by a special 'dummy' link
  bool m_directParent{ true };
  /// flag indicating that the this track was at some point suspended
  /// and already saved.
  bool m_wasSuspended{ false };

  /// vector of pointers to hits created by that track
  Hits m_hits{};

  /// pointer to a specialised DetTrackInfo object containing detector-specific
  /// track information
  DetTrackInfo* m_detInfo{ nullptr };
};

extern G4ThreadLocal G4Allocator<GaussinoTrackInformation>* aGaussinoTrackInformationAllocator;

inline void* GaussinoTrackInformation::operator new( size_t ) {
  if ( !aGaussinoTrackInformationAllocator ) {
    aGaussinoTrackInformationAllocator = new G4Allocator<GaussinoTrackInformation>;
  }
  return (void*)aGaussinoTrackInformationAllocator->MallocSingle();
}

inline void GaussinoTrackInformation::operator delete( void* info ) {
  aGaussinoTrackInformationAllocator->FreeSingle( (GaussinoTrackInformation*)info );
}
