#include "AmpGen/Expression.h"

using namespace AmpGen;

  DEFINE_CAST( Log )
  DEFINE_CAST( Sqrt )
  DEFINE_CAST( Exp )
  DEFINE_CAST( Abs )
  DEFINE_CAST( Sin )
  DEFINE_CAST( Cos )
  DEFINE_CAST( Tan )
  DEFINE_CAST( aSin)
  DEFINE_CAST( aCos) 
DEFINE_CAST( aTan) 

  //// logarithm member functions ////
  Log::Log( const Expression& other ) : IUnaryExpression( other ) { }

  std::string Log::to_string() const {
    return "log(" + m_expression.to_string() +")";
  }
std::complex<double> Log::complexEval() const { 
  return log( m_expression.complexEval() ); 
}

double Log::realEval() const { return log( m_expression.realEval() ); }
Expression Log::conjugate() const { return Log(m_expression.conjugate()) ; }
Expression Log::d( const Parameter& div ){ return 1./m_expression; }
//// end logarithm member functions 

//// sqrt member functions ////

Sqrt::Sqrt( const Expression& other ) :  IUnaryExpression(other)  {} 
std::string Sqrt::to_string() const { return "sqrt(" + m_expression.to_string() +")";}
Expression Sqrt::d ( const Parameter& div ) { return m_expression.d(div)/(2*Sqrt(m_expression));  }
std::complex<double> Sqrt::complexEval() const { return sqrt( m_expression.complexEval() ); }
double Sqrt::realEval() const { return sqrt(m_expression.realEval() ); }
Expression Sqrt::conjugate() const { return Sqrt(m_expression.conjugate()) ; }

//// End sqrt member functions //// 

//// exp member functions ////

Exp::Exp( const Expression& other ) : IUnaryExpression(other) {}
std::string Exp::to_string() const {
  return "exp(" + m_expression.to_string() +")";
}
Expression Exp::d( const Parameter& div ) { return Exp(m_expression)*m_expression.d(div);  }
std::complex<double> Exp::complexEval() const { return exp( m_expression.complexEval() ); }
double Exp::realEval() const { return exp( m_expression.realEval() ); }
Expression Exp::conjugate() const { return Exp(m_expression.conjugate()) ; }

//// end logarithm member functions 

//// abs member functions ////

Abs::Abs( const Expression& expression ) : IUnaryExpression(expression) {}
std::string Abs::to_string() const 
{
  return "fabs("+m_expression.to_string() +")";
}
Expression Abs::d( const Parameter& div )
{
  return Ternary( m_expression > Constant(0) , m_expression.d(div), - m_expression.d(div) );  
}

double Abs::realEval() const { return abs(m_expression.realEval()); }
std::complex<double> Abs::complexEval() const { return m_expression.complexEval(); }
Expression Abs::conjugate() const { return Expression( std::make_shared<Abs>(*this) )  ; }


Sin::Sin( const Expression& expression ) : IUnaryExpression(expression){} 
std::string Sin::to_string() const { return "sin("+m_expression.to_string()+")" ; }
double Sin::realEval() const { return sin( m_expression.realEval() ); }
std::complex<double> Sin::complexEval() const { return sin( m_expression.complexEval() ) ; }
Expression Sin::conjugate() const { return Sin( m_expression.conjugate() ) ; }


Cos::Cos( const Expression& expression ) : IUnaryExpression(expression){}
std::string Cos::to_string() const { return "cos("+m_expression.to_string()+")"; } 
double Cos::realEval() const { return cos( m_expression.realEval() ); }
std::complex<double> Cos::complexEval() const { return cos( m_expression.complexEval() ) ; }
Expression Cos::conjugate() const { return Cos( m_expression.conjugate() ) ; }

Tan::Tan( const Expression& expression ) : IUnaryExpression(expression){}
std::string Tan::to_string() const { return "tan("+m_expression.to_string()+")"; } 
double Tan::realEval() const { return tan( m_expression.realEval() ); }
std::complex<double> Tan::complexEval() const { return tan( m_expression.complexEval() ) ; }
Expression Tan::conjugate() const { return Tan( m_expression.conjugate() ) ; }

aSin::aSin( const Expression& expression ) : IUnaryExpression(expression){}
std::string aSin::to_string() const { return "asin("+m_expression.to_string()+")" ; }
double aSin::realEval() const { return asin( m_expression.realEval() ); }
std::complex<double> aSin::complexEval() const { return asin( m_expression.complexEval() ) ; }
Expression aSin::conjugate() const { return aSin( m_expression.conjugate() ) ; }

aCos::aCos( const Expression& expression ) : IUnaryExpression(expression){}
std::string aCos::to_string() const { return "acos("+m_expression.to_string()+")"; }
double aCos::realEval() const { return acos( m_expression.realEval() ); }
std::complex<double> aCos::complexEval() const { return acos( m_expression.complexEval() ) ; }
Expression aCos::conjugate() const { return aCos( m_expression.conjugate() ) ; }


aTan::aTan( const Expression& expression ) : IUnaryExpression(expression){}
std::string aTan::to_string() const { return "atan("+m_expression.to_string()+")"; } 
double aTan::realEval() const { return atan( m_expression.realEval() ); }
std::complex<double> aTan::complexEval() const { return atan( m_expression.complexEval() ) ; }
Expression aTan::conjugate() const { return aTan( m_expression.conjugate() ) ; }

Expression Cos::d( const Parameter& div)
{
  return Constant(-1)*Sin( m_expression ) * m_expression.d(div);
} 

Expression Sin::d( const Parameter& div){
  return Cos( m_expression ) * m_expression.d(div);
}

Expression Tan::d( const Parameter& div){
  return m_expression.d(div) / ( Cos( m_expression ) * Cos(m_expression) );
}

Expression aCos::d( const Parameter& div){
  return m_expression.d(div) / Sqrt( 1 - m_expression * m_expression );
}
Expression aSin::d( const Parameter& div){
  return - m_expression.d(div) / Sqrt( 1 - m_expression * m_expression );
}
Expression aTan::d( const Parameter& div){
  return m_expression.d( div )  / ( 1 + m_expression * m_expression );
}

