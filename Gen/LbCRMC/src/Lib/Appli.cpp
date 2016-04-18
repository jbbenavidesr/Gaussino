// $Id:Appli.cpp,v 1., 29-07-2015, Laure Massacrier $
// access epos common block Appli.h
#include "LbCRMC/Appli.h"

// set pointer to zero at start
Appli::APPLI* Appli::s_appli =0;

// Constructor
Appli::Appli(){ }

// Destructor
Appli::~Appli() { }

//access iappl in COMMON
int& Appli::iappl(){
 init();
 return s_appli -> iappl;
}

//access model in COMMON
int& Appli::model() {
  init() ;
  return s_appli -> model ;
}


