#include "AmpGen/Lineshapes.h"
#include "AmpGen/Trigonometry.h"

#define PHASE( x ) Complex( Cos(x), Sin(x) ) 

using namespace AmpGen;

DEFINE_LINESHAPE(LASS){
  Expression mass = Parameter( particleName +"_mass");
  Expression width = Parameter( particleName+"_width");
  Expression a = Parameter( "LASS::a", 2.07 );
  Expression r = Parameter( "LASS::r", 3.32 );
  const Expression q2 = Q2( s, s1, s2 ) / ( 1000.*1000.);
  const Expression q = Sqrt( q2 );
  const Expression gamma = Gamma(s,s1,s2,mass,width,0,0);
  const Expression scatteringPhase = aTan( 2 * a * q / ( 2 + a * r *q2 ) );
  const Expression resonancePhase = aTan( mass * gamma / ( mass*mass -s ) );
  const Expression phase = scatteringPhase + resonancePhase;
  const Expression rho = 1000.*Sqrt( q2 / s );
  const Expression returnValue = Sin(phase)*Complex( Cos(phase), Sin(phase ) ) / rho ;
  if( dbexpressions != 0 ){
    add_debug( s, dbexpressions );
    add_debug( q2, dbexpressions );
    add_debug( q, dbexpressions );
    add_debug( scatteringPhase, dbexpressions );
    add_debug( resonancePhase, dbexpressions );
    add_debug( rho, dbexpressions );
    add_debug( returnValue, dbexpressions );
    add_debug( gamma, dbexpressions );
  };

  return returnValue;
}



DEFINE_LINESHAPE(gLASS){
  Expression mass = Parameter( particleName +"_mass");
  Expression width = Parameter( particleName+"_width");
  const Expression sK = 493.677*493.677;
  const Expression sPi = 139.57*139.57;
  Expression a = Parameter( "gLASS::a", 2.07 ); /// short distance coupling  
  Expression r = Parameter( "gLASS::r", 3.32 ); /// range parameter 
  Expression F = Parameter( "gLASS::F"+lineshapeModifier, 1.0 ) ; /// vec[0];
  Expression R = Parameter( "gLASS::R"+lineshapeModifier, 1.0 ) ; /// resonant coupling 
  Expression phiF = Parameter("gLASS::phiF"+lineshapeModifier,0.0) ; /// F-phase
  Expression phiR = Parameter("gLASS::phiR"+lineshapeModifier,0.0) ; /// R-phase

  const Expression q2 = Q2( s,s1,s2 ) / ( 1000.*1000.);
  const Expression q = Sqrt( q2 );
  const Expression gamma = Gamma(s,s1,s2,mass,width,0,0);
  const Expression scatteringPhase = phiF + aTan( 2 * a * q / ( 2 + a * r *q2 ) );
  const Expression resonancePhase = phiR + aTan( mass * gamma / ( mass*mass -s ) );
  const Expression rho = 1000.*Sqrt( q2 / s );
  const Expression returnValue =( 
      F * Sin( scatteringPhase )*PHASE( scatteringPhase ) +
      R * Sin( resonancePhase  )*PHASE( resonancePhase + 2 * scatteringPhase )
      ) / rho ; 
  if( dbexpressions != 0 ){
    add_debug( q , dbexpressions );
    add_debug( rho, dbexpressions );
    add_debug( gamma, dbexpressions );
    add_debug( rho, dbexpressions );
    add_debug( scatteringPhase, dbexpressions );
    add_debug( resonancePhase, dbexpressions );

  };

  return returnValue; 
}

DEFINE_LINESHAPE(Lupton){
  const Expression lassPart = LASS().get( s,s1,s2, particleName, L, lineshapeModifier,  dbexpressions );
  Expression b1 = Parameter( "Lupton::b1", -15.0 ); /// short distance coupling  
  Expression b2 = Parameter( "Lupton::b2", 26.97 ); /// range parameter 
  Expression b3 = Parameter( "Lupton::b3", -20.02 ) ; /// vec[0];
  Expression m = Sqrt( s );
  Expression q = ( m / Parameter( particleName+"_mass") ) - 1. ;
  Expression ff = b1*q + b2*q*q + b3*q*q*q;
  if( dbexpressions != 0 ){
    add_debug( q, dbexpressions);
    add_debug( ff, dbexpressions );
  };
  return Exp( ff ) * lassPart;

}
