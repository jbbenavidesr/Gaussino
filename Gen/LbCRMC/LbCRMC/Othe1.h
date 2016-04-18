 // $Id: Othe1.h,v 1. 29-07-2015 Laure Massacrier Exp $

#ifndef LBCRMC_OTHE1_H
#define LBCRMC_OTHE1_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall OTHE1_ADDRESS(void) ;
}
#else
extern "C" {
  void* othe1_address__(void) ;
}
#endif

/** @class Othe1 Othe1.h LbCRMC/Othe1.h
 *
 *  Class to access cevt epos Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Othe1 {
public:
  Othe1();
  ~Othe1();
  int& istore();
  int& istmax();
  float& gaumx(); // ?
  int& irescl(); // ?
  int& ntrymx(); // ?
  int& nclean(); // ?
  int& iopdg(); // ?
  int& ioidch(); // ?
 
  inline void init(); // inlined for speed of access (small function)

private:
  struct OTHE1;
  friend struct OTHE1;
  
  struct OTHE1 {
    
  int istore;
  int istmax;
  float gaumx; // ?
  int irescl; // ?
  int ntrymx; // ?
  int nclean; // ?
  int iopdg; // ?
  int ioidch; // ?
  
  };
  
  static OTHE1* s_othe1;
};

// Inline implementations for cevt
// initialise pointer
#ifdef WIN32
void Othe1::init(void) {
  if ( 0 == s_othe1 ) s_othe1 = static_cast<OTHE1*>(OTHE1_ADDRESS());
}
#else
void Othe1::init(void) {
  if ( 0 == s_othe1 ) s_othe1 = static_cast<OTHE1*>(othe1_address__());
}
#endif
#endif // LBCRMC_CEVT_H
 
