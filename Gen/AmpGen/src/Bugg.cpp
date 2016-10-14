#include "AmpGen/Lineshapes.h"
#include "AmpGen/NamedParameter.h"

//// implementation of scalar S-wave Lineshape suggested in D. V. Bugg, J. Phys. G34:151, 2007, arXiv:hep-ph/0608081  ///

using namespace AmpGen;

Expression rho(const Expression& s, const Expression& s0){
  Expression a2 = 1.0 - (4*s0)/ s;
  return Ternary( a2 > 0, a2, Constant(0) ); 
}

Expression rho_2( const Expression& s, const Expression& s0){
  Complex a2 = Complex( 1.0 - (4*s0)/s, 0 );
  return Sqrt(a2);
}


Expression q( const Expression& s, const Expression& s0 ){
  return Ternary( s > 4*s0 , s - 4*s0, 4*s0 - s );
}

Expression rho_4pi(const Expression& s, const Expression& lambda, const Expression& s0){
    return 1. / ( 1 + Exp( lambda*(s0-s)));
}

Expression Buggj1(const Expression& s, const Expression& m0){
    
    Expression rho_pipi = Sqrt( rho(s,m0*m0));
    Expression addTerm = Ternary( rho_pipi > 0., rho_pipi * Log((1.-rho_pipi)/(1.+rho_pipi)), 0.);
    return (2.+addTerm)/M_PI;
}

Expression Gamma_4pi(const Expression& s, const Expression& m0, const Expression& M, const Expression& g_4pi, const Expression& lambda, const Expression& s0){
    return Ternary(s > 16. * m0 * m0,  g_4pi * rho_4pi(s,lambda,s0)/rho_4pi(M*M,lambda,s0), 0.);
}

DEFINE_LINESHAPE(Bugg){
    Expression M = Parameter("Bugg::M",0.935);
    Expression b1 = Parameter("Bugg::b1", 1.302);
    Expression b2 = Parameter("Bugg::b2",0.340);
    Expression A = Parameter("Bugg::A",2.426);
    Expression g_4pi = Parameter("Bugg::g_4pi",0.011);
    Expression g_2K = Parameter("Bugg::g_2K",0.6);
    Expression g_2eta = Parameter("Bugg::g_2eta",0.2);
    Expression alpha = Parameter("Bugg::alpha",1.3);
    Constant  mPiPlus(139.570/1000.);
    Constant  mKPlus(493.677/1000.);
    Constant  mEta(547.862/1000.);

    Expression sA = Parameter("Bugg::sA",0.41) * mPiPlus * mPiPlus;
    Expression s0_4pi = Parameter("Bugg::s0_4pi",7.082/2.845);
    Expression lambda_4pi = Parameter("Bugg::lambda_4pi",2.845);
    
    Expression sInGeV = s/(1000.*1000.);
    
    Expression z = Buggj1(sInGeV,mPiPlus)-Buggj1(M*M,mPiPlus);
    
    Expression g1sg = M * ( b1 + b2 * sInGeV ) * Exp( - ( sInGeV - M*M ) / A );
    Expression adlerZero = ( sInGeV - sA ) / ( M * M - sA ) ;

    Expression gamma_2pi = g1sg * adlerZero * rho_2(sInGeV, mPiPlus * mPiPlus );

    Expression gamma_2K = g_2K * g1sg * sInGeV/(M*M) 
      * Exp( -alpha * q(sInGeV, mKPlus * mKPlus ) ) 
      * rho_2(sInGeV, mKPlus * mKPlus );

    Expression gamma_2eta = g_2eta * g1sg * sInGeV/(M*M) 
      * Exp( -alpha * q(sInGeV, mEta * mEta ) ) 
      * rho_2(sInGeV,mEta*mEta);
    Expression gamma_4pi = M * Gamma_4pi(sInGeV,mPiPlus,M,g_4pi,lambda_4pi,s0_4pi);
    
    Expression Gamma_tot = gamma_2pi + gamma_2K + gamma_2eta + gamma_4pi;
    Expression az = g1sg * adlerZero;

    Expression iBW = Complex(M*M-sInGeV- az*z, 0) - Complex(0,1)*Gamma_tot ;
    
    Expression BW = 1. / iBW ; 
    
    if( dbexpressions != 0 ){
        
        add_debug(M,   dbexpressions );
        add_debug(g1sg     , dbexpressions );
        add_debug(gamma_2pi, dbexpressions );
        add_debug(gamma_2K, dbexpressions );
        add_debug(gamma_2eta, dbexpressions );
        add_debug(gamma_4pi, dbexpressions );
        add_debug( sA, dbexpressions );
        add_debug(adlerZero, dbexpressions ); 
        add_debug(Gamma_tot,          dbexpressions);
        add_debug(s,          dbexpressions);
        add_debug( z , dbexpressions );
        add_debug( Buggj1(sInGeV,mPiPlus), dbexpressions );
        add_debug( iBW , dbexpressions );
        add_debug( az*z , dbexpressions );
        add_debug( rho_4pi(sInGeV,lambda_4pi,s0_4pi) , dbexpressions ); 
   }
    return BW;
}
