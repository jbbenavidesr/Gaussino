// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"

// from GaussRD
#include "GaussRD/IGaussRDCtr.h"

// local
#include "GaussRDCtrFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRDCtrFilter
//
// 
// 2016-03-15 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GaussRDCtrFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRDCtrFilter::GaussRDCtrFilter( const std::string& Name   ,
                                    ISvcLocator*       SvcLoc )
  : GaudiAlgorithm ( Name , SvcLoc ) 
  , m_gaussRDSvcName ( "GaussRD" ) 
  , m_gaussRDSvc     ( 0         )
{ 
  declareProperty( "GaussRD" , m_gaussRDSvcName ) ; 
}

//=============================================================================
// Destructor
//=============================================================================
GaussRDCtrFilter::~GaussRDCtrFilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRDCtrFilter::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if( sc.isFailure() ) { return sc ; }
  
  m_gaussRDSvc = svc<IGaussRDCtr>( m_gaussRDSvcName , true ) ;
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRDCtrFilter::execute() 
{  
  if ( nullptr == gaussRDSvc() ) 
  { m_gaussRDSvc = svc<IGaussRDCtr>( m_gaussRDSvcName , true ) ; }
  
  if ( nullptr == gaussRDSvc() ) 
  { return Error ( " execute(): IGaussRDCtr* points to NULL" ) ;}

  if (gaussRDSvc()->registerNewEvent()){
    setFilterPassed(true);
  } else {
    setFilterPassed(false);
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
