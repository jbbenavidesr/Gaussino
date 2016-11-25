// local
#include "GaussRedecayFakePileUp.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// From GaussRedecay
#include "GaussRedecay/IGaussRedecayStr.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRedecayFakePileUp
//
// 2005-08-17 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( GaussRedecayFakePileUp )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRedecayFakePileUp::GaussRedecayFakePileUp( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ){
    declareInterface<IPileUpTool>(this);
    declareProperty("GaussRedecay", m_gaussRDSvcName = "GaussRedecay");
}

//=============================================================================
// Destructor 
//=============================================================================
GaussRedecayFakePileUp::~GaussRedecayFakePileUp( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode GaussRedecayFakePileUp::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  m_gaussRDStrSvc = svc<IGaussRedecayStr>(m_gaussRDSvcName, true);
  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int GaussRedecayFakePileUp::numberOfPileUp( ) {
  return m_gaussRDStrSvc->getNPileUp();
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode GaussRedecayFakePileUp::finalize( ) {
  if (m_gaussRDStrSvc != nullptr) release(m_gaussRDStrSvc);
  return GaudiTool::finalize( ) ;
}
