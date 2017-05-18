#include "AmpGen/Lineshapes.h"

using namespace AmpGen;

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

