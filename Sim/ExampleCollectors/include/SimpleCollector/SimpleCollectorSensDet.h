/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

// from Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Gaussino
#include "GiGaMTCoreMessage/IMessage.h"
#include "GiGaMTCoreRun/GaussinoTrackInformation.h"
#include "GiGaMTDetFactories/GiGaMTG4SensDetFactory.h"
#include "SimpleCollector/SimpleCollectorHit.h"

class G4FastHit;
class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;
class G4Track;

namespace SimpleCollector {
  class SensDet : public G4VSensitiveDetector, public virtual Gsino::Message {

  public:
    inline SensDet( const std::string& name ) : G4VSensitiveDetector( name ) { collectionName.insert( "Hits" ); }

    void Initialize( G4HCofThisEvent* ) override;
    void EndOfEvent( G4HCofThisEvent* ) override;

    virtual bool ProcessHits( G4Step*, G4TouchableHistory* ) override;

  private:
    HitsCollection* fHitsCollection;
  };

  template <class AnySensDet>
  class SensDetFactory : public GiGaMTG4SensDetFactory<AnySensDet> {

  public:
    using base_fac = GiGaMTG4SensDetFactory<AnySensDet>;
    using base_fac::base_fac;

    AnySensDet* construct() const override {
      auto sensdet = base_fac::construct();
      return sensdet;
    }
  };
} // namespace SimpleCollector
