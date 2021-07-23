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
// Include files
// local
#include "Generators/LbTaula.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LbTaula
//
// 2013-05-31 : Patrick Robbe
//-----------------------------------------------------------------------------

extern "C" {
#ifdef WIN32
  void __stdcall SETTAULAOUTPUTUNIT( int * ) ;
#else
  void settaulaoutputunit_( int * ) ;
#endif
}

void LbTaula::setOutputUnit( int outputUnit ) {
#ifdef WIN32
  SETTAULAOUTPUTUNIT( &outputUnit ) ;
#else
  settaulaoutputunit_( &outputUnit ) ;
#endif
}
