#include "AmpGen/Lineshapes.h"
#include "AmpGen/Array.h"
#include "AmpGen/Unary.h"
#include "AmpGen/NamedParameter.h"
#include "AmpGen/Trigonometry.h"

using namespace AmpGen;

Expression getSpline( const std::string& name, const Expression& x, const std::string& arrayName, std::vector<DBSYMBOL>* dbexpressions = 0 ){


  double min, max, nBins(0);
    min = AmpGen::NamedParameter<double>( name+"::Spline::Min", 0. ).getVal();
    max = AmpGen::NamedParameter<double>( name+"::Spline::Max", 0. ).getVal();
    nBins = AmpGen::NamedParameter<double>( name+"::Spline::N", 0. ).getVal();

  double spacing = ( max - min )/((double)nBins - 1. );
  Expression dx = Fmod( x - min , spacing );
  Expression bin = ( x - min )  / spacing ;
  Array m_x, m_xf;
  for( unsigned int i =0;i<nBins ; ++i ){
    m_x.push_back ( Parameter( name+"::Spline::"+arrayName+"::" +std::to_string(i) ));
    m_xf.push_back( Parameter( name+"::Spline::"+arrayName+"::C::"+std::to_string(i))); // curvature term /// 
  }
   
  if( dbexpressions != 0 ){
    add_debug( dx, dbexpressions );
    add_debug( bin, dbexpressions);
    add_debug( m_x[bin], dbexpressions );
    add_debug( x > min && x < max , dbexpressions ); 
    for( unsigned int i =0;i<nBins ; ++i ){
      add_debug( m_x[i], dbexpressions );
      add_debug( m_xf[i], dbexpressions );    
    }


  };
  Expression returnValue = Ternary( x > min && x < max , m_x[bin] +
      ( (m_x[bin+1]-m_x[bin])/spacing  - (m_xf[bin+1]+2*m_xf[bin])*spacing / 6. )*dx +
      m_xf[bin] *dx *dx / 2. +
      dx*dx*dx*( m_xf[bin+1] - m_xf[bin] ) / (6.*spacing) , 0 ) ;
  return returnValue;
}

DEFINE_LINESHAPE(DecaySpline){
  const Expression q2 = Q2( s, s1,s2 ) / (1000.*1000.);
  if( dbexpressions != 0 ){
    add_debug( q2 , dbexpressions );
  }
  return getSpline( particleName, q2, "FF" );
}


DEFINE_LINESHAPE(WidthSpline){
  const Expression& mass = Parameter( particleName +"_mass");
  const Expression& width = Parameter( particleName +"_width");
  const Expression& radius = Parameter( particleName +"_radius");
  const Expression q2 = Abs(Q2( s,s1,s2 ));
  const Expression BF = BlattWeisskopf_Norm( q2*radius*radius, 0, L ) ; // : 
  const Expression runningWidth = (width / 1000. ) * getSpline( particleName, s / (1000.*1000.), "Gamma" , dbexpressions );
  const Expression norm = kFactor( mass, width) * Sqrt(BF) ;
  const Expression massInGeV = mass / 1000.;
  const Expression sInGeV = s / (1000.*1000.);
  const Expression BW = norm / Complex(  massInGeV*massInGeV - sInGeV , -massInGeV*runningWidth);

  if( dbexpressions != 0 ){
    add_debug( mass, dbexpressions );
    add_debug( q2 , dbexpressions );
    add_debug( BF , dbexpressions );
    add_debug( s, dbexpressions );
    add_debug( runningWidth , dbexpressions );
    add_debug( kFactor(mass,width), dbexpressions );
    add_debug( BW, dbexpressions );
  };
  return BW ;
}


DEFINE_LINESHAPE(FormFactorSpline){
  const Expression& mass = Parameter( particleName +"_mass");
  const Expression& width = Parameter( particleName +"_width");
  const Expression& radius = Parameter( particleName +"_radius");
  const Expression q2 = Abs(Q2( s,s1,s2 ));
  const Expression q20 = Abs(Q2( mass*mass ,s1,s2  ) );
  const Expression runningWidth = Gamma( s,s1,s2, mass, width, radius, L , dbexpressions); 
  const Expression BW = 1000000. * getSpline(particleName,s,"FF") 
    / Complex( -s + mass*mass, -mass*runningWidth);
  return BW;
}

/// Brian's suggestion for an unusual combination of LASS and splines ///
DEFINE_LINESHAPE(LassSpline){
  Expression modulus = getSpline( particleName,s/(1000.*1000.),"LASS");
  return modulus * LASS().get(s,s1,s2, particleName, L, lineshapeModifier, dbexpressions);
}

DEFINE_LINESHAPE(Spline){
  Expression real = getSpline( particleName,s/(1000.*1000.),"Re") ; 
  Expression imag = getSpline( particleName,s/(1000.*1000.),"Im") ; 

  if( dbexpressions != 0 ){
    add_debug( real, dbexpressions);
    add_debug( imag, dbexpressions);
  }

  return lineshapeModifier != "Polar" ? 
           Complex ( real , imag ) : 
           real * Complex( Cos(imag), Sin(imag ) ) ;
}
