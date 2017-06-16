#include "AmpGen/Lineshapes.h"

using namespace AmpGen;

Expression AmpGen::Q2(const Expression& Msq, const Expression& M1sq, const Expression& M2sq){
  const Expression num = Msq/4. -( M1sq + M2sq)/2. + ( M1sq - M2sq)*( M1sq - M2sq)/(4*Msq);  
  return num ; 
}

Expression AmpGen::kFactor( const Expression& mass,  const Expression& width ,
    std::vector<DBSYMBOL>* dbexpressions
    ){
  /// 
  const Expression massInGeV = mass / 1000.;
  const Expression widthInGeV = width / 1000.;
  const Expression gammaInGeV = massInGeV * Sqrt( massInGeV * massInGeV + widthInGeV * widthInGeV );
  const Expression kInGeV = 2*sqrt(2)*massInGeV*widthInGeV*gammaInGeV
    / ( M_PI * Sqrt( massInGeV * massInGeV + gammaInGeV ) );
  if( dbexpressions != 0 ) {
    add_debug( gammaInGeV , dbexpressions );
    add_debug( Sqrt(kInGeV)     , dbexpressions );
  };
  return Sqrt(kInGeV) ;
}

Expression AmpGen::BlattWeisskopf_Norm( const Expression& z2, const Expression& z02 , unsigned int L){
  if( L==0 ) return Expression( Constant(1) );
  else if( L==1) return (1+z02)/(1+z2);
  else if( L==2) return ( z02*z02 + 3*z02 + 9 ) / ( z2*z2 + 3*z2 + 9 )  ;
  else {
    std::cout << "Error - cannot understand L > 2" << std::endl;
    return Expression(Constant(0));
  }
}

Expression AmpGen::BlattWeisskopf( const Expression& z2, unsigned int L){
  if( L==0 ) return Expression( Constant(1));
  else if( L==1) return  2*z2  / ( 1 + z2 ) ;
  else if( L==2) return  (13*z2*z2 ) /  ( z2*z2 + 3*z2 + 9 ) ; 
  else {
    std::cout << "Error - cannot understand L > 2" << std::endl;
    return Expression(Constant(0));
  }
}


Expression AmpGen::Gamma( const Expression& s, 
                          const Expression& s1, 
                          const Expression& s2, 
                          const Expression& mass, 
                          const Expression& width, 
                          const Expression& radius, 
                          unsigned int L, 
                          std::vector<DBSYMBOL>* dbexpressions){

  const Expression q2 = Abs(Q2( s, s1 , s2 )); 
  const Expression q20 = Abs(Q2( mass*mass , s1, s2 ) );
  Expression BF = BlattWeisskopf_Norm( q2*radius*radius, q20*radius*radius, L );
  Expression qr = Sqrt( q2 / q20 );
  for( unsigned int i = 0 ; i < L; ++i ) qr = qr*(q2/q20);

  const Expression mreco = Sqrt(s);
  const Expression mr = mass / mreco;
  /*
     if( dbexpressions != 0 ){
     add_debug( q2, dbexpressions );
     add_debug( q20, dbexpressions );
     add_debug( Sqrt(q2/q20), dbexpressions );
     add_debug( BF, dbexpressions );
     add_debug( qr, dbexpressions );
     add_debug( mr, dbexpressions );
     }
     */
  return width*BF*qr*mr;
}

Expression kFactor( const Expression& mass,  const Expression& width , 
    std::vector<DBSYMBOL>* dbexpressions 
    ){
  /// 
  const Expression massInGeV = mass / 1000.;
  const Expression widthInGeV = width / 1000.;
  const Expression gammaInGeV = massInGeV * Sqrt( massInGeV * massInGeV + widthInGeV * widthInGeV );
  const Expression kInGeV = 2*sqrt(2)*massInGeV*widthInGeV*gammaInGeV 
    / ( M_PI * Sqrt( massInGeV * massInGeV + gammaInGeV ) );
  if( dbexpressions != 0 ) {
    add_debug( gammaInGeV , dbexpressions );
    add_debug( Sqrt(kInGeV)     , dbexpressions );
  };
  return Sqrt(kInGeV) ; 
}



//// Lineshape factory declarations //// 

template <> Factory<AmpGen::IShape>* Factory<AmpGen::IShape>::gImpl = 0;

bool LineshapeFactory::isLineshape( const std::string& lineshape ){

  size_t pos = lineshape.find(".");
  if( pos == std::string::npos )
    return get(lineshape) != 0 ;
  else
    return get( lineshape.substr(0,pos) ) != 0 ;
}

Expression LineshapeFactory::getLineshape( const std::string& lineshape
    , const Expression& s
    , const Expression& s1
    , const Expression& s2
    , const std::string& particleName
    , const unsigned int& L 
    , std::vector< std::pair< std::string, Expression >>* dbexpressions  ){
  size_t pos = lineshape.find(".");
  
  if( pos == std::string::npos ){
    auto it = get(lineshape);
    if( !it ) ERROR( "Lineshape : " << lineshape << " not found" );
    return it->get(s,s1,s2, particleName, L, "", dbexpressions );
  }
  else{
    return get( lineshape.substr(0,pos) )->get(
        s,s1,s2,particleName,L,lineshape.substr(pos+1), dbexpressions );
  }
}

