// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"

// from GaussRD
#include "GaussRD/IGaussRDStr.h"

// local
#include "GaussRDCopyToService.h"
#include "MCCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRDCopyToService
//
// 
// 2016-03-15 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GaussRDCopyToService )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRDCopyToService::GaussRDCopyToService( const std::string& Name   ,
                                    ISvcLocator*       SvcLoc )
  : GaudiAlgorithm ( Name , SvcLoc ) 
  , m_gaussRDSvcName ( "GaussRD" ) 
  , m_gaussRDSvc     ( 0         )
{ 
  declareProperty( "GaussRD" , m_gaussRDSvcName ) ; 
  declareProperty("Particles",
		  m_particlesLocation = LHCb::MCParticleLocation::Default,
		  "Location to place the MCParticles.");
  declareProperty("Vertices",
		  m_verticesLocation = LHCb::MCVertexLocation::Default,
		  "Location to place the MCVertices.");  
  declareProperty( "MCHitsLocation", m_hitsLocation = "",
                   "Location in TES where to put resulting MCHits" );
}

//=============================================================================
// Destructor
//=============================================================================
GaussRDCopyToService::~GaussRDCopyToService() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRDCopyToService::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if( sc.isFailure() ) { return sc ; }

  if( "" == m_hitsLocation ) {
    fatal() << "Property MCHitsLocation need to be set! " << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_gaussRDSvc = svc<IGaussRDStr>( m_gaussRDSvcName , true ) ;
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRDCopyToService::execute() 
{  
  if ( nullptr == gaussRDSvc() ) 
  { m_gaussRDSvc = svc<IGaussRDStr>( m_gaussRDSvcName , true ) ; }
  
  if ( nullptr == gaussRDSvc() ) 
  { return Error ( " execute(): IGaussRDCtr* points to NULL" ) ;}

  auto cloner = m_gaussRDSvc->mcCloner();

  //Stuff all the MCParticles in there
  auto m_particleContainer = get<LHCb::MCParticles>(m_particlesLocation);
  for(auto & part: *m_particleContainer){
    cloner->cloneMCP(part);
  }

  //Stuff all the MCVertices in there
  auto m_vertexContainer= get<LHCb::MCVertices>(m_verticesLocation);
  for(auto & vtx: *m_vertexContainer){
    cloner->cloneMCV(vtx);
  }

  //Stuff all the MCHits in there
  auto m_hitsContainer= get<LHCb::MCHits>(m_hitsLocation);
  for(auto & hit: *m_hitsContainer){
    cloner->cloneMCHit(hit);
  }


  return StatusCode::SUCCESS;
}


//=============================================================================
