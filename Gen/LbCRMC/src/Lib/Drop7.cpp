// $Id:Drop7.cpp,v 1., 29-07-2015, Laure Massacrier $
// access drop7 common block drop7.h
#include "LbCRMC/Drop7.h"

// set pointer to zero at start
Drop7::DROP7* Drop7::s_drop7 =0;

// Constructor
Drop7::Drop7(){ }

// Destructor
Drop7::~Drop7() { }

//access ptclu in COMMON
float& Drop7::ptclu(){
 init();
 return s_drop7 -> ptclu;
}

//access yradpi in COMMON
float& Drop7::yradpi() {
  init() ;
  return s_drop7 -> yradpi ;
}

//access yradpx in COMMON
float& Drop7::yradpx(){
 init();
 return s_drop7 -> yradpx;
}

//access fploss in COMMON
float& Drop7::fploss(){
 init();
 return s_drop7 -> fploss;
}  

//access fvisco in COMMON
float& Drop7::fvisco(){
 init();
 return s_drop7 -> fvisco;
}  

//access fplmin in COMMON
float& Drop7::fplmin(){
 init();
 return s_drop7 -> fplmin;
} 
  
//access ioclude in COMMON
int& Drop7::ioclude(){
 init();
 return s_drop7 -> ioclude;
} 

//access iocluin in COMMON
int& Drop7::iocluin(){
 init();
 return s_drop7 -> iocluin;
}  

//access ioquen in COMMON
int& Drop7::ioquen(){
 init();
 return s_drop7 -> ioquen; 
}  

//access iohole in COMMON
int& Drop7::iohole(){
 init();
 return s_drop7 -> iohole;
}

//access kigrid in COMMON
int& Drop7::kigrid(){
 init();
 return s_drop7 -> kigrid;
}  

