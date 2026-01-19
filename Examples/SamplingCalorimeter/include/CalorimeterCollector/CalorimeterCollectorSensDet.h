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

// from Gaussino
#include "GiGaMTCoreMessage/IMessage.h"
#include "GiGaMTDetFactories/GiGaMTG4SensDetFactory.h"
#include "CalorimeterCollector/CalorimeterCollectorHit.h"

class G4FastHit;
class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;
class G4Track;

namespace CalorimeterCollector {
  class SensDet : public G4VSensitiveDetector, public virtual Gsino::Message {

  public:
    inline SensDet( const std::string& name ) : G4VSensitiveDetector( name ) { collectionName.insert( "Hits" ); }

    void Initialize( G4HCofThisEvent* ) override;
    void EndOfEvent( G4HCofThisEvent* ) override;

    virtual bool ProcessHits( G4Step*, G4TouchableHistory* ) override;

    inline void setRequiredNofCells( int NofCells ) { fNofCells = NofCells; };

  private:
    HitsCollection* fHitsCollection;
    G4int fNofCells = 0;
  };

  template <class AnySensDet>
  class SensDetFactory : public GiGaMTG4SensDetFactory<AnySensDet> {

  protected:
    // Specify the number of layers in the current geometry
    Gaudi::Property<int> m_NofLayers{ this, "NofLayers", 5 };


  public:
    using base_fac = GiGaMTG4SensDetFactory<AnySensDet>;
    using base_fac::base_fac;

    AnySensDet* construct() const override {
      auto sensdet = base_fac::construct();
      sensdet->setRequiredNofCells( m_NofLayers.value() );
      return sensdet;
    }


  };
} // namespace CalorimeterCollector
