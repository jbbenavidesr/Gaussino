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

// GiGa
#include "GiGaMTCoreDet/GaussHitBase.h"

// G4
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include <G4String.hh>

namespace CalorimeterCollector {
  class Hit : public Gaussino::HitBase {

  public:
    void* operator new( size_t );
    void operator delete( void* aHit );

    // methods from base class
    void Draw() override {}
    void Print() override;

    // methods to handle data
    void Add(G4double de, G4double dl);

    // get methods
    inline G4double GetEdep() const { return fEdep; };
    inline G4double GetTrackLength() const { return fTrackLength; };

    //Set Methods
    inline void SetLayerNumber( int layerNumber ) { fLayerNumber = layerNumber; };
    inline void SetEventID( int eventID ) { fEventID = eventID; };
    inline void SetDetectorName( G4String sensitiveDetector ) { fSensitiveDetector = sensitiveDetector; };


  private:
    G4double fEdep = 0.;  ///< Energy deposit in the sensitive volume
    G4double fTrackLength = 0.;  ///< Track length in the  sensitive volume
    int fLayerNumber = -1;
    int fEventID = -1;
    G4String fSensitiveDetector = "";

  };

  using HitsCollection = G4THitsCollection<Hit>;

  extern G4ThreadLocal G4Allocator<Hit>* HitAllocator;
} // namespace CalorimeterCollector
