#ifndef IVERTEX_H
#define IVERTEX_H

#include "AmpGen/Particle.h"
#include "AmpGen/Tensor.h"
#include "AmpGen/Factory.h"

#include <map>

namespace AmpGen { 
  struct IVertex {  
    static const LorentzIndex mu;
    static const LorentzIndex nu;
    static const LorentzIndex alpha;
    static const LorentzIndex beta;
    static const double GeV;
    virtual Tensor get( const Tensor& P,  const Tensor& Q, const Tensor& V1, const Tensor& V2,std::vector<DBSYMBOL>* db = 0)=0; 
    virtual ~IVertex(){};
    IVertex* create(){ return this ; } /// only a single Vertex defined /// 
  };


#define DECLARE_VERTEX(NAME) \
  struct NAME : public IVertex { \
    virtual Tensor get( const Tensor& P, const Tensor& Q,  const Tensor& V1, const Tensor& V2, std::vector<DBSYMBOL>* db = 0); \
    static unsigned int _id;  } \

#define DEFINE_VERTEX( VERTEX, PID ) \
  REGISTER_WITH_KEY( IVertex, VERTEX, PID , unsigned int ) ; \
  Tensor VERTEX::get( const Tensor& P, const Tensor& Q, const Tensor& V1, const Tensor& V2 , std::vector<DBSYMBOL>* db) \

  extern unsigned int pid( const double& motherSpin, 
      const double& daughter0, 
      const double& daughter1, 
      const unsigned int& orbital, 
      const unsigned int& spin=0);


  DECLARE_VERTEX( S_SS_S );

  DECLARE_VERTEX( S_VV_S );
  DECLARE_VERTEX( S_VV_D ); 
  DECLARE_VERTEX( S_VV_P );
  DECLARE_VERTEX( S_VS_P );
  DECLARE_VERTEX( S_TV_P );

  DECLARE_VERTEX( S_TV_D );

  DECLARE_VERTEX( S_TS_D );
  DECLARE_VERTEX( S_TT_S );

  DECLARE_VERTEX( V_SS_P );
  DECLARE_VERTEX( V_VS_P );
  DECLARE_VERTEX( V_VS_S );
  DECLARE_VERTEX( V_VS_D );
  DECLARE_VERTEX( V_TS_P );
  DECLARE_VERTEX( V_TS_D );

  DECLARE_VERTEX( T_VS_D );
  DECLARE_VERTEX( T_VS_P );
  DECLARE_VERTEX( T_SS_D );
  DECLARE_VERTEX( T_TS_D );
  DECLARE_VERTEX( T_TS_S );

  class VertexFactory : public Factory<IVertex, unsigned int> {
    public: 
      static Tensor getSpinFactor( const Tensor& P, const Tensor& Q,  const Tensor& V1, const Tensor& V2, const unsigned int& hash, std::vector<DBSYMBOL>* db=0 ) ; //Particle& mother,std::vector<DBSYMBOL>* db = 0 ) ; 
      static bool isVertex( const unsigned int& hash ) ;
  };
}
#endif
