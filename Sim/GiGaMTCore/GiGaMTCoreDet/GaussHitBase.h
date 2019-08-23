#pragma once
// Include files
#include "Geant4/G4VHit.hh"

/** @class Det/GaussHitBase Det/GaussHitBase.h GaussTools/Det/GaussHitBase.h
 *
 *
 *  @author Witold Pokorski
 *  @date   2002-10-29
 */

namespace Gaussino
{

  class HitBase : public G4VHit
  {
  public:
    /// Standard constructor
    inline HitBase() = default;

    virtual inline ~HitBase() = default; ///< Destructor

    inline void SetTrackID( G4int tid ) { m_trackID = tid; };
    inline G4int GetTrackID() const { return m_trackID; };

    /// Accessor in "Gaudi-style"
    inline G4int trackID() const { return m_trackID; }
    /// Accessor in "Gaudi-style"
    inline void setTrackID( const G4int id ) { m_trackID = id; }

  protected:
    G4int m_trackID;
  };
}
