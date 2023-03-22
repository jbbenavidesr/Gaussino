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

// Geant4
#include "G4FastHit.hh"
#include "G4FastTrack.hh"
#include "G4Navigator.hh"
#include "G4TouchableHandle.hh"

namespace Gaussino::CustomSimulation {
  // FIXME: this is just a dumb copy of G4FastSimHitMaker in order to introduce
  // a workaround
  class HitMaker {

  public:
    HitMaker();
    ~HitMaker();
    void        make( const G4FastHit& aHit, const G4FastTrack& aTrack );
    inline void SetNameOfWorldWithSD( const G4String& aName ) { fWorldWithSdName = aName; };

  private:
    G4TouchableHandle fTouchableHandle;
    G4Navigator*      fpNavigator;
    G4bool            fNaviSetup;
    G4String          fWorldWithSdName;
  };
} // namespace Gaussino::CustomSimulation
