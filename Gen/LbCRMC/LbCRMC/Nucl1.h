 // $Id: Cevt.h,v 1. 29-07-2015 Laure Massacrier Exp $

#ifndef LBCRMC_NUCL1_H
#define LBCRMC_NUCL1_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall NUCL1_ADDRESS(void) ;
}
#else
extern "C" {
  void* nucl1_address__(void) ;
}
#endif

/** @class Nucl1 Nucl1.h LbCRMC/Nucl1.h
 *
 *  Class to access Nucl1 epos Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Nucl1 {
public:
  Nucl1();
  ~Nucl1();
  int& laproj(); //? of projectile
  int& maproj(); //? of projectile
  int& latarg(); //? of target
  int& matarg(); //? of target 
  float& core(); //?
  float& fctrmx(); //?
 

  inline void init(); // inlined for speed of access (small function)


private:
  struct NUCL1;
  friend struct NUCL1;
  
  struct NUCL1 {

    int laproj; //? of projectile
    int maproj; //? of projectile
    int latarg; //? of target
    int matarg; //? of target 
    float core; //?
    float fctrmx; //?
  
 
  };
  
  static NUCL1* s_nucl1;
};

// Inline implementations for nucl1
// initialise pointer
#ifdef WIN32
void Nucl1::init(void) {
  if ( 0 == s_nucl1 ) s_nucl1 = static_cast<NUCL1*>(NUCL1_ADDRESS());
}
#else
void Nucl1::init(void) {
  if ( 0 == s_nucl1 ) s_nucl1 = static_cast<NUCL1*>(nucl1_address__());
}
#endif
#endif // LBCRMC_NUCL1_H
 
