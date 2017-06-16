//--------------------------------------------------------------------------
//
//  X -> N Particles with the AMPGEN-MINT generator
//	Tim Evans 
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenModels/EvtLbAmpGen.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtRadCorr.hh"
// Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/GaudiException.h"

// C++
#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dlfcn.h>
#include <iomanip> 
#include <complex.h>
#include <stdexcept>

EvtLbAmpGen::~EvtLbAmpGen() {

}


std::string EvtLbAmpGen::getName(){
  return "LbAmpGen";
}


EvtDecayBase* EvtLbAmpGen::clone(){
  return new EvtLbAmpGen;
}

void splitByDelim(const std::string &s, char delim, std::vector<std::string>& elems) {
  std::string item;
  std::stringstream ss(s);
  while (std::getline(ss, item, delim)) {
    if( item != " " && item != "" && item != "\n" && item != "\t")
      elems.push_back(item);
  }
}


void EvtLbAmpGen::init(){

  /// The argument is the decfile is the name of the shared library to load
  
  if ( getNArg() != 1 ){
    GaudiException( "Not enough arguments in DECFILE", "EvtGen",StatusCode::FAILURE );
  }
  std::vector<std::string> directories; 

  directories.push_back( System::getEnv( "PWD" ) );
  splitByDelim( System::getEnv("LD_LIBRARY_PATH"), ':', directories );
  /*
   loop over the current path and all of LD_LIBRARY_PATH
   looking for the requested shared library. If the shared library is 
   found, we take that one.  
  */
  m_handle = nullptr;
  for( auto& is : directories ){ 
    std::string libName = is + "/libLb" + getArgStr(0) + ".so";
    report(INFO,"EvtGen") << "INFO: checking for shared library: " << libName << std::endl; 
    struct stat buffer;
    if( stat( libName.c_str(), &buffer ) != 0 ) continue; 
    m_handle = dlopen( libName.c_str() ,RTLD_NOW); 
    report( INFO, "EvtGen" ) << "Loaded library " << libName << std::endl; 
    if( m_handle == nullptr ){
      GaudiException( "Library could not be linked", "EvtGen", StatusCode::FAILURE );
    }
    break; 
  } 
  if( m_handle == nullptr ){
    GaudiException( "Library not found " + getArgStr(0), "EvtGen",StatusCode::FAILURE );
  }
  /// The total amplitude is always referred to as FCN
  if( ! m_fcn.set( m_handle, "FCN") ){
    GaudiException("Library does not contain FCN","EvtGen",StatusCode::FAILURE );
  }
  /// set the buffer size to 4* 4 vectors + 1 proper time. 
  m_evtBuffer.resize(4 * getNDaug() + 1 );

}

void EvtLbAmpGen::initProbMax(){
  /// the probability can be normalised when generating the 
  /// source code, hence the max probability is always 1. 
  setProbMax(1.);
}

void EvtLbAmpGen::decay( EvtParticle *p){
  
  p->initializePhaseSpace(getNDaug(),getDaugs());
  EvtVector4R total;
  
  /// we use the funny root-ordering for four momenta - should
  /// probably change this at some point, but quite non-trivial. 
  for( int i = 0 ; i < getNDaug() ; ++i){
    EvtVector4R p4 = p->getDaug(i)->getP4();
    m_evtBuffer[ 4*i + 0 ] = 1000 * p4.get(1);
    m_evtBuffer[ 4*i + 1 ] = 1000 * p4.get(2);
    m_evtBuffer[ 4*i + 2 ] = 1000 * p4.get(3);
    m_evtBuffer[ 4*i + 3 ] = 1000 * p4.get(0);
    total += p4;
  };
  /* 
   Pput the lifetime in the event buffer for future proofing. 
   won't work naively as the accept-reject uses the same lifetime 
   multiple times. 
  */
  m_evtBuffer[ 4*getNDaug() ] = p->getLifetime(); 
 
  /*
   Get probability of event - PDFs assume particles, not antiparticles
   therefore, an additional minus sign is required 
   if generating anti particles in the P-odd amplitudes
   With assumption of no CPV in charm sector
   For systems with larger CPV, can include two different amplitudes.
  */
  double prob = m_fcn( &(m_evtBuffer[0]) ,p->getPDGId() > 0 ? 1 : -1 ) ;
  if( prob > 1 ){
    report(ERROR,"EvtGen") << " prob > prob(max) !" << std::endl; 
  }
  setProb( prob );
}

