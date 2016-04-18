// $Id:Accum.cpp,v 1., 29-07-2015, Laure Massacrier $
// access epos common block Accum.h
#include "LbCRMC/Accum.h"

// set pointer to zero at start
Accum::ACCUM* Accum::s_accum =0;

// Constructor
Accum::Accum() : m_dummy( 0 ) { }

// Destructor
Accum::~Accum() { }

//access imsg in COMMON
int& Accum::imsg(){
 init();
 return s_accum -> imsg;
}

//access jerr in COMMON
int& Accum::jerr(int n) {
  init() ;
  if ( n < 1 || n > lenmxjerr() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  return s_accum -> jerr[ n - 1 ] ;
}

//access ntevt in COMMON
int& Accum::ntevt(){
 init();
 return s_accum -> ntevt;
}

//access nrevt in COMMON
int& Accum::nrevt(){
 init();
 return s_accum -> nrevt;
}  

//access naevt in COMMON
int& Accum::naevt(){
 init();
 return s_accum -> naevt;
}  

//access nrstr in COMMON
int& Accum::nrstr(){
 init();
 return s_accum -> nrstr;
} 
  
//access nrptl in COMMON
int& Accum::nrptl(){
 init();
 return s_accum -> nrptl;
} 
