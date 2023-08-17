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

// from Geant4
#ifdef CUSTOMSIM
#  include "G4VFastSimSensitiveDetector.hh"
#else
namespace MCCollector {
  class G4VFastSimSensitiveDetector {};
} // namespace MCCollector
#endif
#include "G4VSensitiveDetector.hh"

// from Gaussino
#include "GiGaMTCoreMessage/IMessage.h"
#include "GiGaMTCoreRun/GaussinoTrackInformation.h"
#include "GiGaMTDetFactories/GiGaMTG4SensDetFactory.h"
#include "MCCollector/MCCollectorHit.h"

class G4FastHit;
class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;
class G4Track;

namespace MCCollector {
  class SensDet : public G4VSensitiveDetector, public G4VFastSimSensitiveDetector, public virtual Gsino::Message {

  public:
    inline SensDet( const std::string& name ) : G4VSensitiveDetector( name ) { collectionName.insert( "Hits" ); }

    void Initialize( G4HCofThisEvent* ) override;
    void EndOfEvent( G4HCofThisEvent* ) override;

    virtual bool ProcessHits( G4Step*, G4TouchableHistory* ) override;
#ifdef CUSTOMSIM
    virtual bool ProcessHits( const G4FastHit*, const G4FastTrack*, G4TouchableHistory* ) override;
#endif

    inline void setRequireEDep( bool requireEDep ) { m_requireEDep = requireEDep; }
    inline void setOnlyForward( bool onlyForward ) { m_onlyForward = onlyForward; }
    inline void setOnlyAtBoundary( bool onlyAtBoundary ) { m_onlyAtBoundary = onlyAtBoundary; }
    inline void setPrintStats( bool printStats ) { m_printStats = printStats; }
    inline void setForceStoreTruth( bool forceStoreTruth ) { m_forceStoreTruth = forceStoreTruth; }

  protected:
    virtual MCCollector::Hit* RetrieveAndSetupHit( const G4Track*, G4TouchableHistory* );

    HitsCollection* m_col;

    bool m_requireEDep     = false;
    bool m_onlyForward     = true;
    bool m_onlyAtBoundary  = false;
    bool m_printStats      = false;
    bool m_forceStoreTruth = false;
  };

  template <class AnySensDet>
  class SensDetFactory : public GiGaMTG4SensDetFactory<AnySensDet> {

  protected:
    // Watch out: default dE/dx=0 true by default
    Gaudi::Property<bool> m_requireEDep{ this, "RequireEDep", false };

    // Only forward particles
    Gaudi::Property<bool> m_onlyForward{ this, "OnlyForward", true };

    // Only hits at the boundary
    Gaudi::Property<bool> m_onlyAtBoundary{ this, "OnlyAtBoundary", false };

    // Print additional information at the end of each event
    Gaudi::Property<bool> m_printStats{ this, "PrintStats", false };

    // Force storing info about tracks & particles
    Gaudi::Property<bool> m_forceStoreTruth{ this, "ForceStoreTruth", false };

  public:
    using base_fac = GiGaMTG4SensDetFactory<AnySensDet>;
    using base_fac::base_fac;

    AnySensDet* construct() const override {
      auto sensdet = base_fac::construct();
      sensdet->setRequireEDep( m_requireEDep.value() );
      sensdet->setOnlyForward( m_onlyForward.value() );
      sensdet->setOnlyAtBoundary( m_onlyAtBoundary.value() );
      sensdet->setPrintStats( m_printStats.value() );
      sensdet->setForceStoreTruth( m_forceStoreTruth.value() );
      return sensdet;
    }
  };
} // namespace MCCollector
