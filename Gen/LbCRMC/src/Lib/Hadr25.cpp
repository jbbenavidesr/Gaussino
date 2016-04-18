// $Id:Hadr25.cpp,v 1., 29-07-2015, Laure Massacrier $
// access epos common block Hadr25.h
#include "LbCRMC/Hadr25.h"

// set pointer to zero at start
Hadr25::HADR25* Hadr25::s_hadr25 = 0;

// Constructor
Hadr25::Hadr25() : m_realdummy( 0. ){ }

// Destructor
Hadr25::~Hadr25() { }

//access idprojin in COMMON
int& Hadr25::idprojin(){
 init();
 return s_hadr25 -> idprojin;
}

//access idtargin in COMMON
int& Hadr25::idtargin() {
  init() ;
  return s_hadr25 -> idtargin ;
}

//access rexdifi in COMMON
float& Hadr25::rexdifi( int n ){
 init();
 if ( n < 1 || n > 4 ) {
    m_realdummy = -999 ;
    return m_realdummy ;
 }
 return s_hadr25 -> rexdifi[n-1];
}

//access rexndii in COMMON
float& Hadr25::rexndii( int n ){
 init();
 if ( n < 1 || n > 4 ) {
    m_realdummy = -999 ;
    return m_realdummy ;
 }
 return s_hadr25 -> rexndii[n-1];
}

//access irdmpr in COMMON
int& Hadr25::irdmpr() {
  init() ;
  return s_hadr25 -> irdmpr ;
}

//access isoproj in COMMON
int& Hadr25::isoproj() {
  init() ;
  return s_hadr25 -> isoproj ;
}

//access isotarg in COMMON
int& Hadr25::isotarg() {
  init() ;
  return s_hadr25 -> isotarg ;
}
