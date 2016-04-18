 // $Id: Drop7.h,v 1. 29-07-2015 Laure Massacrier Exp $

#ifndef LBCRMC_DROP7_H
#define LBCRMC_DROP7_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall DROP7_ADDRESS(void) ;
}
#else
extern "C" {
  void* drop7_address__(void) ;
}
#endif

/** @class Drop7 drop7.h LbCRMC/Drop7.h
 *
 *  Class to access drop7 epos Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Drop7 {
public:
  Drop7();
  ~Drop7();
  float& ptclu(); // ?
  float& yradpi(); // ?
  float& yradpx(); // ?
  float& fploss(); // ? 
  float& fvisco(); // ?
  float& fplmin(); // ?
  int& ioclude(); // ?
  int& iocluin(); // ? 
  int& ioquen(); // ?
  int& iohole(); // ?
  int& kigrid(); // ?


  inline void init(); // inlined for speed of access (small function)


private:
  struct DROP7;
  friend struct DROP7;
  
  struct DROP7 {
  
  float ptclu; // ?
  float yradpi; // ?
  float yradpx; // ?
  float fploss; // ? 
  float fvisco; // ?
  float fplmin; // ?
  int ioclude; // ?
  int iocluin; // ? 
  int ioquen; // ?
  int iohole; // ?
  int kigrid; // ? 
  
  };
  
  static DROP7* s_drop7;
};

// Inline implementations for drop7
// initialise pointer
#ifdef WIN32
void Drop7::init(void) {
  if ( 0 == s_drop7 ) s_drop7 = static_cast<DROP7*>(DROP7_ADDRESS());
}
#else
void Drop7::init(void) {
  if ( 0 == s_drop7 ) s_drop7 = static_cast<DROP7*>(drop7_address__());
}
#endif
#endif // LBCRMC_DROP7_H
 
