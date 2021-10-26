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
// $Id: MinimumBias.cpp,v 1.4 2007-01-12 15:17:38 ranjard Exp $
// Include files 

// local
#include "MinimumBias.h"
// from Gaudi

#include "GenInterfaces/IProductionTool.h"

// Event 
#include "HepMC3/GenEvent.h"
#include "Event/GenCollision.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MinimumBias
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( MinimumBias )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MinimumBias::MinimumBias( const std::string & type , const std::string & name,
                          const IInterface * parent )
  : ExternalGenerator( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
MinimumBias::~MinimumBias( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode MinimumBias::initialize( ) {
  info() << "Generating Minimum Bias events." << endmsg ;
  return ExternalGenerator::initialize( ) ;
}

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool MinimumBias::generate( const unsigned int nPileUp , 
                            HepMC3::GenEventPtrs & theEvents , 
                            LHCb::GenCollisions & theCollisions, HepRandomEnginePtr & engine ) const {
  StatusCode sc ;
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC3::GenEventPtr theGenEvent( 0 ) ;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( &theEvents , &theCollisions , theGenEvent , 
                        theGenCollision ) ;
    
    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision , engine ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;
  } 
  return true ;
}
