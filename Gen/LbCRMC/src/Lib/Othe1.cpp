// $Id:Othe1.cpp,v 1., 29-07-2015, Laure Massacrier $
// access epos common block Othe1.h
#include "LbCRMC/Othe1.h"

// set pointer to zero at start
Othe1::OTHE1* Othe1::s_othe1 =0;

// Constructor
Othe1::Othe1(){ }

// Destructor
Othe1::~Othe1() { }

//access istore in COMMON
int& Othe1::istore(){
 init();
 return s_othe1 -> istore;
}

//access istmax in COMMON
int& Othe1::istmax() {
  init() ;
  return s_othe1 -> istmax ;
}

//access gaumx in COMMON
float& Othe1::gaumx(){
 init();
 return s_othe1 -> gaumx;
}

//access irescl in COMMON
int& Othe1::irescl(){
 init();
 return s_othe1 -> irescl;
}  

//access ntrymx in COMMON
int& Othe1::ntrymx(){
 init();
 return s_othe1 -> ntrymx;
}  

//access nclean in COMMON
int& Othe1::nclean(){
 init();
 return s_othe1 -> nclean;
} 
  

//access iopdg in COMMON
int& Othe1::iopdg(){
 init();
 return s_othe1 -> iopdg;
}  

//access ioidch in COMMON
int& Othe1::ioidch(){
 init();
 return s_othe1 -> ioidch; 
}  

