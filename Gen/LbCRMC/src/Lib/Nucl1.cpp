// $Id:Nucl1.cpp,v 1., 29-07-2015, Laure Massacrier $
// access epos common block Nucl1.h
#include "LbCRMC/Nucl1.h"

// set pointer to zero at start
Nucl1::NUCL1* Nucl1::s_nucl1 =0;

// Constructor
Nucl1::Nucl1(){ }

// Destructor
Nucl1::~Nucl1() { }

//access core in COMMON
float& Nucl1::core(){
 init();
 return s_nucl1 -> core;
}

//access fctrmx in COMMON
float& Nucl1::fctrmx() {
  init() ;
  return s_nucl1 -> fctrmx;
}

//access laproj in COMMON
int& Nucl1::laproj(){
 init();
 return s_nucl1 -> laproj;
}

//access maproj in COMMON
int& Nucl1::maproj(){
 init();
 return s_nucl1 -> maproj;
}  

//access latarg in COMMON
int& Nucl1::latarg(){
 init();
 return s_nucl1 -> latarg;
}  

//access matarg in COMMON
int& Nucl1::matarg(){
 init();
 return s_nucl1 -> matarg;
} 
  
