// $Id: FlatSmearVertex.cpp,v 1.4 2008-10-09 11:45:33 gcorti Exp $
// Include files 

// local
#include "FlatSmearVertex.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector4DTypes.h"

// from Event
#include "Event/HepMCEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlatSmearVertex
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FlatSmearVertex );


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
  
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize flat random number generator" ) ;

  release( randSvc ) ;
  return sc ;
}
 
//=============================================================================
// Smearing function
//=============================================================================
StatusCode FlatSmearVertex::smearVertex( LHCb::HepMCEvent * theEvent ) {
  double dx , dy , dz , dt ;
  
  dx = m_xmin + m_flatDist( ) * ( m_xmax - m_xmin ) ;
  dy = m_ymin + m_flatDist( ) * ( m_ymax - m_ymin ) ;
  dz = m_zmin + m_flatDist( ) * ( m_zmax - m_zmin ) ;
  dt = 0. ;

  Gaudi::LorentzVector dpos( dx , dy , dz , dt ) ;
  
  HepMC::GenEvent::vertex_iterator vit ;
  HepMC::GenEvent * pEvt = theEvent -> pGenEvt() ;
  for ( vit = pEvt -> vertices_begin() ; vit != pEvt -> vertices_end() ; 
        ++vit ) {
    Gaudi::LorentzVector pos ( (*vit) -> position() ) ;
    pos += dpos ;
    (*vit) -> set_position( HepMC::FourVector( pos.x() , pos.y() , pos.z() ,
                                               pos.t() ) ) ;
  }

  return StatusCode::SUCCESS ;      
}

