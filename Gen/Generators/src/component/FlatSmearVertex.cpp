// $Id: FlatSmearVertex.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
// Include files 

// local
#include "FlatSmearVertex.h"

// from Gaudi
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlatSmearVertex
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_COMPONENT( FlatSmearVertex )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlatSmearVertex::FlatSmearVertex( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IVertexSmearingTool >( this ) ;
    declareProperty( "xVertexMin" , m_xmin = 0.0 * Gaudi::Units::mm ) ;
    declareProperty( "xVertexMax" , m_xmax = 0.0 * Gaudi::Units::mm ) ;
    declareProperty( "yVertexMin" , m_ymin = 0.0 * Gaudi::Units::mm ) ;
    declareProperty( "yVertexMax" , m_ymax = 0.0 * Gaudi::Units::mm ) ;
    declareProperty( "zVertexMin" , m_zmin = 0.0 * Gaudi::Units::mm ) ;
    declareProperty( "zVertexMax" , m_zmax = 0.0 * Gaudi::Units::mm ) ;
    declareProperty( "BeamDirection", m_zDir = 0 );
    declareProperty( "Tilt", m_tilt = false );
    declareProperty( "TiltAngle", m_tiltAngle = -3.601e-3 );
    
}

//=============================================================================
// Destructor 
//=============================================================================
FlatSmearVertex::~FlatSmearVertex( ) { ; }

//=============================================================================
// Initialize 
//=============================================================================
StatusCode FlatSmearVertex::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  if ( m_xmin > m_xmax ) return Error( "xMin > xMax !" ) ;
  if ( m_ymin > m_ymax ) return Error( "yMin > yMax !" ) ;  
  if ( m_zmin > m_zmax ) return Error( "zMin > zMax !" ) ;
  
  sc = m_flatDist.initialize( randSvc , Rndm::Flat( 0. , 1. ) ) ;
  
  std::string infoMsg = " applying TOF of interaction with ";
  if ( m_zDir == -1 ) {
    infoMsg = infoMsg + "negative beam direction";
  } else if ( m_zDir == 1 ) {
    infoMsg = infoMsg + "positive beam direction";
  } else if ( m_zDir == 0 ) {
    infoMsg = " with TOF of interaction equal to zero ";
  } else {
    return Error("BeamDirection can only be set to -1 or 1, or 0 to switch off TOF");
  }

  info() << "Smearing of interaction point with flat distribution "
         << " in x, y and z " << endmsg;
  info() << infoMsg << endmsg;
  info() << " with " << m_xmin / Gaudi::Units::mm 
         << " mm <= x <= " << m_xmax / Gaudi::Units::mm << " mm, "
         << m_ymin / Gaudi::Units::mm << " mm <= y <= " 
         << m_ymax / Gaudi::Units::mm << " mm and "
         << m_zmin / Gaudi::Units::mm << " mm <= z <= " 
         << m_zmax / Gaudi::Units::mm << " mm." << endmsg;

  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize flat random number generator" ) ;

  release( randSvc ) ;
  return sc ;
}
 
//=============================================================================
// Smearing function
//=============================================================================
StatusCode FlatSmearVertex::smearVertex( HepMC::GenEvent * theEvent ) {
  double dx , dy , dz , dt ;
  
  dx = m_xmin + m_flatDist( ) * ( m_xmax - m_xmin ) ;
  dy = m_ymin + m_flatDist( ) * ( m_ymax - m_ymin ) ;
  dz = m_zmin + m_flatDist( ) * ( m_zmax - m_zmin ) ;
  dt = m_zDir * dz/Gaudi::Units::c_light ;

  Gaudi::LorentzVector dpos( dx , dy , dz , dt ) ;
  
  for ( auto & vtx : theEvent->vertices() ) {
    Gaudi::LorentzVector pos ( vtx -> position() ) ;
    pos += dpos ;

    if (m_tilt) {
      Gaudi::LorentzVector negT( 0, 0, (fabs(m_zmax-m_zmin)<1e-3 ? -m_zmax : 0), 0 );
      Gaudi::LorentzVector posT( 0, 0, (fabs(m_zmax-m_zmin)<1e-3 ? +m_zmax : 0), 0 );
      Gaudi::RotationX rotX( m_zmax==m_zmin ? m_tiltAngle : 0 );
      pos = pos + negT;
      pos = rotX(pos);
      pos = pos + posT;
    }
    vtx->set_position( HepMC::FourVector( pos.x(), pos.y(), pos.z(), pos.t() ) );
  }

  return StatusCode::SUCCESS ;      
}
