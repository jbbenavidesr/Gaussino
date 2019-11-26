// $Id: BeamSpotSmearVertexWithSvc.cpp,v 1.12 2010-05-09 17:05:42 gcorti Exp $
// Include files 

// local
#include "BeamSpotSmearVertexWithSvc.h"

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h" 

// from Event
#include "GenBeam/IBeamInfoSvc.h"
#include "HepMC3/FourVector.h"
#include "HepMC3/GenEvent.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbAcceptance
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( BeamSpotSmearVertexWithSvc )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamSpotSmearVertexWithSvc::BeamSpotSmearVertexWithSvc( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IVertexSmearingTool >( this ) ;
    declareProperty( "Xcut" , m_xcut = 4. ) ; // times SigmaX 
    declareProperty( "Ycut" , m_ycut = 4. ) ; // times SigmaY
    declareProperty( "Zcut" , m_zcut = 4. ) ; // times SigmaZ
    declareProperty( "SignOfTimeVsT0", m_timeSignVsT0 = 0 ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
BeamSpotSmearVertexWithSvc::~BeamSpotSmearVertexWithSvc( ) { ; }

//=============================================================================
// Initialize 
//=============================================================================
StatusCode BeamSpotSmearVertexWithSvc::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  m_beaminfosvc = svc<IBeamInfoSvc>("BeamInfoSvc", true);
  if (!m_beaminfosvc) {
    return Error("Error retrieving the BeamInfosvc");
  }
    
  info() << "Smearing of interaction point with Gaussian distribution "
         << endmsg;
 
  return sc ;
}

//=============================================================================
// Smearing function
//=============================================================================
StatusCode BeamSpotSmearVertexWithSvc::smearVertex( HepMC3::GenEventPtr theEvent , HepRandomEnginePtr & engine ) {

  double dx , dy , dz;

  CLHEP::RandGauss gaussDist{engine.getref(), 0, 1};
  
  do { dx = gaussDist( ) ; } while ( fabs( dx ) > m_xcut ) ;
  dx = dx * m_beaminfosvc -> sigmaX() + m_beaminfosvc -> beamSpot().x() ;
  do { dy = gaussDist( ) ; } while ( fabs( dy ) > m_ycut ) ;
  dy = dy * m_beaminfosvc -> sigmaY() + m_beaminfosvc -> beamSpot().y() ;
  do { dz = gaussDist( ) ; } while ( fabs( dz ) > m_zcut ) ;
  dz = dz * m_beaminfosvc -> sigmaZ() + m_beaminfosvc -> beamSpot().z() ;

  double meanT = m_timeSignVsT0 * m_beaminfosvc -> beamSpot().z() / Gaudi::Units::c_light ;

  HepMC3::FourVector dpos( dx , dy , dz , meanT ) ;
  
  theEvent->shift_position_by(dpos);

  return StatusCode::SUCCESS ;      
}
