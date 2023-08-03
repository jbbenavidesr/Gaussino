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
// $Id: FixedNInteractions.cpp,v 1.5 2009-04-07 16:11:21 gcorti Exp $
// Include files

// local
#include "FixedNInteractions.h"

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// from Event
#include "Event/GenHeader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedNInteractions
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( FixedNInteractions )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedNInteractions::FixedNInteractions( const std::string& type, const std::string& name, const IInterface* parent )
    : GaudiTool( type, name, parent ) {
  declareInterface<IPileUpTool>( this );
  declareProperty( "NInteractions", m_nInteractions = 1 );
}

//=============================================================================
// Destructor
//=============================================================================
FixedNInteractions::~FixedNInteractions() {}

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedNInteractions::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  if ( 1 == m_nInteractions )
    info() << "Single Interaction Mode" << endmsg;
  else
    info() << "Fixed Number of Interactions per Event = : " << m_nInteractions << endmsg;
  return sc;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int FixedNInteractions::numberOfPileUp( HepRandomEnginePtr& ) { return m_nInteractions; }

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void FixedNInteractions::printPileUpCounters() {}
