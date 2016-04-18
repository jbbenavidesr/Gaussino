// $Id:C2evt.cpp,v 1., 29-07-2015, Laure Massacrier $
// access epos common block c2evt.h
#include "LbCRMC/C2evt.h"

// set pointer to zero at start
C2evt::C2EVT* C2evt::s_c2evt =0;

// Constructor
C2evt::C2evt(){ }

// Destructor
C2evt::~C2evt() { }


//access ng1evt in COMMON
int& C2evt::ng1evt() {
  init() ;
  return s_c2evt -> ng1evt ;
}

//access ng2evt in COMMON
int& C2evt::ng2evt() {
  init() ;
  return s_c2evt -> ng2evt ;
}

//access rglevt in COMMON
float& C2evt::rglevt(){
 init();
 return s_c2evt -> rglevt;
}

//access sglevt in COMMON
float& C2evt::sglevt(){
 init();
 return s_c2evt -> sglevt;
}

//access eglevt in COMMON
float& C2evt::eglevt(){
 init();
 return s_c2evt -> eglevt;
}

//access fglevt in COMMON
float& C2evt::fglevt(){
 init();
 return s_c2evt -> fglevt;
}

//access ikoevt in COMMON
int& C2evt::ikoevt() {
  init() ;
  return s_c2evt -> ikoevt ;
}

//access typevt in COMMON
float& C2evt::typevt(){
 init();
 return s_c2evt -> typevt;
}
