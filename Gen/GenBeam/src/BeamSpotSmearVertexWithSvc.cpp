// $Id: BeamSpotSmearVertexWithSvc.cpp,v 1.12 2010-05-09 17:05:42 gcorti Exp $
// Include files 

// local
#include "BeamSpotSmearVertexWithSvc.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/PhysicalConstants.h" 

// from Event
#include "Event/HepMCEvent.h"
#include "GenBeam/IBeamInfoSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbAcceptance
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( BeamSpotSmearVertexWithSvc )


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
  
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_gaussDist.initialize( randSvc , Rndm::Gauss( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize gaussian random number generator" ) ;

  m_beaminfosvc = svc<IBeamInfoSvc>("BeamInfoSvc", true);
  if (!m_beaminfosvc) {
    return Error("Error retrieving the BeamInfosvc");
  }
    
  info() << "Smearing of interaction point with Gaussian distribution "
         << endmsg;

  release( randSvc ) ;
 
  return sc ;
}

//=============================================================================
// Smearing function
//=============================================================================
StatusCode BeamSpotSmearVertexWithSvc::smearVertex( HepMC::GenEvent * theEvent ) {

  double dx , dy , dz;
  
  do { dx = m_gaussDist( ) ; } while ( fabs( dx ) > m_xcut ) ;
  dx = dx * m_beaminfosvc -> sigmaX() + m_beaminfosvc -> beamSpot().x() ;
  do { dy = m_gaussDist( ) ; } while ( fabs( dy ) > m_ycut ) ;
  dy = dy * m_beaminfosvc -> sigmaY() + m_beaminfosvc -> beamSpot().y() ;
  do { dz = m_gaussDist( ) ; } while ( fabs( dz ) > m_zcut ) ;
  dz = dz * m_beaminfosvc -> sigmaZ() + m_beaminfosvc -> beamSpot().z() ;

  double meanT = m_timeSignVsT0 * m_beaminfosvc -> beamSpot().z() / Gaudi::Units::c_light ;

  HepMC::FourVector dpos( dx , dy , dz , meanT ) ;
  
  theEvent->shift_position_by(dpos);

  return StatusCode::SUCCESS ;      
}
