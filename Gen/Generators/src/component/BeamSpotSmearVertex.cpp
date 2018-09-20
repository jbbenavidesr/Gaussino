// $Id: BeamSpotSmearVertex.cpp,v 1.12 2010-05-09 17:05:42 gcorti Exp $
// Include files 

// local
#include "BeamSpotSmearVertex.h"

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h" 

// from HepMC
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

// from Event
#include "Event/BeamParameters.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbAcceptance
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( BeamSpotSmearVertex )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamSpotSmearVertex::BeamSpotSmearVertex( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IVertexSmearingTool >( this ) ;
    declareProperty( "Xcut" , m_xcut = 4. ) ; // times SigmaX 
    declareProperty( "Ycut" , m_ycut = 4. ) ; // times SigmaY
    declareProperty( "Zcut" , m_zcut = 4. ) ; // times SigmaZ
    declareProperty( "SignOfTimeVsT0", m_timeSignVsT0 = 0 ) ;
    declareProperty( "BeamParameters" , 
                     m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
BeamSpotSmearVertex::~BeamSpotSmearVertex( ) { ; }

//=============================================================================
// Initialize 
//=============================================================================
StatusCode BeamSpotSmearVertex::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
    
  info() << "Smearing of interaction point with Gaussian distribution "
         << endmsg;
 
  return sc ;
}

//=============================================================================
// Smearing function
//=============================================================================
StatusCode BeamSpotSmearVertex::smearVertex( HepMC::GenEvent * theEvent ,
                                             CLHEP::HepRandomEngine & engine ) {

  LHCb::BeamParameters * beamp = get< LHCb::BeamParameters >( m_beamParameters ) ;
  if ( 0 == beamp ) Exception( "No beam parameters registered" ) ;

  double dx , dy , dz;

  CLHEP::RandGauss gaussDist{engine, 0, 1};
  
  do { dx = gaussDist( ) ; } while ( fabs( dx ) > m_xcut ) ;
  dx = dx * beamp -> sigmaX() + beamp -> beamSpot().x() ;
  do { dy = gaussDist( ) ; } while ( fabs( dy ) > m_ycut ) ;
  dy = dy * beamp -> sigmaY() + beamp -> beamSpot().y() ;
  do { dz = gaussDist( ) ; } while ( fabs( dz ) > m_zcut ) ;
  dz = dz * beamp -> sigmaZ() + beamp -> beamSpot().z() ;

  double meanT = m_timeSignVsT0 * beamp -> beamSpot().z() / Gaudi::Units::c_light ;

  HepMC::FourVector dpos( dx , dy , dz , meanT ) ;

  theEvent->shift_position_by(dpos);

  return StatusCode::SUCCESS ;      
}

