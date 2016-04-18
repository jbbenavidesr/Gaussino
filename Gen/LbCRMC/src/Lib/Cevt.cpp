// $Id:Cevt.cpp,v 1., 29-07-2015, Laure Massacrier $
// access epos common block cevt.h
#include "LbCRMC/Cevt.h"

// set pointer to zero at start
Cevt::CEVT* Cevt::s_cevt =0;

// Constructor
Cevt::Cevt(){ }

// Destructor
Cevt::~Cevt() { }

//access phievt in COMMON
float& Cevt::phievt(){
 init();
 return s_cevt -> phievt;
}

//access nevt in COMMON
int& Cevt::nevt() {
  init() ;
  return s_cevt -> nevt ;
}

//access bimevt in COMMON
float& Cevt::bimevt(){
 init();
 return s_cevt -> bimevt;
}

//access kolevt in COMMON
int& Cevt::kolevt(){
 init();
 return s_cevt -> kolevt;
}  

//access koievt in COMMON
int& Cevt::koievt(){
 init();
 return s_cevt -> koievt;
}  

//access pmxevt in COMMON
float& Cevt::pmxevt(){
 init();
 return s_cevt -> pmxevt;
} 
  
//access egyevt in COMMON
float& Cevt::egyevt(){
 init();
 return s_cevt -> egyevt;
} 

//access npjevt in COMMON
int& Cevt::npjevt(){
 init();
 return s_cevt -> npjevt;
}  

//access ntgevt in COMMON
int& Cevt::ntgevt(){
 init();
 return s_cevt -> ntgevt; 
}  

//access npnevt in COMMON
int& Cevt::npnevt(){
 init();
 return s_cevt -> npnevt;
}

//access nppevt in COMMON
int& Cevt::nppevt(){
 init();
 return s_cevt -> nppevt;
}  

//access ntnevt in COMMON
int& Cevt::ntnevt(){
 init();
 return s_cevt -> ntnevt;
}

//access ntpevt in COMMON
int& Cevt::ntpevt(){
 init(); 
 return s_cevt -> ntpevt; 
}  

//access jpnevt in COMMON
int& Cevt::jpnevt(){
  init();
  return s_cevt -> jpnevt;
} 

//access jppevt in COMMON 
int& Cevt::jppevt(){
  init();
  return s_cevt -> jppevt;
}

//access jtnevt in COMMON
int& Cevt::jtnevt(){
 init();
 return s_cevt -> jtnevt;
} 

//access jtpevt in COMMON
int& Cevt::jtpevt(){
 init();
 return s_cevt -> jtpevt;
}

//access xbjevt in COMMON
float& Cevt::xbjevt(){
 init();
 return s_cevt -> xbjevt;
} 

//access qsqevt in COMMON
float& Cevt::qsqevt(){
 init();
 return s_cevt -> qsqevt;
}  

//access nglevt in COMMON
int& Cevt::nglevt(){
 init();
 return s_cevt -> nglevt;
} 

//access zppevt in COMMON
float& Cevt::zppevt(){
 init();
 return s_cevt -> zppevt;
}  

//access zptevt in COMMON
float& Cevt::zptevt(){
 init();
 return s_cevt -> zptevt;
}  
  
//access minfra in COMMON
int& Cevt::minfra(){
 init(); 
 return s_cevt -> minfra;
} 

//access maxfra in COMMON
int& Cevt::maxfra(){
 init();
 return s_cevt -> maxfra;
} 
  
//access kohevt in COMMON
int& Cevt::kohevt(){ 
 init();
 return s_cevt -> kohevt;
}    
  
