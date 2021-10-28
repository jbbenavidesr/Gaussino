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
//#include "GaussTools/GaussTrackInformation.h"

// local
#include "GiGaMTCoreMessage/IGiGaMessage.h"
#include "GiGaMTCoreTruth/GaussinoTrackInformation.h"
#include "GiGaMTFactories/GiGaMTG4SensDetFactory.h"
#include "MCCollectorHit.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// from Gaudi
#include "Gaudi/Accumulators.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Geant4
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4TouchableHistory.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VSensitiveDetector.hh"
#include "Geant4/G4ios.hh"

namespace MCCollector {
  class SensDet : public G4VSensitiveDetector, public virtual GiGaMessage {

  public:
    inline SensDet( const std::string& name ) : G4VSensitiveDetector( name ) { collectionName.insert( "Hits" ); }

    void Initialize( G4HCofThisEvent* ) override;
    void EndOfEvent( G4HCofThisEvent* ) override;

    bool ProcessHits( G4Step*, G4TouchableHistory* ) override;

    inline void setRequireEDep( bool requireEDep ) { m_requireEDep = requireEDep; }
    inline void setOnlyForward( bool onlyForward ) { m_onlyForward = onlyForward; }
    inline void setOnlyAtBoundary( bool onlyAtBoundary ) { m_onlyAtBoundary = onlyAtBoundary; }

    Gaudi::Accumulators::SummingCounter<>* m_hits_no{nullptr};
    Gaudi::Accumulators::SummingCounter<>* m_energy{nullptr};

  protected:
    HitsCollection* m_col;

    bool m_requireEDep    = false;
    bool m_onlyForward    = true;
    bool m_onlyAtBoundary = false;
  };

  class SensDetFactory : public GiGaMTG4SensDetFactory<SensDet> {

  protected:
    mutable Gaudi::Accumulators::SummingCounter<> m_hits_no{this, "#collector hits"};
    mutable Gaudi::Accumulators::SummingCounter<> m_energy{this, "#collector energy"};

    // Watch out: default dE/dx=0 true by default
    Gaudi::Property<bool> m_requireEDep{this, "RequireEDep", false};

    // Only forward particles
    Gaudi::Property<bool> m_onlyForward{this, "OnlyForward", true};

    // Only hits at the boundary
    Gaudi::Property<bool> m_onlyAtBoundary{this, "OnlyAtBoundary", false};

  public:
    using base_fac = GiGaMTG4SensDetFactory<SensDet>;
    using base_fac::base_fac;

    SensDet* construct() const override {
      auto sensdet = base_fac::construct();
      sensdet->setRequireEDep( m_requireEDep.value() );
      sensdet->setOnlyForward( m_onlyForward.value() );
      sensdet->setOnlyAtBoundary( m_onlyAtBoundary.value() );
      sensdet->m_hits_no = &m_hits_no;
      sensdet->m_energy  = &m_energy;
      return sensdet;
    }
  };
} // namespace MCCollector

DECLARE_COMPONENT_WITH_ID( MCCollector::SensDetFactory, "MCCollectorSensDet" )

void MCCollector::SensDet::Initialize( G4HCofThisEvent* HCE ) {

  m_col = new HitsCollection( SensitiveDetectorName, collectionName[0] );

  int HCID = G4SDManager::GetSDMpointer()->GetCollectionID( SensitiveDetectorName + "/" + collectionName[0] );

  HCE->AddHitsCollection( HCID, m_col );

  // standard print left as is
  debug( " Initialize(): CollectionName='" + m_col->GetName() + "' for SensDet='" + m_col->GetSDname() + "'" );
}

bool MCCollector::SensDet::ProcessHits( G4Step* step, G4TouchableHistory* /* history */ ) {
  if ( 0 == step ) return false;

  auto track = step->GetTrack();

  // If required to have energy deposition check
  double edep = step->GetTotalEnergyDeposit();
  if ( m_requireEDep && edep <= 0.0 ) return false;
  if ( step->GetStepLength() == 0. ) return false;

  auto preStep = step->GetPreStepPoint();

  // do not store hits created by particles moving backwards
  auto premom = preStep->GetMomentum();
  if ( m_onlyForward && premom.z() < 0. ) return false;

  // do not store hits created not at the boundary
  if ( m_onlyAtBoundary && preStep->GetStepStatus() != 1 ) return false;

  auto prepos = preStep->GetPosition();
  auto newHit = new MCCollector::Hit();
  newHit->SetEdep( edep );
  newHit->SetEntryPos( prepos );
  newHit->SetMomentum( premom );
  // track info
  int trid = track->GetTrackID();
  newHit->SetTrackID( trid );
  auto ui = track->GetUserInformation();
  auto gi = (GaussinoTrackInformation*)ui;
  gi->setCreatedHit( true );
  gi->setToStoreTruth( true );
  gi->addHit( newHit );

  m_col->insert( newHit );
  return true;
}

void MCCollector::SensDet::EndOfEvent( G4HCofThisEvent* /* HCE */ ) {
  int    hits_no = 0;
  double energy  = 0.;

  std::vector<MCCollector::Hit*>* hits = m_col->GetVector();
  for ( auto& hit : *hits ) {
    hits_no++;
    energy += hit->GetEdep();
  }

  debug( boost::str( boost::format( "%s  #Hits=%5d Energy=%8.3g[GeV] " ) % m_col->GetSDname() % hits_no %
                     ( energy / Gaudi::Units::GeV ) ) );
  if ( m_hits_no && m_energy ) {
    ( *m_hits_no ) += hits_no;
    ( *m_energy ) += energy;
  }
}
