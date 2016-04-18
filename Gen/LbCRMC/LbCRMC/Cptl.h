#ifndef LBCRMC_CPTL_H
#define LBCRMC_CPTL_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall CPTL_ADDRESS(void) ;
}
#else
extern "C" {
  void* cptl_address__(void) ;
}
#endif

// Length of the particle variables arrays
static const int s_lenmxptl = 200000; //maximum number of particles produced in EPOS

/** @class Cptl Cptl.h LbCRMC/Cptl.h
 *
 *  Class to access cptl epos Common block from C++ code.
 * 
 *  @author Laure Massacrier
 *  @date   2015-07-30
 */

class Cptl {
public:
  Cptl();
  ~Cptl();
  
  int& nptl(); //current particle index
  float& pptl(int n, int i); // i=1 x-component of particle momentum, i=2 y-component of particle momentum		                            // i=3 z-component of particle momentum, i=4 particle energy, i=5 particle mass
  int& iorptl( int n ); // particle number of father
  int& idptl( int n ) ; // particle id
  int& istptl( int n ); // particle status: 40-41: Remnant, 30-31 Pomeron, 20-21 Parton, 10-11 Droplet, 0-01 particles
			// last digit=0 : last generation
			// last digit=1 : not last generation
  float& tivptl(int n, int i); // i=1 formation time, i=2 destruction time (always in the pp-cms)
  int& ifrptl(int n, int i); // i=1 particle number of first child, i=2 particle number of last child
  int& jorptl( int n ); // particle number of mother
  float& xorptl(int n, int i); // i=1 x-component of formation point, i=2 y-component of formation point, 
				// i=3 z-componen of formation point, i=4 formation time
  int& ibptl(int n, int i); // i=1 ?, i=2?, i=3?, i=4?
  int& ityptl(int n); // type of particles origin (10-19: target, 20-29: soft Pom, 30-39: hard Pom, 40-49 projectile,  		            // 50: string)
  

  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenmxptl() const {return s_lenmxptl;}

private:
  struct CPTL;
  friend struct CPTL;
  
  struct CPTL {
  int nptl; //current particle index
  float pptl[s_lenmxptl][5]; // i=1 x-component of particle momentum, i=2 y-component of particle momentum		                            // i=3 z-component of particle momentum, i=4 particle energy, i=5 particle mass
  int iorptl[s_lenmxptl]; // particle number of father
  int idptl[s_lenmxptl] ; // particle id
  int istptl[s_lenmxptl]; // particle status: 40-41: Remnant, 30-31 Pomeron, 20-21 Parton, 10-11 Droplet, 0-01 particles
			// last digit=0 : last generation
			// last digit=1 : not last generation
  float tivptl[s_lenmxptl][2]; // i=1 formation time, i=2 destruction time (always in the pp-cms)
  int ifrptl[s_lenmxptl][2]; // i=1 particle number of first child, i=2 particle number of last child
  int jorptl[s_lenmxptl]; // particle number of mother
  float xorptl[s_lenmxptl][4]; // i=1 x-component of formation point, i=2 y-component of formation point, 
				// i=3 z-componen of formation point, i=4 formation time
  int ibptl[s_lenmxptl][4]; // i=1 ?, i=2?, i=3?, i=4?
  int ityptl[s_lenmxptl]; // type of particles origin (10-19: target, 20-29: soft Pom, 30-39: hard Pom, 40-49 projectile,  		            // 50: string)
 
  };
  int m_dummy;
  float m_realdummy;
  
  static CPTL* s_cptl;
};

// Inline implementations fo cptl
// initialise pointer
#ifdef WIN32
void Cptl::init(void) {
  if ( 0 == s_cptl ) s_cptl = static_cast<CPTL*>(CPTL_ADDRESS());
}
#else
void Cptl::init(void) {
  if ( 0 == s_cptl ) s_cptl = static_cast<CPTL*>(cptl_address__());
}
#endif
#endif // LBPYTHIA_CPTL_H
 
