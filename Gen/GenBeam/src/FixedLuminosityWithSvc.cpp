// $Id: FixedLuminosityWithSvc.cpp,v 1.6 2009-04-07 16:11:21 gcorti Exp $
// Include files 

// local
#include "FixedLuminosityWithSvc.h"

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// From Event
#include "Event/BeamParameters.h"
#include "Event/GenFSR.h"
#include "Event/GenFSRMTManager.h"
#include "Event/GenCountersFSR.h"

// From Generators
#include "GenInterfaces/ICounterLogFile.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandPoisson.h"
#include "GenBeam/IBeamInfoSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedLuminosityWithSvc
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( FixedLuminosityWithSvc )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedLuminosityWithSvc::FixedLuminosityWithSvc( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_xmlLogTool ( 0 ) ,
    m_numberOfZeroInteraction( 0 ) ,
    m_nEvents( 0 ) {
    declareInterface< IPileUpTool >( this ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FixedLuminosityWithSvc::~FixedLuminosityWithSvc( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedLuminosityWithSvc::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  //  XMl log file
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;
  m_beaminfosvc = svc<IBeamInfoSvc>("BeamInfoSvc", true);

  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int FixedLuminosityWithSvc::numberOfPileUp( HepRandomEnginePtr & engine ) {
  auto genFSR = GenFSRMTManager::GetGenFSR();
  int key = 0;

  unsigned int result = 0 ;
  while ( 0 == result ) {
    m_nEvents++ ;
    key = LHCb::GenCountersFSR::CounterKeyToType("AllEvt");
    genFSR->incrementGenCounter(key,1);
    CLHEP::RandPoisson poissonGenerator{engine.getref(), m_beaminfosvc->nu()};
    result = (unsigned int) poissonGenerator() ;
    if ( 0 == result ) {
      m_numberOfZeroInteraction++ ;
      key =LHCb::GenCountersFSR::CounterKeyToType("ZeroInt");
      genFSR->incrementGenCounter(key, 1); 
    }
  }
  return result ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void FixedLuminosityWithSvc::printPileUpCounters( ) {
  //FIXME: Printout
  //printCounter( m_xmlLogTool , "all events (including empty events)", m_nEvents ) ;
  //printCounter( m_xmlLogTool , "events with 0 interaction" , 
                //m_numberOfZeroInteraction ) ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode FixedLuminosityWithSvc::finalize( ) {
  return GaudiTool::finalize( ) ;
}
