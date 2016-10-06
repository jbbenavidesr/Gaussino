#include "AmpGen/Lineshapes.h"

using namespace AmpGen;

LineshapeFactory *LineshapeFactory::s_instance = 0;

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


DEFINE_LINESHAPE(FormFactor){
  Expression radius = Parameter( particleName + "_radius") ;
  const Expression q2 = Q2( s, s1, s2 ) ;
  const Expression FormFactor = Sqrt( BlattWeisskopf_Norm(q2*radius*radius,0,L));
  if( dbexpressions != 0 ){
    add_debug( q2              , dbexpressions );
    add_debug( FormFactor      , dbexpressions );
  };
  return FormFactor * Complex(1.,0.);
}

DEFINE_LINESHAPE(NonRes){ 
  Expression radius = Parameter( particleName + "_radius");
  Expression mass = Parameter( particleName +"_mass");

  const Expression q2 = Abs(Q2( s, s1, s2 ));
  const Expression q20 = Abs(Q2( mass*mass , s1, s2 ) );
  Expression FormFactor = Sqrt( BlattWeisskopf_Norm( q2*radius*radius, 0, L ) ) ;

  if( dbexpressions != 0 ){
    add_debug(FormFactor, dbexpressions);
    add_debug(q2, dbexpressions);
  }
  return FormFactor * Complex(1.,0.);
}

Expression AmpGen::Gamma( const Expression& s, const Expression& s1, const Expression& s2, const Expression& mass, const Expression& width, const Expression& radius, unsigned int L, std::vector<DBSYMBOL>* dbexpressions){

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

DEFINE_LINESHAPE(BW){
  const Expression& mass = Parameter( particleName +"_mass");
  const Expression& width = Parameter( particleName +"_width");
  const Expression& radius = Parameter( particleName +"_radius");
  const Expression q2 = Abs(Q2( s, s1, s2 ));
  const Expression q20 = Abs(Q2( mass*mass , s1, s2 ) );

  Expression FormFactor = Sqrt ( BlattWeisskopf_Norm( q2*radius*radius,0, L ) ) ; 
  if( lineshapeModifier == "BL" ) FormFactor = Sqrt ( BlattWeisskopf( q2*radius*radius, L ) ) ; 
  const Expression runningWidth = Gamma( s,s1,s2, mass, width, radius, L , dbexpressions) / 1000.;
  const Expression massInGeV = mass / 1000.;
  const Expression sInGeV = s / (1000.*1000.); 
  const Expression BW = FormFactor / Complex(  massInGeV*massInGeV - sInGeV , -massInGeV*runningWidth);
  const Expression kf = kFactor(mass,width,dbexpressions);
  if( dbexpressions != 0 ){
    add_debug( Sqrt(s), dbexpressions );
    add_debug( s1, dbexpressions );
    add_debug( s2 , dbexpressions );
    add_debug( q2, dbexpressions );
    add_debug( q20, dbexpressions);
    add_debug( radius, dbexpressions );
    add_debug( FormFactor, dbexpressions);
    add_debug( runningWidth, dbexpressions);
    add_debug( BW, dbexpressions);
    add_debug( mass, dbexpressions );
    add_debug( width, dbexpressions);
    add_debug( radius, dbexpressions);
    add_debug( kf     , dbexpressions );
    add_debug( kf*BW, dbexpressions );
  }
  return  kf*BW; ;
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

Expression aSqrtTerm(const Expression& m0, const Expression& m){
  Expression a2 = 1.0 - (2*m0/m)*(2*m0/m);
  return Ternary( a2 > Constant(0), Sqrt(a2), Constant(0) ); 
}

DEFINE_LINESHAPE(Flatte){
  Expression mass = Parameter( particleName +"_mass");
  Expression radius = Parameter( particleName +"_radius");

  Expression gPi = Parameter("Flatte::gPi",165.);
  Expression gK_by_gPi = Parameter("Flatte::gK_by_gPi",4.21);
  Constant mPi0(134.9766);
  Constant mPiPlus(139.57018);
  Constant mK0(497.648);
  Constant mKPlus(493.677);

  const Expression mreco = Sqrt( s );
  Expression Gpipi = (1./3.) * aSqrtTerm(    mPi0, mreco ) 
    +            (2./3.) * aSqrtTerm( mPiPlus, mreco );


  Expression GKK   = (1./2.) * aSqrtTerm(     mK0, mreco ) 
    +            (1./2.) * aSqrtTerm(  mKPlus, mreco );

  Expression FlatteWidth = gPi * Gpipi + gK_by_gPi * GKK;
  const Expression q2 = Abs(Q2( s, s1, s2 )); 
  const Expression q20 = Abs(Q2( mass*mass , s1, s2 ) );
  Constant one(1);
  const Expression BW = 1000000. / Complex( -s + mass*mass, -mass*FlatteWidth);
  if( dbexpressions != 0 ){
    add_debug( gPi, dbexpressions );
    add_debug( mass, dbexpressions );
    add_debug( gK_by_gPi, dbexpressions );
    add_debug( q2, dbexpressions );
    add_debug( q20, dbexpressions);
    add_debug( Gpipi, dbexpressions);
    add_debug( GKK, dbexpressions);
    add_debug( FlatteWidth, dbexpressions); 
    add_debug( BW, dbexpressions);
  }
  return BW;
}

DEFINE_LINESHAPE(SBW){
  Expression mass = Parameter( particleName +"_mass");
  Expression radius = Parameter( particleName +"_radius");
  Expression width = Parameter( particleName +"_width");

  const Expression q2 = Abs(Q2( s, s1, s2 )); 
  const Expression q20 = Abs( Q2(mass*mass, s1, s2 ) );
  Expression BF = BlattWeisskopf_Norm( q2*radius*radius, 0, L );
  const Expression BW = Sqrt( kFactor(mass,width) ) * (1000.) / Complex( -s + mass*mass, -mass*width);
  if( dbexpressions != 0 ){
    add_debug( Sqrt(s), dbexpressions );
    add_debug( q2, dbexpressions );
    add_debug( q20, dbexpressions);
    add_debug( radius, dbexpressions );
    add_debug( BF, dbexpressions);
    add_debug( width, dbexpressions);
    add_debug( BW, dbexpressions);
  }
  return BW;
}

bool LineshapeFactory::isLineshape( const std::string& lineshape ){

  auto gLineshapes = LineshapeFactory::get()->lineshapes();
  size_t pos = lineshape.find(".");
  if( pos == std::string::npos )
    return gLineshapes.find(lineshape) != gLineshapes.end() ;
  else
    return gLineshapes.find( lineshape.substr(0,pos) ) != gLineshapes.end() ;
}

Expression LineshapeFactory::getLineshape( const std::string& lineshape
    , const Expression& s
    , const Expression& s1
    , const Expression& s2
    , const std::string& particleName
    , const unsigned int& L 
    , std::vector< std::pair< std::string, Expression >>* dbexpressions  ){
  size_t pos = lineshape.find(".");

  auto ls = LineshapeFactory::get()->lineshapes();

  if( pos == std::string::npos ){
    auto it = ls.find(lineshape);
    if( it == ls.end() ) ERROR( "Lineshape : " << lineshape << " not found" );
    return ls[lineshape]->get(s,s1,s2, particleName, L, "", dbexpressions );
  }
  else{
    return ls[ lineshape.substr(0,pos) ]->get(
        s,s1,s2,particleName,L,lineshape.substr(pos+1), dbexpressions );
  }
}

