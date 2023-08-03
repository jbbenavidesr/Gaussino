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
// class GaussG4UserLimits
// (based on CMS code by P.Arce)
//

enum ULtype { ULMinEkine, ULMaxTime, ULMaxStepLength, ULMaxTrackLength, ULMinRange };

#include "G4UserLimits.hh"
#include "globals.hh"
#include <map>

class G4Track;
namespace Gaussino {
  typedef std::map<int, double>      mid;
  typedef std::multimap<int, ULtype> mmiul;

  class UserLimits : public G4UserLimits {
    friend bool operator==( const UserLimits&, const UserLimits& );
    friend bool operator!=( const UserLimits&, const UserLimits& );

  public: // with description
    UserLimits( const G4String& type );
    virtual ~UserLimits() = default;

  public: // with description
    const G4String& GetType() const;
    void            SetType( const G4String& type );
    // Set/Get type name for UserLimits.

    virtual G4double GetMaxAllowedStep( const G4Track& ) override;
    G4double         GetUserMaxTrackLength( const G4Track& ) override;
    G4double         GetUserMaxTime( const G4Track& ) override;
    G4double         GetUserMinEkine( const G4Track& ) override;
    G4double         GetUserMinRange( const G4Track& ) override;

    using G4UserLimits::SetMaxAllowedStep;
    virtual void SetMaxAllowedStep( G4double ustepMax, const G4int particleCode );
    using G4UserLimits::SetUserMaxTrackLength;
    virtual void SetUserMaxTrackLength( G4double utrakMax, const G4int particleCode );
    using G4UserLimits::SetUserMaxTime;
    virtual void SetUserMaxTime( G4double utimeMax, const G4int particleCode );
    using G4UserLimits::SetUserMinEkine;
    virtual void SetUserMinEkine( G4double uekinMin, const G4int particleCode );
    using G4UserLimits::SetUserMinRange;
    virtual void SetUserMinRange( G4double urangMin, const G4int particleCode );

    bool GetUserLimitSet( const G4int particleCode, ULtype ultype ) const;

  private:
    G4double GetCut( const G4Track& aTrack, const mid& partCutMap, const bool cutBelow );

  protected:             // with description
    mid fMaxStepLength;  // max allowed Step size in this volume per particle
    mid fMaxTrackLength; // max total track length per particle
    mid fMaxTime;        // max time per particle
    mid fMinEkine;       // min kinetic energy per particle
    mid fMinRange;       // min remaining range per particle (only for charged particles)
    // Keep track of which user limits are set, so that only the necessary processes are created. This will avoid
    // unnecessary invokations to processes at each G4Step
    mmiul fUserLimitSet;

    G4String fType;
  };

  /** comparison operator for UserLimits class
   *  @param ul1 the first UL to be compared
   *  @param ul2 the second UL to be compared
   *  @return the result of coomaprison operation
   */
  bool operator==( const Gaussino::UserLimits& ul1, const Gaussino::UserLimits& ul2 );

  /** comparison operator for UserLimits class
   *  @param ul1 the first UL to be compared
   *  @param ul2 the second UL to be compared
   *  @return the result of coomaprison operation
   */
  inline bool operator!=( const Gaussino::UserLimits& ul1, const Gaussino::UserLimits& ul2 ) { return !( ul1 == ul2 ); }

} // namespace Gaussino
#include "G4Track.hh"

inline const G4String& Gaussino::UserLimits::GetType() const { return fType; }

inline void Gaussino::UserLimits::SetType( const G4String& type ) { fType = type; }
