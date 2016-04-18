// access epos common cptl
#include "LbCRMC/Cptl.h"

// set pointer to zero at start
Cptl::CPTL* Cptl::s_cptl = 0;

// Constructor
Cptl::Cptl() : m_dummy( 0 ) , m_realdummy( 0. )  { }

// Destructor
Cptl::~Cptl() { }

//access to nptl in COMMON
int& Cptl::nptl() {
  init() ;
  return s_cptl -> nptl ;
}

//access to pptl in COMMON
float& Cptl::pptl( int i , int n ) {
  init() ;
  if ( i < 1 || i > 5 || n < 1 || n > lenmxptl() ) {
    m_realdummy = -999 ;
    return m_realdummy ;
  }
  // return s_cptl -> pptl[ i - 1 ][ n - 1 ] ;
  return s_cptl -> pptl[ n - 1 ][ i - 1 ] ;
}

//access to iorptl in COMMON
int& Cptl::iorptl( int n ) {
  init() ;
  if ( n < 1 || n > lenmxptl() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  // return s_cptl -> iorptl[ n - 1 ] ;
   return s_cptl -> iorptl[ n - 1 ] ;
}

//access to idptl in COMMON
int& Cptl::idptl( int n ) {
  init() ;
  if ( n < 1 || n > lenmxptl() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  // return s_cptl -> idptl[ n - 1 ] ;
  return s_cptl -> idptl[ n - 1 ] ;
}

//access to istptl in COMMON
int& Cptl::istptl( int n ) {
  init() ;
  if ( n < 1 || n > lenmxptl() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  // return s_cptl -> istptl[ n - 1 ] ;
   return s_cptl -> istptl[ n - 1 ] ;
}

//access to tivptl in COMMON
float& Cptl::tivptl( int i , int n ) {
  init() ;
  if ( i < 1 || i > 2 || n < 1 || n > lenmxptl() ) {
    m_realdummy = -999 ;
    return m_realdummy ;
  }
  // return s_cptl -> tivptl[ i - 1 ][ n - 1 ] ;
    return s_cptl -> tivptl[ n - 1 ][ i - 1 ] ;
}

//access to ifrptl in COMMON
int& Cptl::ifrptl( int i , int n ) {
  init() ;
  if ( i < 1 || i > 2 || n < 1 || n > lenmxptl() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  //  return s_cptl -> ifrptl[ i - 1 ][ n - 1 ] ;
  return s_cptl -> ifrptl[ n - 1 ][ i -1 ] ;
}


//access to jorptl in COMMON
int& Cptl::jorptl( int n ) {
  init() ;
  if ( n < 1 || n > lenmxptl() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  //  return s_cptl -> jorptl[ n - 1 ] ;
    return s_cptl -> jorptl[ n - 1 ] ;
}


//access to xorptl in COMMON
float& Cptl::xorptl( int i , int n ) {
  init() ;
  if ( i < 1 || i > 4 || n < 1 || n > lenmxptl() ) {
    m_realdummy = -999 ;
    return m_realdummy ;
  }
  // return s_cptl -> xorptl[ i - 1 ][ n - 1 ] ;
    return s_cptl -> xorptl[ n - 1 ][ i - 1 ] ;
}


//access to ibptl in COMMON
int& Cptl::ibptl( int i , int n ) {
  init() ;
  if ( i < 1 || i > 4 || n < 1 || n > lenmxptl() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  // return s_cptl -> ibptl[ i - 1 ][ n - 1 ] ;
   return s_cptl -> ibptl[ n - 1 ][ i -1 ] ;
}

//access to ityptl in COMMON
int& Cptl::ityptl( int n ) {
  init() ;
  if ( n < 1 || n > lenmxptl() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  //  return s_cptl -> ityptl[ n - 1 ] ;
    return s_cptl -> ityptl[ n - 1 ] ;
}

