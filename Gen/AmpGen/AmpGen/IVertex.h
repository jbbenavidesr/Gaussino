#ifndef IVERTEX_H
#define IVERTEX_H

#include "AmpGen/Particle.h"
#include "AmpGen/Tensor.h"
#include <map>

namespace AmpGen { 
struct IVertex {  
  static const LorentzIndex mu;
  static const LorentzIndex nu;
  static const LorentzIndex alpha;
  static const LorentzIndex beta;
  static const double GeV;
  virtual Tensor get( Tensor P,  Tensor Q, Tensor V1, Tensor V2,std::vector<DBSYMBOL>* db = 0)=0; 
  virtual ~IVertex(){};
};


#define DECLARE_VERTEX(NAME ) struct NAME : public IVertex { \
  virtual Tensor get( Tensor P, Tensor Q,  Tensor V1, Tensor V2, std::vector<DBSYMBOL>* db = 0); }

#define DEFINE_VERTEX(NAME) Tensor NAME::get( Tensor P, Tensor Q, Tensor V1, Tensor V2 , std::vector<DBSYMBOL>* db)

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

DECLARE_VERTEX( T_VS_D );
DECLARE_VERTEX( T_VS_P );
DECLARE_VERTEX( T_SS_D );
DECLARE_VERTEX( T_TS_D );
DECLARE_VERTEX( T_TS_S );


class VertexFactory {

  static VertexFactory *s_vertexInstance;
  public:
  std::map<unsigned int,IVertex*> m_vertices;

  VertexFactory() : m_vertices( {
      { pid(0,0,0,0) , new S_SS_S() }, 
      { pid(0,1,0,1) , new S_VS_P() },
      { pid(0,1,1,0) , new S_VV_S() },
      { pid(0,1,1,1) , new S_VV_P() },
      { pid(0,1,1,2) , new S_VV_D() },
      { pid(0,2,1,1) , new S_TV_P() },
      { pid(0,2,1,2) , new S_TV_D() },
      { pid(0,2,0,2) , new S_TS_D() },
      { pid(0,2,2,0) , new S_TT_S() },

      { pid(1,0,0,1) , new V_SS_P() },
      { pid(1,1,0,0) , new V_VS_S() },
      { pid(1,1,0,1) , new V_VS_P() },
      { pid(1,1,0,2) , new V_VS_D() },
      { pid(1,2,0,1) , new V_TS_P() },

      { pid(2,1,0,1) , new T_VS_P() },
      { pid(2,1,0,2) , new T_VS_D() },
      { pid(2,0,0,2) , new T_SS_D() },
      { pid(2,2,0,0) , new T_TS_S() } }) {} ;

  static VertexFactory* get(){
    if( !s_vertexInstance )
      s_vertexInstance = new VertexFactory();
    return s_vertexInstance;
  };
  static Tensor getSpinFactor( Tensor P, Tensor Q,  Tensor V1, Tensor V2, const unsigned int& hash, std::vector<DBSYMBOL>* db=0 ) ; //Particle& mother,std::vector<DBSYMBOL>* db = 0 ) ; 
  void addSpinfactor( const unsigned int& hash, IVertex* vertex){
    if( m_vertices.find(hash) != m_vertices.end() ){
      IVertex* oldVertex = m_vertices.find(hash)->second;
      delete oldVertex;
      WARNING("Overwriting Vertex - " << hash);
    }
    m_vertices[hash] = vertex;
  }
  const std::map<unsigned int, IVertex*>& vertices() const{ return get()->m_vertices; }
  static bool isVertex( const unsigned int& hash ) ;
};

} 
#endif
