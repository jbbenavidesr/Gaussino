// $Id:Hadr5.cpp,v 1., 30-10-2015, Laure Massacrier $
// access epos common block Hadr5.h
#include "LbCRMC/Hadr5.h"

// set pointer to zero at start
Hadr5::HADR5* Hadr5::s_hadr5 =0;

// Constructor
Hadr5::Hadr5(){ }

// Destructor
Hadr5::~Hadr5() { }

//access sigtot in COMMON
float& Hadr5::sigtot(){
 init();
 return s_hadr5 -> sigtot;
}

//access sigcut in COMMON
float& Hadr5::sigcut() {
  init();
  return s_hadr5 -> sigcut;
}

//access sigela in COMMON
float& Hadr5::sigela(){
 init();
 return s_hadr5 -> sigela;
}

//access sloela in COMMON
float& Hadr5::sloela(){
 init();
 return s_hadr5 -> sloela;
}  

//access sigsd in COMMON
float& Hadr5::sigsd(){
 init(); 
 return s_hadr5 -> sigsd;
}  
  
//access sigine in COMMON
float& Hadr5::sigine(){
 init();
 return s_hadr5 -> sigine; 
}  

//access sigdif in COMMON
float& Hadr5::sigdif(){
 init();
 return s_hadr5 -> sigdif; 
}  

//access sigineaa in COMMON
float& Hadr5::sigineaa(){
 init();
 return s_hadr5 -> sigineaa; 
}  

//access sigtotaa in COMMON
float& Hadr5::sigtotaa(){
 init();
 return s_hadr5 -> sigtotaa; 
}  

//access sigelaaa in COMMON
float& Hadr5::sigelaaa(){
 init();
 return s_hadr5 -> sigelaaa;
}  

//access sigcutaa in COMMON
float& Hadr5::sigcutaa(){
 init();
 return s_hadr5 -> sigcutaa; 
}  

//access sigdd in COMMON
float& Hadr5::sigdd(){
 init();
 return s_hadr5 -> sigdd; 
}  






  
