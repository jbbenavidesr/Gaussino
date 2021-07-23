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
#include "NewRnd/RndGlobal.h"

thread_local CLHEP::RandFlat* ThreadLocalgRandom::m_generator = nullptr;
thread_local HepRandomEnginePtr* ThreadLocalEngine::m_engine = nullptr;

//=============================================================================
// Throw a flat random numbers.
//=============================================================================
Double_t ThreadLocalgRandom::Rndm()
{
  return ( *m_generator )();
}

void ThreadLocalgRandom::RndmArray( Int_t n, Double_t* array )
{
  for ( Int_t i = 0; i < n; ++i ) array[i] = ( *m_generator )();
}

void ThreadLocalgRandom::RndmArray( Int_t n, Float_t* array )
{
  for ( Int_t i = 0; i < n; ++i ) array[i] = ( Float_t )( *m_generator )();
}
