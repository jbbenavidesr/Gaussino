// $Id: RepeatDecay.cpp,v 1.1 2007-04-01 21:28:12 robbep Exp $
// Include files 

// local
#include "RepeatDecay.h"

// from Generators
#include "GenInterfaces/ISampleGenerationTool.h"
#include "GenEvent/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RepeatDecay
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( RepeatDecay )

//=======================================================
// Standard constructor, initializes variables
//=======================================================
RepeatDecay::RepeatDecay( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool  ( type, name , parent )  ,
    m_baseTool( 0 ) ,
    m_nRedecay( 0 )  { 
  declareInterface< ISampleGenerationTool >( this ) ;
  declareProperty( "BaseTool" , m_baseToolName = "Inclusive" ) ;
  declareProperty( "NRedecay" , m_nRedecayLimit = 50 ) ;
}

//=======================================================
// Destructor
//=======================================================
RepeatDecay::~RepeatDecay( ) { ; }

//=======================================================
// Initialize method
//=======================================================
StatusCode RepeatDecay::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize base class !" ) ;

  if ( "" == m_baseToolName ) 
    return Error( "No name given for Base Tool" ) ;

  info() << "Generating repeated events using base events of type "
         << m_baseToolName << endmsg ;

  m_baseTool = tool< ISampleGenerationTool >( m_baseToolName , this ) ;

  m_theMemorizedEvents.clear() ;
  m_theMemorizedCollisions.clear() ;

  return sc ;
}

//===================================================
// Generate Set of Event for Minimum Bias event type
//===================================================
bool RepeatDecay::generate( const unsigned int nPileUp , 
                            std::vector<HepMC::GenEvent> & theEvents , 
                            LHCb::GenCollisions & theCollisions ) {
  bool result = false ;

  if ( ( m_theMemorizedEvents.empty() ) || 
       ( m_nRedecay > m_nRedecayLimit ) ) {
    m_theMemorizedEvents.clear() ;
    m_theMemorizedCollisions.clear() ;
    m_nRedecay = 0 ;
    
    result = m_baseTool -> generate( nPileUp , theEvents , theCollisions ) ;  
    if ( result ) {
      // Erase decays before storing the event
      std::vector< HepMC::GenParticlePtr > eraseList ;
      
      //Loop over events
      for ( auto & event :  theEvents ) {
        //Loop over particles
        for ( auto & particle : event.particles() ) {
          // identify particles where switch to EvtGen occured
          if ( LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen ==
               particle -> status() )
            eraseList.push_back( particle ) ;
        }
      }
      
      for ( auto & particle : eraseList ) 
        HepMCUtils::RemoveDaughters( particle ) ;

      copyEvents( theEvents , m_theMemorizedEvents ) ;
      copyCollisions( theCollisions , m_theMemorizedCollisions , theEvents ) ;
    }
  } else {
    ++m_nRedecay ;
    
    copyEvents( m_theMemorizedEvents , theEvents ) ;
    copyCollisions( m_theMemorizedCollisions , theCollisions , 
                    m_theMemorizedEvents ) ;
    result = true ;
  }
  
  return result ;
}

//===================================================
// Print the counters
//===================================================
void RepeatDecay::printCounters( ) const {
  m_baseTool -> printCounters() ;
}

//===================================================
// Copy a HepMCEvent to another
//===================================================
void RepeatDecay::copyEvents( std::vector<HepMC::GenEvent> & from , 
                              std::vector<HepMC::GenEvent> & to ) {
  // Erase the event where to copy
  to.clear() ;

  for ( auto & event : from ) {
    LHCb::HepMCEvent * theHepMCEvent = new LHCb::HepMCEvent( ) ;
    theHepMCEvent -> setGeneratorName( (*it) -> generatorName() ) ;
    (*theHepMCEvent -> pGenEvt()) = (*(*it) -> pGenEvt()) ;
    to -> insert( theHepMCEvent ) ;
  } 
}

//===================================================
// Copy a HepMCEvent to another
//===================================================
void RepeatDecay::copyCollisions( LHCb::GenCollisions * from , 
                                  LHCb::GenCollisions * to ,
                                  LHCb::HepMCEvents * theEvents ) {
  // Erase the event where to copy
  to -> clear() ;

  LHCb::GenCollisions::iterator it ;
  LHCb::HepMCEvents::iterator itEvents = theEvents -> begin() ;
  
  for ( it = from -> begin() ; it != from -> end() ; ++it ) {
    LHCb::GenCollision * theGenCollision = new LHCb::GenCollision( ) ;
    theGenCollision -> setIsSignal( (*it) -> isSignal() ) ;
    theGenCollision -> setProcessType( (*it) -> processType() ) ;
    theGenCollision -> setSHat( (*it) -> sHat() ) ;
    theGenCollision -> setTHat( (*it) -> tHat() ) ;
    theGenCollision -> setUHat( (*it) -> uHat() ) ;
    theGenCollision -> setPtHat( (*it) -> ptHat() ) ;
    theGenCollision -> setX1Bjorken( (*it) -> x1Bjorken() ) ;
    theGenCollision -> setX2Bjorken( (*it) -> x2Bjorken() ) ;
    theGenCollision -> setEvent( (*itEvents) ) ;
    
    to -> insert( theGenCollision ) ;
    ++itEvents ;
  } 
}
