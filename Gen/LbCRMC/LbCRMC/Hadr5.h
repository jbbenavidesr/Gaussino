// $Id: Hadr5.h,v 1. 30-10-2015 Laure Massacrier Exp $

#ifndef LBCRMC_HADR5_H
#define LBCRMC_HADR5_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall HADR5_ADDRESS(void) ;
}
#else
extern "C" {
  void* hadr5_address__(void) ;
}
#endif

/** @class Hadr5 Hadr5.h LbCRMC/Hadr5.h
 *
 *  Class to access hadr5 epos Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Hadr5 {
public:
  Hadr5();
  ~Hadr5();
  float& sigtot(); // ........ h-p total cross section in mb
  float& sigcut(); // ........ h-p cut cross section in mb : in principle it is the non-diffractive xs but the definition depends on the model
  float& sigela(); // ........ h-p elastic cross section in mb
  float& sloela(); // ........ h-p elastic slope
  float& sigsd(); // ........ h-p single diffractive cross section in mb (both side)
  float& sigine(); // ........ h-p inelastic cross section in mb (all inelastic processes=sigtot-sigela)
  float& sigdif();  // ........ h-p diffractive cross section in mb (SD+DD+DPE) (in principle sigdif+sigcut=sigine but it depends how DPE xs is counted (in EPOS 1.99 it is counted as elastic because nothing was produced but in EPOS LHC DPE are produced)
  float& sigineaa(); // ........ h-A or A-A cross section in mb (inelastic cross section to be used as CURRENT EVENT XS for defined projectile and target, previous h-p xs are really for h-p even if the projectile/target were defined as a nuclei)
  float& sigtotaa(); // ........ h-A or A-A total cross section in mb
  float& sigelaaa(); // ........ h-A or A-A elastic cross section in mb
  float& sigcutaa(); // ........ h-A or A-A ND xs or production xs mb
  float& sigdd();  // ........ h-p double diffractive cross section in mb (both side)


  inline void init(); // inlined for speed of access (small function)


private:
  struct HADR5;
  friend struct HADR5;
  
  struct HADR5 {
    
  float sigtot; // ........ h-p total cross section in mb
  float sigcut; // ........ h-p cut cross section in mb : in principle it is the non-diffractive xs but the definition depends on the model
  float sigela; // ........ h-p elastic cross section in mb
  float sloela; // ........ h-p elastic slope
  float sigsd; // ........ h-p single diffractive cross section in mb (both side)
  float sigine; // ........ h-p inelastic cross section in mb (all inelastic processes=sigtot-sigela)
  float sigdif;  // ........ h-p diffractive cross section in mb (SD+DD+DPE) (in principle sigdif+sigcut=sigine but it depends how DPE xs is counted (in EPOS 1.99 it is counted as elastic because nothing was produced but in EPOS LHC DPE are produced)
  float sigineaa; // ........ h-A or A-A cross section in mb (inelastic cross section to be used as CURRENT EVENT XS for defined projectile and target, previous h-p xs are really for h-p even if the projectile/target were defined as a nuclei)
  float sigtotaa; // ........ h-A or A-A total cross section in mb
  float sigelaaa; // ........ h-A or A-A elastic cross section in mb
  float sigcutaa; // ........ h-A or A-A ND xs or production xs mb
  float sigdd;  // ........ h-p double diffractive cross section in mb (both side) 
 
  
  };
  
  static HADR5* s_hadr5;
};

// Inline implementations for cevt
// initialise pointer
#ifdef WIN32
void Hadr5::init(void) {
  if ( 0 == s_hadr5 ) s_hadr5 = static_cast<HADR5*>(HADR5_ADDRESS());
}
#else
void Hadr5::init(void) {
  if ( 0 == s_hadr5 ) s_hadr5 = static_cast<HADR5*>(hadr5_address__());
}
#endif
#endif // LBCRMC_HADR5_H
 
