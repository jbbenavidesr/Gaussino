 // $Id: Accum.h,v 1. 29-07-2015 Laure Massacrier Exp $

#ifndef LBCRMC_ACCUM_H
#define LBCRMC_ACCUM_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall ACCUM_ADDRESS(void) ;
}
#else
extern "C" {
  void* accum_address__(void) ;
}
#endif

static const int s_lenmxjerr = 10; //maximum number of ?

/** @class Accum Accum.h LbCRMC/Accum.h
 *
 *  Class to access Accum epos Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Accum {
public:
  Accum();
  ~Accum();
  int& imsg(); //?
  int& jerr(int n); //?
  int& ntevt(); //?
  int& nrevt(); // event number
  int& naevt(); //?
  int& nrstr(); //?
  int& nrptl(); //?


  inline void init(); // inlined for speed of access (small function)
  int lenmxjerr() const {return s_lenmxjerr;}

private:
  struct ACCUM;
  friend struct ACCUM;
  
  struct ACCUM {
    
  int imsg; //?
  int jerr[s_lenmxjerr]; //?
  int ntevt; //?
  int nrevt; // event number
  int naevt; //?
  int nrstr; //?
  int nrptl; //?
  
  };

  int m_dummy;
  static ACCUM* s_accum;
};

// Inline implementations for accum
// initialise pointer
#ifdef WIN32
void Accum::init(void) {
  if ( 0 == s_accum ) s_accum = static_cast<ACCUM*>(ACCUM_ADDRESS());
}
#else
void Accum::init(void) {
  if ( 0 == s_accum ) s_accum = static_cast<ACCUM*>(accum_address__());
}
#endif
#endif // LBCRMC_ACCUM_H
 
