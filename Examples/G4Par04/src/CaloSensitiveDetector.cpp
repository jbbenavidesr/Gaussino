/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"

// Gaussino
#include "CaloG4Hit.h"
#include "EventInformation.h"
#include "GiGaMTCoreRun/GaussinoTrackInformation.h"
#include "GiGaMTDetFactories/GiGaMTG4SensDetFactory.h"

// Geant4
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VFastSimSensitiveDetector.hh"
#include "G4VSensitiveDetector.hh"

namespace Gaussino::G4Par04 {
  struct CaloSensDet final : public G4VSensitiveDetector,
                             public G4VFastSimSensitiveDetector,
                             public virtual GiGaMessage {
    CaloSensDet( G4String aName );
    virtual void   Initialize( G4HCofThisEvent* ) override;
    virtual void   EndOfEvent( G4HCofThisEvent* ) override;
    virtual G4bool ProcessHits( G4Step*, G4TouchableHistory* ) override;
    virtual G4bool ProcessHits( const G4FastHit*, const G4FastTrack*, G4TouchableHistory* ) override;
    CaloG4Hit*     RetrieveAndSetupHit( G4ThreeVector, const G4Track* );

    /// Number of mesh readout cells in cylindrical coordinates
    G4ThreeVector fMeshNbOfCells = { 10, 10, 10 };
    /// Size of mesh readout cells in cylindrical coordinates.
    G4ThreeVector fMeshSizeOfCells = { 1 * m, 2 * CLHEP::pi / 10., 1 * m };

  private:
    CaloHitsCollection*          fHitsCollection = nullptr;
    std::map<size_t, CaloG4Hit*> m_hitmap        = {};
    /// ID of collection of hits
    G4int fHitCollectionID = -1;
  };

  using BaseFactory = GiGaMTG4SensDetFactory<CaloSensDet>;
  struct CaloSensDetFactory : public BaseFactory {

    using BaseFactory::BaseFactory;

    CaloSensDet* construct() const override {
      auto sensdet = BaseFactory::construct();
      sensdet->fMeshNbOfCells =
          G4ThreeVector{ (double)m_nbOfRhoCells.value(), (double)m_nbOfPhiCells.value(), (double)m_nbOfZCells.value() };
      sensdet->fMeshSizeOfCells = G4ThreeVector{
          m_sizeOfRhoCells.value(), 2. * Gaudi::Units::pi / m_nbOfPhiCells.value(), m_sizeOfZCells.value() };
      return sensdet;
    }

  private:
    // 2.325 mm of tungsten =~ 0.25 * 9.327 mm = 0.25 * R_Moliere
    Gaudi::Property<double> m_sizeOfRhoCells{ this, "SizeOfRhoCells", 2.325 * Gaudi::Units::mm };
    // 2 * 1.4 mm of tungsten =~ 0.65 X_0
    Gaudi::Property<double> m_sizeOfZCells{ this, "SizeOfZCells", 3.4 * Gaudi::Units::mm };
    Gaudi::Property<size_t> m_nbOfRhoCells{ this, "NbOfRhoCells", 18 };
    Gaudi::Property<size_t> m_nbOfPhiCells{ this, "NbOfPhiCells", 50 };
    Gaudi::Property<size_t> m_nbOfZCells{ this, "NbOfZCells", 45 };
  };
} // namespace Gaussino::G4Par04

DECLARE_COMPONENT( Gaussino::G4Par04::CaloSensDetFactory )

Gaussino::G4Par04::CaloSensDet::CaloSensDet( G4String aName ) : G4VSensitiveDetector( aName ) {
  collectionName.insert( "Hits" );
}

void Gaussino::G4Par04::CaloSensDet::Initialize( G4HCofThisEvent* aHCE ) {
  fHitsCollection = new CaloHitsCollection( SensitiveDetectorName, collectionName[0] );
  if ( fHitCollectionID < 0 ) { fHitCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID( fHitsCollection ); }
  aHCE->AddHitsCollection( fHitCollectionID, fHitsCollection );
}

void Gaussino::G4Par04::CaloSensDet::EndOfEvent( G4HCofThisEvent* ) { m_hitmap.clear(); }

