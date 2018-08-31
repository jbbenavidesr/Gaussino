#include "G4SaveTestCalHits.h"

// FCCSW
#include "FCCTest/CalorimeterHit.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"

// Geant4
#include "Geant4/G4Event.hh"

DECLARE_COMPONENT( G4SaveTestCalHits )

StatusCode G4SaveTestCalHits::initialize()
{
  if ( extends::initialize().isFailure() ) {
    return StatusCode::FAILURE;
  }

  if ( m_calType.value().find( "ECal" ) != std::string::npos &&
       m_calType.value().find( "HCal" ) != std::string::npos ) {
    error() << "Wrong type of the calorimeter. Set the property 'caloType' to either 'ECal' or 'HCal'" << endmsg;
    return StatusCode::FAILURE;
  } else {
    info() << "Initializing a tool saving the outputs for the calorimeter type: " << m_calType << endmsg;
  }
  bookHistos();
  return StatusCode::SUCCESS;
}

StatusCode G4SaveTestCalHits::monitor( const G4Event& aEvent )
{
  std::lock_guard<std::mutex> guard(m_hist_lock);
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  FCCTest::CalorimeterHit* hit;
  double energyTotal;
  int hitNo;
  debug() << "There are " << collections->GetNumberOfCollections() << " hit collections." << endmsg;
  for ( int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++ ) {
    collect = collections->GetHC( iter_coll );
    if ( collect->GetName().find( m_calType ) != std::string::npos ) {
      size_t n_hit = collect->GetSize();
      energyTotal  = 0;
      hitNo        = 0;
      debug() << "\t" << n_hit << " hits are stored in a calorimeter collection #" << iter_coll << ": "
                << collect->GetName() << endmsg;
      for ( size_t iter_hit = 0; iter_hit < n_hit; iter_hit++ ) {
        hit = dynamic_cast<FCCTest::CalorimeterHit*>( collect->GetHit( iter_hit ) );
        if ( hit->GetXid() != -1 && hit->GetYid() != -1 && hit->GetZid() != -1 ) {
          if ( hit->GetEdep() != 0 ) hitNo++;
          m_hitX->fill( hit->GetPos().x() );
          m_hitY->fill( hit->GetPos().y() );
          m_hitZ->fill( hit->GetPos().z() );
          m_hitEnergy->fill( hit->GetEdep() );
          energyTotal += hit->GetEdep();
        }
      }
      std::cout << "\t" << hitNo << " hits are non-zero in collection #" << iter_coll << ": " << collect->GetName()
                << std::endl;
      std::cout << "\t" << energyTotal << " MeV = total energy stored" << std::endl;
      m_totHitEnergy->fill( energyTotal );
    }
  }
  return StatusCode::SUCCESS;
}

void G4SaveTestCalHits::bookHistos()
{

  debug() << "==> Book histograms" << endmsg;

  m_totHitEnergy = book( 101, "hit energy", 50, 2000, 3000 );
  m_hitEnergy    = book( 102, "hit energy", 50, 0, 100 );
  m_hitX         = book( 103, "position x of hit", 50, -50, 50 );
  m_hitY         = book( 104, "position y of hit", 50, -50, 50 );
  m_hitZ         = book( 105, "position z of hit", 50, 0, 100 );

  return;
}
