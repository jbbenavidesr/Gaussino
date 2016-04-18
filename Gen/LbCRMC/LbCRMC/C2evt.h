// $Id: C2evt.h,v 1. 29-07-2015 Laure Massacrier Exp $

#ifndef LBCRMC_C2EVT_H
#define LBCRMC_C2EVT_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall C2EVT_ADDRESS(void) ;
}
#else
extern "C" {
  void* c2evt_address__(void) ;
}
#endif

/** @class C2evt C2evt.h LbCRMC/C2evt.h
 *
 *  Class to access c2evt epos Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class C2evt {
public:
  C2evt();
  ~C2evt();
  
  int& ng1evt(); // number of Glauber participants with at least one IAs
  int& ng2evt(); // number of Glauber participants with at least two IAs
  float& rglevt(); // ?
  float& sglevt(); // ?
  float& eglevt(); // ?
  float& fglevt(); // ?
  int& ikoevt(); // number of elementary parton-parton scatterings
  float& typevt(); // type of event (1= Non Diff, 2= Double Diff, 3= Single Diff) 


  inline void init(); // inlined for speed of access (small function)


private:
  struct C2EVT;
  friend struct C2EVT;
  
  struct C2EVT {
    
  int ng1evt; // number of Glauber participants with at least one IAs
  int ng2evt; // number of Glauber participants with at least two IAs
  float rglevt; // ?
  float sglevt; // ?
  float eglevt; // ?
  float fglevt; // ?
  int ikoevt; // number of elementary parton-parton scatterings
  float typevt; // type of event (1= Non Diff, 2= Double Diff, 3= Single Diff)
  
  };
  
  static C2EVT* s_c2evt;
};

// Inline implementations for cevt
// initialise pointer
#ifdef WIN32
void C2evt::init(void) {
  if ( 0 == s_c2evt ) s_c2evt = static_cast<C2EVT*>(C2EVT_ADDRESS());
}
#else
void C2evt::init(void) {
  if ( 0 == s_c2evt ) s_c2evt = static_cast<C2EVT*>(c2evt_address__());
}
#endif
#endif // LBCRMC_C2EVT_H
 
