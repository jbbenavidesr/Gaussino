// Include file

#include <cmath>

 // from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// HepMC
#include "Event/HepMCEvent.h"

// local
#include "BoostForEpos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BoostForEpos
//
// 2016-10-21 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BoostForEpos )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BoostForEpos::BoostForEpos( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputHepMCEvent" , m_inputHepMCEvent =
                   LHCb::HepMCEventLocation::Default ) ;
  declareProperty( "p_x" , m_px = 0. ) ;  
  declareProperty( "p_y" , m_py = 0. ) ;  
  declareProperty( "p_z" , m_pz = 0. ) ;
}
//=============================================================================
// Destructor
//=============================================================================
BoostForEpos::~BoostForEpos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode BoostForEpos::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  double proton_mass = 0.938272 * Gaudi::Units::GeV ;
  Gaudi::LorentzVector vec( m_px , m_py, m_pz , std::sqrt( proton_mass * 
                                                           proton_mass + 
                                                           m_px * m_px + 
                                                           m_py * m_py + 
                                                           m_pz * m_pz ) ) ;

  m_boost = ROOT::Math::Boost( -vec.BoostToCM() ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BoostForEpos::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

    // loop over input HepMC and find EPOS event
  SmartDataPtr< LHCb::HepMCEvents > hepMCptr( eventSvc() , 
                                              m_inputHepMCEvent ) ;
  
  HepMC::GenEvent * theEPOSevent( 0 ) ;

  if ( 0 == hepMCptr ) {
    info() << "No HepMCEvents at location " << m_inputHepMCEvent 
           << endmsg ;
    return StatusCode::FAILURE ;
  } else {
    if ( hepMCptr->size() == 1 ) {
      theEPOSevent = (*hepMCptr)( 0 ) -> pGenEvt() ; 
    } else {
      theEPOSevent = (*hepMCptr)( 1 ) -> pGenEvt() ;
    }
  }

  // loop over particles and apply boost
  for ( HepMC::GenEvent::particle_iterator itP = theEPOSevent -> particles_begin() ;
          itP != theEPOSevent -> particles_end() ; ++itP ) {
    std::cout << "AVANT = " << (*itP) -> momentum().px() / Gaudi::Units::GeV 
              << " " << (*itP) -> momentum().py()  / Gaudi::Units::GeV
              << " " << (*itP) -> momentum().pz()  / Gaudi::Units::GeV
              << " " << (*itP) -> momentum().e()  / Gaudi::Units::GeV<< std::endl ;
    Gaudi::LorentzVector momentum( (*itP) -> momentum() ) ;
    Gaudi::LorentzVector newMomentum = m_boost( momentum ) ;
    (*itP) -> set_momentum( HepMC::FourVector( newMomentum.px() , 
                                               newMomentum.py() , 
                                               newMomentum.pz() , 
                                               newMomentum.e() ) ) ;
    std::cout << "APRES = " << (*itP) -> momentum().px()  / Gaudi::Units::GeV 
              << " " << (*itP) -> momentum().py()  / Gaudi::Units::GeV
              << " " << (*itP) -> momentum().pz()  / Gaudi::Units::GeV
              << " " << (*itP) -> momentum().e()  / Gaudi::Units::GeV << std::endl ;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BoostForEpos::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
