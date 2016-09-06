#include "AmpGen/Lineshapes.h"

//// implementation of Gounaris Sakurai Lineshape /// 
using namespace AmpGen;


Expression k(Expression mpipi) 
{
  Constant mpi(139.57018);
  return Sqrt(0.25*mpipi-Pow(mpi,2));
}

Expression k2(Expression mpipi) 
{
  Constant mpi(139.57018);
  return 0.25*mpipi-Pow(mpi,2);
}

Expression kprime(Expression mpipi) 
{
  Constant mpi(139.57018);
  return 1.0/(8.0*Sqrt(0.25*mpipi-Pow(mpi,2)));
}

Expression h(Expression mpipi) 
{
  Constant mpi(139.57018);
  Expression logterm = Log((Sqrt(mpipi)+2.0*k(mpipi))/(2.0*mpi));
  return (2.0*k(mpipi)*logterm)/(M_PI*Sqrt(mpipi));
}
Expression hprime(Expression mpipi) 
{
  return h(mpipi)*(0.125/(k(mpipi)*k(mpipi))-0.5/mpipi)+0.5/(M_PI*mpipi); 
}

Expression d(const Expression& mpipi , const Expression& m, std::vector<DBSYMBOL>* dbexpressions=0 ) 
{
  //// this is just a constant, so we ignore ///
  
  Constant mpi(139.57018);
  Expression logfactor = Log((m+2*k(m*m))/(2.0*mpi));
  Expression term1 = (3.0/M_PI)*Pow(mpi/k(m*m),2)*logfactor;
  Expression term2 = m/(2.0*M_PI*k(m*m));
  Expression term3 = Pow(mpi,2)*m/(M_PI*Pow(k(m*m),3));

  return term1+term2-term3;
  
}
DEFINE_LINESHAPE(GS){
  Expression mass = Parameter( particleName +"_mass");
  Expression radius = Parameter( particleName +"_radius");
  Expression width = Parameter( particleName +"_width");

  Expression s0 = mass*mass;
  Expression GsTerm = k2(s)*( h(s) - h(s0) ) + k2(s0)*hprime(s0)*( s0 - s );
  Expression BWReal = s0 - s + width*s0* GsTerm   / ( k(s0) * k(s0) *k(s0) );
  Expression BWIm = -mass*Gamma(s,s1,s2,mass,width,radius,L);
  const Expression q2 = Abs(Q2( s, s1, s2 ));
  const Expression q20 = Abs(Q2( mass*mass , s1, s2 ) );

  Expression BF = Sqrt( BlattWeisskopf_Norm( q2*radius*radius, 0 , L ) );
  if( dbexpressions != 0 ){
    add_debug( BF, dbexpressions );
    add_debug( Sqrt(s) , dbexpressions );
    add_debug( Sqrt(s0) , dbexpressions );
    add_debug( GsTerm , dbexpressions );
    add_debug( BWReal , dbexpressions );
    add_debug( BWIm , dbexpressions ); 
    add_debug( h(s), dbexpressions);
    add_debug( h(s0), dbexpressions );
    add_debug( k(s) , dbexpressions );
    add_debug( k(s0) , dbexpressions );
    add_debug( d(s,mass) , dbexpressions );
  }  
  Expression FF = BF*(1 + d(s,mass,dbexpressions)*width / mass );
  return FF*1000.*1000.*(1. + d(s, mass) ) / Complex( BWReal, BWIm );

}
