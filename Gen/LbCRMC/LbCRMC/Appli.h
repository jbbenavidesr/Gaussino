 // $Id: Cevt.h,v 1. 29-07-2015 Laure Massacrier Exp $

#ifndef LBCRMC_APPLI_H
#define LBCRMC_APPLI_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall APPLI_ADDRESS(void) ;
}
#else
extern "C" {
  void* appli_address__(void) ;
}
#endif

/** @class Appli Appli.h LbCRMC/Appli.h
 *
 *  Class to access appli epos Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Appli {
public:
  Appli();
  ~Appli();
  int& iappl(); //?
  int& model(); //?


  inline void init(); // inlined for speed of access (small function)


private:
  struct APPLI;
  friend struct APPLI;
  
  struct APPLI {
    
  int iappl; //?
  int model; //?

  };
  
  static APPLI* s_appli;
};

// Inline implementations for cevt
// initialise pointer
#ifdef WIN32
void Appli::init(void) {
  if ( 0 == s_appli ) s_appli = static_cast<APPLI*>(APPLI_ADDRESS());
}
#else
void Appli::init(void) {
  if ( 0 == s_appli ) s_appli = static_cast<APPLI*>(appli_address__());
}
#endif
#endif // LBCRMC_APPLI_H
 
