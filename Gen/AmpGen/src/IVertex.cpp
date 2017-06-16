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

template <> Factory<AmpGen::IVertex, unsigned int>* Factory<AmpGen::IVertex, unsigned int>::gImpl = 0;

bool VertexFactory::isVertex( const unsigned int& hash){
  return get(hash) != 0 ; 
}

Tensor VertexFactory::getSpinFactor( const Tensor& P, const Tensor& Q, const Tensor& V1, const Tensor& V2, const unsigned int& hash, std::vector<DBSYMBOL>* db){
  auto connector = VertexFactory::get( hash );
  if( connector == 0 ){
    ERROR( "( hash = " << hash<< ")   " << std::bitset<20>( hash ) );
    return Tensor( std::vector<double>({1.} ),{0} ) ;
  }
  else return connector->get( P,Q,V1,V2 , db );
}

DEFINE_VERTEX( S_SS_S , pid(0,0,0,0) ) { return V1 * V2[0] ; } 


DEFINE_VERTEX( S_VV_S , pid(0,1,1,0) ) { return V1(mu) * V2(-mu ); } 

DEFINE_VERTEX( S_VV_D , pid(0,1,1,2) ) {
  Tensor L2 = Orbital_DWave( P, Q ) / (GeV*GeV);
  return V1 ( mu ) *  L2( -mu, -nu ) * V2( nu );
} 

DEFINE_VERTEX( S_VV_P, pid(0,1,1,1) ) {
  Tensor L = Orbital_PWave(P,Q);
  Tensor coupling = LeviCivita()(-mu,-nu,-alpha,-beta) * L(alpha) * P(beta) ;
  return  V1( mu ) * coupling( -mu , -nu ) * V2( nu ) / (GeV*GeV);
}

DEFINE_VERTEX( S_VS_P, pid(0,1,0,1) )  {  
  DEBUG("P-wave part = " );
  Tensor p_wave = Orbital_PWave(P,Q) ;
  Tensor p_v1 = V1(mu) * p_wave(-mu);

  return p_v1 * V2[0] / GeV; } 


  DEFINE_VERTEX( V_SS_P , pid(1,0,0,1) ){ 
    DEBUG("Getting p-wave object");
    Tensor p_wave = Orbital_PWave( P, Q ) ; 
    DEBUG( "Got p-wave object");
    Expression scalar_part = V1[0] * V2[0] / GeV; 
    DEBUG( "Rank = " << p_wave.rank() );

    Tensor returnValue = p_wave * scalar_part ;
    DEBUG( "Return rank = " << returnValue.rank() );
    return returnValue;
  } 

DEFINE_VERTEX( V_VS_P , pid(1,1,0,1) ) {
  Tensor L  = Orbital_PWave(P,Q) / GeV ; /// orbital part ////
  TensorHelper coupling = LeviCivita()(-mu,-nu,-alpha,-beta) * L( alpha ) * P( beta ) / GeV;   
  return ( coupling * V1(nu) ) * V2[0];
}

DEFINE_VERTEX( V_VS_S , pid(1,1,0,0) ) { return Orbital_PWave( P , V1 ) * V2[0]; }

DEFINE_VERTEX( V_VS_D , pid(1,1,0,2) ){
  Tensor L_2_V0 = Orbital_DWave( P, Q ) / ( GeV * GeV ) ; 
  Tensor Sv = Spin1ProjectionOperator( P );
  return ( Sv( mu , nu ) * L_2_V0( -nu,-alpha) * V1(alpha) ) * V2[0]; 
} 

DEFINE_VERTEX( T_VS_D, pid(2,1,0,2) ) {

  Tensor G = LeviCivita()( -mu,-nu,-alpha,-beta) * P(nu) * Q(alpha) * V1(beta) ;
  Tensor L     = Orbital_PWave( P, Q );  
  return ( G(mu) * L(nu) + L(mu) * G(nu)  ) * V2[0] / Constant(-2*GeV*GeV*GeV);
}


DEFINE_VERTEX( T_TS_S , pid(2,2,0,0) ) {

  Tensor S = Spin1ProjectionOperator( P );
  Tensor term1 =  S(-mu,-alpha) * V1( alpha , beta ) * S( -beta, -nu ) ;
  Tensor term2 =  S*( dot(V1,S) ) / 3.;
  return ( term1 - term2 ) * V2[0];
} 


DEFINE_VERTEX( T_VS_P ,pid(2,1,0,1) ) { 
  Tensor L = Orbital_PWave( P, Q ) / GeV ;
  Tensor S = Spin1ProjectionOperator(P);
  Tensor Vp= S(-mu,-nu) * V1 (nu) ;
  return V2[0]  * ( (  L(alpha) * Vp(beta)   +  L(beta) * Vp(alpha)   )/2. 
      - S(alpha,beta)*dot(L, V1 )/3.) ;
} 

DEFINE_VERTEX( T_SS_D , pid(2,0,0,2) ) { 
  return Orbital_DWave( P, Q ) / (GeV*GeV) ; 
} 

DEFINE_VERTEX( S_TV_P , pid(0,2,1,1) ) { 
  Tensor L = Orbital_PWave( P, Q );
  return ( V1 (mu,nu ) * L(-mu) ) * V2(-nu) ; 
} 

DEFINE_VERTEX( S_TS_D , pid(0,2,0,2) ) { 
  Tensor orbital = Orbital_DWave( P, Q );
  return V2[0] * Tensor( { dot (orbital, V1 ) / (GeV*GeV) } , {1} ) ; 
} 

DEFINE_VERTEX( S_TV_D , pid(0,2,1,2) ) {
  Tensor term1 = V1 (alpha, beta ) * Orbital_DWave( P, Q ) ( -beta, -nu )  ;
  Tensor term2 = LeviCivita()(-mu,-nu,-alpha,-beta) * P( alpha ) * V2( beta ) ; // Antisymmetric( P, V2 );
  return Tensor( {dot( term1, term2 )} ) / (GeV*GeV*GeV);
}

DEFINE_VERTEX( S_TT_S , pid(0,2,2,0) ){ return Tensor( {dot(V1,V2)} ) ; }

DEFINE_VERTEX( V_TS_P , pid(1,2,0,1) ){
  Tensor S = Spin1ProjectionOperator(P);
  Tensor L = Orbital_PWave( P,Q) / (GeV);
  return  ( S(-mu,-nu) * L(-alpha) * V1(nu,alpha) ) * V2[0] ;
}


DEFINE_VERTEX( V_TS_D , pid(1,2,0,2) ){
  Tensor L = (-1) * Orbital_PWave( P,Q);
  Tensor coupling = LeviCivita()(-mu,-nu,-alpha,-beta) * P(nu) * Q(alpha);
  return coupling(-mu,-nu) * V1(nu,alpha) * L(-alpha) / (GeV*GeV*GeV);
}

