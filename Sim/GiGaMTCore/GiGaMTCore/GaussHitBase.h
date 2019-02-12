#pragma once
// Include files
#include "Geant4/G4VHit.hh"

/** @class GaussHitBase GaussHitBase.h GaussTools/GaussHitBase.h
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
    inline HitBase();

    virtual inline ~HitBase(); ///< Destructor

    inline void SetTrackID( G4int tid ) { m_trackID = tid; };
    inline G4int GetTrackID() { return m_trackID; };

    /// Accessor in "Gaudi-style"
    inline G4int trackID() const { return m_trackID; }
    /// Accessor in "Gaudi-style"
    inline void setTrackID( const G4int id ) { m_trackID = id; }

  private:
    G4int m_trackID;
  };
}
