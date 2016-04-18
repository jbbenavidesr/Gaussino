 // $Id: Hadr25.h,v 1. 29-07-2015 Laure Massacrier Exp $

#ifndef LBCRMC_HADR25_H
#define LBCRMC_HADR25_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall HADR25_ADDRESS(void) ;
}
#else
extern "C" {
  void* hadr25_address__(void) ;
}
#endif

/** @class Cevt Cevt.h LbCRMC/Cevt.h
 *
 *  Class to access cevt epos Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Hadr25 {
public:
  Hadr25();
  ~Hadr25();
  int& idprojin(); //?
  int& idtargin(); //?
  float& rexdifi(int n); //?
  float& rexndii(int n); //?
  int& irdmpr(); //?
  int& isoproj(); //?
  int& isotarg(); //?
 
  inline void init(); // inlined for speed of access (small function)


private:
  struct HADR25;
  friend struct HADR25;
  
  struct HADR25 {
    
  int idprojin; //?
  int idtargin; //?
  float rexdifi[4]; //?
  float rexndii[4]; //?
  int irdmpr; //?
  int isoproj; //?
  int isotarg; //?
  
  };
  
  float m_realdummy;

  static HADR25* s_hadr25;
};

// Inline implementations for cevt
// initialise pointer
#ifdef WIN32
void Hadr25::init(void) {
  if ( 0 == s_hadr25 ) s_hadr25 = static_cast<HADR25*>(HADR25_ADDRESS());
}
#else
void Hadr25::init(void) {
  if ( 0 == s_hadr25 ) s_hadr25 = static_cast<HADR25*>(hadr25_address__());
}
#endif
#endif // LBCRMC_HADR25_H
 
