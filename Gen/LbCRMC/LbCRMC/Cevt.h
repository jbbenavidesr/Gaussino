 // $Id: Cevt.h,v 1. 29-07-2015 Laure Massacrier Exp $

#ifndef LBCRMC_CEVT_H
#define LBCRMC_CEVT_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall CEVT_ADDRESS(void) ;
}
#else
extern "C" {
  void* cevt_address__(void) ;
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

class Cevt {
public:
  Cevt();
  ~Cevt();
  float& phievt(); // angle of impact parameter
  int& nevt(); //error code 1=valid event, 0 = invalid event
  float& bimevt(); //absolute value of impact parameter
  int& kolevt(); //number of collisions
  int& koievt(); //number of inelastic collisions
  float& pmxevt(); //reference momentum
  float& egyevt(); //pp cm energy (hadron) or string energy (lepton)
  int& npjevt(); //number of primary projectile participants 
  int& ntgevt(); //number of primary target participants
  int& npnevt(); //number of primary projectile neutron spectators
  int& nppevt(); //number of primary projectile proton spectators
  int& ntnevt(); //number of primary target neutron spectators
  int& ntpevt(); //number of primary target proton spectators
  int& jpnevt(); //number of absolute projectile neutron spectators
  int& jppevt(); //number of absolute projectile proton spectators
  int& jtnevt(); //number of absolute target neutron spectators
  int& jtpevt(); //number of absolute target proton spectators
  float& xbjevt(); //bjorken x for dis
  float& qsqevt(); //q**2 for dis
  //sigtot is not declared in epos.inc ?
  int& nglevt(); //number of collisions according to Glauber
  float& zppevt(); //average z-parton projectile
  float& zptevt(); //average z-parton target
  int& minfra(); //?
  int& maxfra(); //?
  int& kohevt(); //?    //be carefull it seems this quantity is not there in the cevt common block? will return always 0 ?


  inline void init(); // inlined for speed of access (small function)


private:
  struct CEVT;
  friend struct CEVT;
  
  struct CEVT {
    
  float phievt; // angle of impact parameter
  int nevt; //error code 1=valid event, 0 = invalid event
  float bimevt; //absolute value of impact parameter
  int kolevt; //number of collisions
  int koievt; //number of inelastic collisions
  float pmxevt; //reference momentum
  float egyevt; //pp cm energy (hadron) or string energy (lepton)
  int npjevt; //number of primary projectile participants 
  int ntgevt; //number of primary target participants
  int npnevt; //number of primary projectile neutron spectators
  int nppevt; //number of primary projectile proton spectators
  int ntnevt; //number of primary target neutron spectators
  int ntpevt; //number of primary target proton spectators
  int jpnevt; //number of absolute projectile neutron spectators
  int jppevt; //number of absolute projectile proton spectators
  int jtnevt; //number of absolute target neutron spectators
  int jtpevt; //number of absolute target proton spectators
  float xbjevt; //bjorken x for dis
  float qsqevt; //q**2 for di
  int nglevt; //number of collisions according to Glauber
  float zppevt; //average z-parton projectile
  float zptevt; //average z-parton target
  int minfra; //?
  int maxfra; //?
  int kohevt; //?   
  
  };
  
  static CEVT* s_cevt;
};

// Inline implementations for cevt
// initialise pointer
#ifdef WIN32
void Cevt::init(void) {
  if ( 0 == s_cevt ) s_cevt = static_cast<CEVT*>(CEVT_ADDRESS());
}
#else
void Cevt::init(void) {
  if ( 0 == s_cevt ) s_cevt = static_cast<CEVT*>(cevt_address__());
}
#endif
#endif // LBCRMC_CEVT_H
 
