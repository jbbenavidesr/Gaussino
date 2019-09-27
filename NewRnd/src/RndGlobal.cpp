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
