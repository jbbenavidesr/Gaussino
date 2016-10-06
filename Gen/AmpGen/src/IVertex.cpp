#include "AmpGen/IVertex.h"
#include <bitset>

using namespace AmpGen;

const LorentzIndex IVertex::mu = LorentzIndex("mu");
const LorentzIndex IVertex::nu = LorentzIndex("nu");
const LorentzIndex IVertex::alpha = LorentzIndex("alpha");
const LorentzIndex IVertex::beta = LorentzIndex("beta");

const double IVertex::GeV = 1000.;

unsigned int AmpGen::pid( const double& motherSpin, const double& daughter0, const double& daughter1, const unsigned int& orbital , const unsigned int& spin) {
  unsigned int hash = ( int(2*motherSpin) << 16 ) + ( int(2*daughter0) << 12 ) + ( int(2*daughter1) << 8 ) + ( orbital << 4 ) + spin ;
  return hash;
}

VertexFactory *VertexFactory::s_vertexInstance = 0;

bool VertexFactory::isVertex( const unsigned int& hash){
  auto gSpinProjectors = VertexFactory::get()->vertices();
  return gSpinProjectors.find( hash ) != gSpinProjectors.end();
}

Tensor VertexFactory::getSpinFactor(Tensor P, Tensor Q, Tensor V1, Tensor V2, const unsigned int& hash, std::vector<DBSYMBOL>* db){
  auto gSpinProjectors = VertexFactory::get()->vertices();
  auto connector = gSpinProjectors.find( hash );
  if( connector == gSpinProjectors.end() ){
    ERROR( "( hash = " << hash<< ")   " << std::bitset<20>( hash ) );
    return Tensor( std::vector<double>({1.} ),{0} ) ;
  }
  else return connector->second->get( P,Q,V1,V2 , db );
}



DEFINE_VERTEX( S_SS_S ) { return V1 * V2[0] ; } 

DEFINE_VERTEX( S_VV_S ) { return V1(mu) | V2(-mu ); } 

DEFINE_VERTEX( S_VV_D ) {
  Tensor L2 = Orbital_DWave( P, Q ) / (GeV*GeV);
  return V1 ( mu ) |  L2( -mu, -nu ) | V2( nu );
} 

DEFINE_VERTEX( S_VV_P) {
  Tensor L = Orbital_PWave(P,Q);
  Tensor coupling = LeviCivita()(-mu,-nu,-alpha,-beta) | L(alpha) | P(beta) ;
  return  V1( mu ) | coupling( -mu , -nu ) | V2( nu ) / (GeV*GeV);
}

DEFINE_VERTEX( S_VS_P)  {  return V1( mu ) | Orbital_PWave(P,Q)( -mu ) * V2[0] / GeV; } 


DEFINE_VERTEX( V_SS_P ){ return Orbital_PWave( P, Q ) * V1[0] * V2[0] / GeV ; } 

DEFINE_VERTEX( V_VS_P ) {
  Tensor L  = Orbital_PWave(P,Q) / GeV ; /// orbital part ////
  Tensor coupling = LeviCivita()(-mu,-nu,-alpha,-beta) | L( alpha ) | P( beta ) / GeV;   
  return ( coupling( -mu , -nu ) | V1(nu) ) * V2[0];
}

DEFINE_VERTEX( V_VS_S ) { return Orbital_PWave( P , V1 ) * V2[0]; }

DEFINE_VERTEX( V_VS_D ){
  Tensor L_2_V0 = Orbital_DWave( P, Q ) / ( GeV * GeV ) ; 
  Tensor Sv = Spin1ProjectionOperator( P );
  return ( Sv( mu , nu ) | L_2_V0( -nu,-alpha) | V1(alpha) ) * V2[0]; 
} 

DEFINE_VERTEX( T_VS_D ) {

  Tensor G = LeviCivita()( -mu,-nu,-alpha,-beta) | P(nu) | Q(alpha) | V1(beta) ;
  Tensor L     = Orbital_PWave( P, Q );
  Tensor thing =  ( (G(mu) | L(nu)) + (L(mu)|G(nu))  ) * V2[0] / Constant(-2*GeV*GeV*GeV);  
  return thing;
}


DEFINE_VERTEX( T_TS_S ) {

  Tensor S = Spin1ProjectionOperator( P );
  Tensor term1 =  S(-mu,-alpha) | V1( alpha , beta ) | S( -beta, -nu ) ;
  Tensor term2 =  S*( dot(V1,S) ) / 3.;
  return ( term1 - term2 ) * V2[0];
} 


DEFINE_VERTEX( T_VS_P ) { 
  Tensor L = Orbital_PWave( P, Q ) / GeV ;
  Tensor S = Spin1ProjectionOperator(P);
  Tensor Vp= S(-mu,-nu) | V1 (nu) ;
  return V2[0] * ( (  ( L(alpha) | Vp(beta) )  + ( Vp(alpha) | L(beta) )  )/2. - S*dot(L, V1 )/3.) ;
} 

DEFINE_VERTEX( T_SS_D ) { return Orbital_DWave( P, Q ) / (GeV*GeV) ; } 

DEFINE_VERTEX( S_TV_P ) { 
  Tensor L = Orbital_PWave( P, Q );
  return ( V1 (mu,nu ) | L(-mu) ) | V2(-nu) ; 
} 

DEFINE_VERTEX( S_TS_D ) { 
  Tensor orbital = Orbital_DWave( P, Q );
  return V2[0] * Tensor( { dot (orbital, V1 ) / (GeV*GeV) } , {1} ) ; 
} 


DEFINE_VERTEX( S_TV_D ) {
  Tensor term1 = V1 (alpha, beta ) | Orbital_DWave( P, Q ) ( -beta, -nu ) ; 
  Tensor term2 = LeviCivita()(-mu,-nu,-alpha,-beta) | P( alpha ) | V2( beta ) ; // Antisymmetric( P, V2 );
  return Tensor( {dot( term1, term2 )} );
} 

DEFINE_VERTEX( S_TT_S ){ return Tensor( {dot(V1,V2)} ) ; }

DEFINE_VERTEX( V_TS_P ){ return ( V1(mu) | Orbital_PWave(P, Q )(-mu) ) * V2[0] ; } 

