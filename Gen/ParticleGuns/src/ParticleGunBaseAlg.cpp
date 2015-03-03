// $Id: ParticleGunBaseAlg.cpp,v 1.6 2006-04-04 08:38:46 gcorti Exp $
// -------------------------------------------------------------
//
// initial version by M. Shapiro
//
// LHCb version W. Pokorski

// local
#include "ParticleGunBaseAlg.h"

// 
#include "Event/HepMCEvent.h"
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"
#include "HepMC/GenEvent.h"

//===========================================================================
// Constructor
//===========================================================================
ParticleGunBaseAlg::ParticleGunBaseAlg(const std::string& name, 
                                       ISvcLocator* pSvcLocator) : 
  GaudiAlgorithm(name, pSvcLocator) 
{
  

  declareProperty("EventType"     , m_evnType = 0 ) ;
  declareProperty("HepMCEvents",    
                  m_eventLoc = LHCb::HepMCEventLocation::Default );
  declareProperty("GenHeader",      
                  m_headerLoc = LHCb::GenHeaderLocation::Default );
  declareProperty("GenCollisions",  
                  m_collLoc = LHCb::GenCollisionLocation::Default );
  
}

//===========================================================================
// Destructor
//===========================================================================
ParticleGunBaseAlg::~ParticleGunBaseAlg() { } 

//===========================================================================
// Initialization
//===========================================================================
StatusCode ParticleGunBaseAlg::initialize() {

  // Initialize the base class
  StatusCode sc = GaudiAlgorithm::initialize( ) ;  
  if ( sc.isFailure( ) ) return sc ;
  
  info() << "Generate event of type " << m_evnType << endmsg;

  return sc ;
}

//===========================================================================
// Execute method (Generate one event)
//===========================================================================
StatusCode ParticleGunBaseAlg::execute() {

  debug() << "Processing event type " << m_evnType << endmsg ;
  
  LHCb::HepMCEvents * anhepMCVector = new LHCb::HepMCEvents();
  put( anhepMCVector, m_eventLoc );
  
  LHCb::GenCollisions* collVector = new LHCb::GenCollisions();
  put( collVector, m_collLoc );

  // Create the HepMC event, store there the generated event and put it in
  // the container in the TES
  LHCb::HepMCEvent* mcevt = new LHCb::HepMCEvent() ;
  mcevt -> setGeneratorName( name() ) ;
  HepMC::GenEvent* evt = mcevt -> pGenEvt() ;

  // Generate an event
  StatusCode sc = callParticleGun( evt ) ;
  if( ! sc.isSuccess( ) ) return Error( "Failed to generate event" , sc ) ;

  anhepMCVector->insert( mcevt );
      
  // Create the GenCollision, fill it and put it in the TES
  // Do not fill hard scatter info!
  LHCb::GenCollision* coll = new LHCb::GenCollision();
  coll->setIsSignal( false ) ;
  coll->setProcessType( evt -> signal_process_id() ) ;
  coll->setEvent( mcevt ) ;
  collVector->insert( coll ) ;
    
  // Update the GenHeader 
  LHCb::GenHeader* genHead = get< LHCb::GenHeader >( m_headerLoc ) ;
  genHead->setEvType( m_evnType );
  genHead->setLuminosity( 0.0 );  // Ask Marco what he wants
  genHead->addToCollisions( coll );

  return StatusCode::SUCCESS ;  
}

