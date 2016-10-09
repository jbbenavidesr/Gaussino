//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtLbAmpGen.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EvtCGEN_HH
#define EvtCGEN_HH

#include "EvtGenBase/EvtDecayProb.hh"
#include "EvtGenBase/EvtGenKine.hh"

#include <dlfcn.h>

class EvtParticle;


/// dynamic_fcn is a trivial wrapper over the dlsym function pointer
/// taking a return type and input types, then perfect forwarding 
/// to the underlying dynamic library using operators. 

template < typename RETURN_TYPE, typename... IN_TYPES > 
class dynamic_fcn {

  private : 
    RETURN_TYPE (*m_fcn)( IN_TYPES... );
  public: 
    dynamic_fcn(){}

    bool set( void* handle, const std::string& name ){
      m_fcn = (RETURN_TYPE(*)(IN_TYPES...))dlsym( handle, name.c_str() );
      if( m_fcn == nullptr ){
        std::cout << "ERROR : " << dlerror() << std::endl;
        return false;
      }
      return true; 
    }
    RETURN_TYPE operator()( IN_TYPES&&... input ) {
      return (*m_fcn)( std::forward<IN_TYPES>(input)... ) ; 
    }
};


class EvtLbAmpGen : public EvtDecayProb
{
  public:
    EvtLbAmpGen() {}

    virtual ~EvtLbAmpGen();

    std::string getName();
    EvtDecayBase* clone();

    void init();
    void initProbMax();

    void decay(EvtParticle *p); 

  private:
    void*                       m_handle; 
    dynamic_fcn<double, const double*, const unsigned int&> m_fcn;
    std::vector<double>         m_evtBuffer;
};

#endif