G4bool Gaussino::G4Par04::CaloSensDet::ProcessHits( G4Step* aStep, G4TouchableHistory* ) {
  G4double edep = aStep->GetTotalEnergyDeposit();
  if ( edep == 0. ) return true;

  const auto* track = aStep->GetTrack();
  auto        hit   = RetrieveAndSetupHit( aStep->GetPostStepPoint()->GetPosition(), track );
  if ( !hit ) return true;

  // Add energy deposit from G4Step
  hit->AddEdep( edep );

  // Fill time information from G4Step
  // If it's already filled, choose hit with earliest global time
  if ( hit->GetTime() == -1 || hit->GetTime() > aStep->GetTrack()->GetGlobalTime() ) {
    hit->SetTime( aStep->GetTrack()->GetGlobalTime() );
  }

  // Set hit type to full simulation (only if hit is not already marked as fast
  // sim)
  if ( hit->GetType() != 1 ) hit->SetType( 0 );

  return true;
}

G4bool Gaussino::G4Par04::CaloSensDet::ProcessHits( const G4FastHit* aHit, const G4FastTrack* aTrack,
                                                    G4TouchableHistory* ) {
  G4double edep = aHit->GetEnergy();
  if ( edep == 0. ) return true;

  const auto* track = aTrack->GetPrimaryTrack();
  auto        hit   = RetrieveAndSetupHit( aHit->GetPosition(), track );
  if ( !hit ) return true;

  // Add energy deposit from G4FastHit
  hit->AddEdep( edep );

  // Fill time information from G4FastTrack
  // If it's already filled, choose hit with earliest global time
  if ( hit->GetTime() == -1 || hit->GetTime() > aTrack->GetPrimaryTrack()->GetGlobalTime() ) {
    hit->SetTime( aTrack->GetPrimaryTrack()->GetGlobalTime() );
  }

  // Set hit type to fast simulation (even if hit was already marked as full
  // sim, overwrite it)
  hit->SetType( 1 );

  return true;
}

Gaussino::G4Par04::CaloG4Hit* Gaussino::G4Par04::CaloSensDet::RetrieveAndSetupHit( G4ThreeVector  aGlobalPosition,
                                                                                   const G4Track* track ) {
  auto eventInfo = dynamic_cast<EventInformation*>( EventInformation::Get() );
  if ( !eventInfo ) {
    throw GaudiException( "No EventInformation available! Something is off with the configuration...",
                          "Gaussino::G4Par04::CaloSensDet", StatusCode::FAILURE );
  };

  auto trackInfo = GaussinoTrackInformation::Get();
  if ( !trackInfo ) {
    throw GaudiException( "No TrackInformation available! Something is off with the configuration...",
                          "Gaussino::G4Par04::CaloSensDet", StatusCode::FAILURE );
  };

  int trackID  = track->GetTrackID();
  int strackID = track->GetParentID();
  if ( !trackInfo->prelStoreTruth() && !trackInfo->storeTruth() ) { trackID = strackID; }

  auto collHit           = eventInfo->GetCollectorHit( trackID );
  auto entrancePosition  = collHit->GetPosition();
  auto entranceDirection = collHit->GetDirection();

  auto delta = aGlobalPosition - entrancePosition;

  // Calculate rotation matrix along the particle momentum direction
  // It will rotate the shower axes to match the incoming particle direction
  G4RotationMatrix rotMatrix     = G4RotationMatrix();
  double           particleTheta = entranceDirection.theta();
  double           particlePhi   = entranceDirection.phi();
  rotMatrix.rotateZ( -particlePhi );
  rotMatrix.rotateY( -particleTheta );
  G4RotationMatrix rotMatrixInv = CLHEP::inverseOf( rotMatrix );
  delta                         = rotMatrix * delta;
  G4int rhoNo                   = std::floor( delta.perp() / fMeshSizeOfCells.x() );
  G4int phiNo                   = std::floor( ( CLHEP::pi + delta.phi() ) / fMeshSizeOfCells.y() );
  G4int zNo                     = std::floor( delta.z() / fMeshSizeOfCells.z() );

  if ( zNo >= fMeshNbOfCells.z() || rhoNo >= fMeshNbOfCells.x() || zNo < 0 ) { return nullptr; }

  // each id must be < 256 (apart from trackID)
  size_t hitID = (size_t)trackID << 32 | (size_t)phiNo << 16 | (size_t)rhoNo << 8 | (size_t)zNo;

  auto hit = m_hitmap[hitID];
  if ( !hit ) {
    hit = new CaloG4Hit;
    hit->SetRhoId( rhoNo );
    hit->SetPhiId( phiNo );
    hit->SetZId( zNo );
    hit->SetRot( rotMatrixInv );
    hit->SetPos( entrancePosition + rotMatrixInv * G4ThreeVector( 0, 0, ( zNo + 0.5 ) * fMeshSizeOfCells.z() ) );
    hit->SetTrackID( trackID );
    trackInfo->setCreatedHit( true );
    trackInfo->addHit( hit );
    m_hitmap[hitID] = hit;
    fHitsCollection->insert( hit );
  }
  return hit;
}
