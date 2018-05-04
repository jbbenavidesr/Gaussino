// $Id: FixedTarget.cpp,v 1.4 2007-02-08 17:46:06 gcorti Exp $
// Include files 

// local
#include "FixedTarget.h"

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// from Event
#include "Event/BeamParameters.h"
#include "GenEvent/BeamForInitialization.h"

#include "NewRnd/RndGlobal.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedTarget
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( FixedTarget )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedTarget::FixedTarget( const std::string& type, const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IBeamTool >( this ) ;
    declareProperty( "BeamParameters" , 
                     m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FixedTarget::~FixedTarget( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedTarget::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  info() << "Collision with fixed target" << endmsg ;

  return sc ;
}

//=============================================================================
// Mean value of the beam momentum
//=============================================================================
void FixedTarget::getMeanBeams( Gaudi::XYZVector & pBeam1 , 
                                Gaudi::XYZVector & pBeam2 )
  const {
  // Retrieve beam parameters from the static class
  LHCb::BeamParameters * beam = 
    BeamForInitialization::getInitialBeamParameters() ;
  if ( 0 == beam ) 
    Exception( "No beam parameters for initialization" ) ;

  double p1x, p1y, p1z ;
  p1x = beam -> energy() * sin( beam -> horizontalCrossingAngle() ) ;
  p1y = beam -> energy() * sin( beam -> verticalCrossingAngle()   ) ;
  p1z = beam -> energy() ;
  pBeam1.SetXYZ( p1x, p1y, p1z ) ;

  pBeam2.SetXYZ( 0., 0., 0. ) ;
}

//=============================================================================
// Current value of the smeared beams
//=============================================================================
void FixedTarget::getBeams( Gaudi::XYZVector & pBeam1 , 
                            Gaudi::XYZVector & pBeam2 ) {
  // Retrieve beam parameters
  LHCb::BeamParameters * beam = get< LHCb::BeamParameters >( m_beamParameters ) ;
  if ( 0 == beam ) Exception( "No beam parameters in TES" ) ;

  auto & engine = ThreadLocalEngine::Get();
  CLHEP::RandGauss gaussianDist{engine, 0, 1};

  double p1x, p1y, p1z ;
  p1x = beam -> energy() * sin( beam -> horizontalCrossingAngle() + 
                                gaussianDist() * beam -> angleSmear() ) ;
  p1y = beam -> energy() * sin( beam -> verticalCrossingAngle() + 
                                gaussianDist() * beam -> angleSmear() ) ;
  p1z = beam -> energy() ;
  pBeam1.SetXYZ( p1x, p1y, p1z ) ;
  
  pBeam2.SetXYZ( 0., 0., 0. ) ;
}
