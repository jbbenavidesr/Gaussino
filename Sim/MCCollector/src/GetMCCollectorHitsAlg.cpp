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

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/Producer.h"
// GiGa TODO: modernize in Gauss-on-Gaussino
#include "GiGa/GiGaHitsByName.h"
#include "GiGa/IGiGaSvc.h"
#include "GiGaCnv/GiGaKineRefTable.h"
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
#include "GiGaCnv/IGiGaKineCnvSvc.h"
// local
#include "MCCollectorHit.h"
// LHCb
#include "Event/MCHit.h"
#include "Event/MCExtendedHit.h"

namespace MCCollector {
  class HitsAlg : public Gaudi::Functional::Producer<LHCb::MCHits()> {
  public:
    HitsAlg( const std::string& name, ISvcLocator* pSvcLocator )
        : Producer( name, pSvcLocator, KeyValue{"MCHitsLocation", ""} ) {}

    virtual LHCb::MCHits operator()() const override;

  protected:
    // TODO: modernize in Gauss-on-Gaussino
    ServiceHandle<IGiGaSvc> m_gigaSvc{this, "GiGaService", "GiGa"};
    // TODO: modernize in Gauss-on-Gaussino
    ServiceHandle<IGiGaKineCnvSvc> m_kineSvc{this, "KineCnvService", IGiGaCnvSvcLocation::Kine};

    Gaudi::Property<std::string>   m_colName{this, "CollectionName", ""};
  };
} // namespace MCCollector


LHCb::MCHits MCCollector::HitsAlg::operator()() const {

  LHCb::MCHits hits;
  // TODO: modernize in Gauss-on-Gaussino
  if ( !m_gigaSvc ) {
    error() << "IGiGaSvc* points to NULL" << endmsg;
    return hits;
  }

  // TODO: modernize in Gauss-on-Gaussino
  GiGaHitsByName col( m_colName.value() );
  *m_gigaSvc >> col; // also StatusCode sc = retrieveHitCollection( col );
                     // in TRY/CATCH&PRINT

  // TODO: modernize in Gauss-on-Gaussino
  if ( 0 == col.hits() ) { warning() << "The hit collection='" << m_colName.value() << "' is not found!" << endmsg; }

  // TODO: modernize in Gauss-on-Gaussino
  GiGaUtil::FastCast<G4VHitsCollection, HitsCollection> mccollectorhits;
  const HitsCollection*                                 hitCollection = mccollectorhits( col.hits() );
  if ( 0 == hitCollection ) {
    error() << "Wrong Collection type" << endmsg;
    return hits;
  }

  int numOfHits = hitCollection->entries();

  for ( int iG4Hit = 0; iG4Hit < numOfHits; ++iG4Hit ) {
    LHCb::MCExtendedHit* newHit = new LHCb::MCExtendedHit();
    auto                 g4Hit  = ( *hitCollection )[iG4Hit];
    Gaudi::XYZPoint      entry( g4Hit->GetEntryPos() );
    Gaudi::XYZVector     mom( g4Hit->GetMomentum() );
    newHit->setMomentum( mom );
    newHit->setEntry( entry );
    newHit->setEnergy( g4Hit->GetEdep() );
    newHit->setP( g4Hit->GetMomentum().mag() );
    // TODO: modernize in Gauss-on-Gaussino
    // fill reference to MCParticle using the Geant4->MCParticle table
    GiGaKineRefTable& table   = m_kineSvc->table();
    int               trackID = g4Hit->GetTrackID();
    if ( table[trackID].particle() ) {
      newHit->setMCParticle( table[trackID].particle() );
    } else {
      warning() << "No pointer to MCParticle for MCHit associated to G4 trackID: " << trackID << endmsg;
    }
    //
    hits.add( newHit );
  }

  return hits;
}

DECLARE_COMPONENT_WITH_ID( MCCollector::HitsAlg, "GetMCCollectorHitsAlg" )
