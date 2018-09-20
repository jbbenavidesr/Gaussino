// $Id: FlatNParticles.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
// Include files 

// from Event 
#include "Event/GenHeader.h"

// local
#include "FlatNParticles.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandomEngine.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlatNParticles
//
// 2008-05-19 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( FlatNParticles )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlatNParticles::FlatNParticles( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )  {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty ( "MinNParticles" , m_minNumberOfParticles = 1 ) ;
    declareProperty ( "MaxNParticles" , m_maxNumberOfParticles = 1 ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FlatNParticles::~FlatNParticles( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FlatNParticles::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  if ( m_minNumberOfParticles > m_maxNumberOfParticles ) 
    return Error( "Max number of particles < min number of particles !" ) ;
  else if ( 0 == m_maxNumberOfParticles ) 
    return Error( "Number of particles to generate set to zero !" ) ;
    
  info() << "Number of particles per event chosen randomly between " 
         << m_minNumberOfParticles << " and " << m_maxNumberOfParticles
         << endmsg ;  
         
  return sc ;
}

//=============================================================================
// Compute the number of particles
//=============================================================================
unsigned int FlatNParticles::numberOfPileUp( CLHEP::HepRandomEngine & engine ) {
  CLHEP::RandFlat flatGenerator{engine, 0, 1};
  return ( m_minNumberOfParticles + 
    (unsigned int) ( flatGenerator() * 
                     ( 1 + m_maxNumberOfParticles - m_minNumberOfParticles ) ) ) ;
}
