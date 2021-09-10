/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// $Id: FixedLuminosity.cpp,v 1.6 2009-04-07 16:11:21 gcorti Exp $
// Include files 

// local
#include "FixedLuminosity.h"

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// From Event
#include "Event/BeamParameters.h"
#include "Event/GenFSR.h"
#include "Event/GenFSRMTManager.h"
#include "Event/GenCountersFSR.h"

// From Generators
#include "Generators/GenCounters.h"
#include "GenInterfaces/ICounterLogFile.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandPoisson.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedLuminosity
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( FixedLuminosity )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedLuminosity::FixedLuminosity( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_xmlLogTool ( 0 ) ,
    m_numberOfZeroInteraction( 0 ) ,
    m_nEvents( 0 ) {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty( "BeamParameters" , 
                     m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
    declareProperty ( "GenFSRLocation", m_FSRName =
                      LHCb::GenFSRLocation::Default);
}

//=============================================================================
// Destructor 
//=============================================================================
FixedLuminosity::~FixedLuminosity( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedLuminosity::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  //  XMl log file
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int FixedLuminosity::numberOfPileUp( HepRandomEnginePtr & engine ) {
  LHCb::BeamParameters * beam = get< LHCb::BeamParameters >( m_beamParameters ) ;
  if ( 0 == beam ) Exception( "No beam parameters registered" ) ;

  auto genFSR = GenFSRMTManager::GetGenFSR(m_FSRName);
  int key = 0;

  unsigned int result = 0 ;
  while ( 0 == result ) {
    m_nEvents++ ;
    key = LHCb::GenCountersFSR::CounterKeyToType("AllEvt");
    if(genFSR) genFSR->incrementGenCounter(key,1);
    CLHEP::RandPoisson poissonGenerator{engine.getref(), beam->nu()};
    result = (unsigned int) poissonGenerator() ;
    if ( 0 == result ) {
      m_numberOfZeroInteraction++ ;
      key =LHCb::GenCountersFSR::CounterKeyToType("ZeroInt");
      if(genFSR) genFSR->incrementGenCounter(key, 1); 
    }
  }
  return result ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void FixedLuminosity::printPileUpCounters( ) {
  using namespace GenCounters ;
  printCounter( m_xmlLogTool , "all events (including empty events)", m_nEvents ) ;
  printCounter( m_xmlLogTool , "events with 0 interaction" , 
                m_numberOfZeroInteraction ) ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode FixedLuminosity::finalize( ) {
  return GaudiTool::finalize( ) ;
}
