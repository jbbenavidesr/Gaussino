#include "AmpGen/Expression.h"

#include <complex>

using namespace AmpGen; 

Expression AmpGen::operator+(const Expression& A, const Expression& B){ 
  const Constant* rawA = dynamic_cast< Constant* >( A.get() );
  const Constant* rawB = dynamic_cast< Constant* >( B.get() );
  if( rawA != 0 && rawA->m_value == 0. ) return B;
  else if ( rawB != 0 && rawB->m_value == 0. ) return A;
  return Expression( Sum( A, B ) ); 
}
Expression AmpGen::operator-(const Expression& A, const Expression& B){ 
  const Constant* rawA = dynamic_cast< Constant* >( A.get() );
  const Constant* rawB = dynamic_cast< Constant* >( B.get() );
  if( rawA != 0 && rawA->m_value == 0. ) return -B;
  else if ( rawB != 0 && rawB->m_value == 0. ) return A;
  return Expression( Sub( A, B ) ); 
}
Expression AmpGen::operator*(const Expression& A, const Expression& B){ 
  const IExpression* iA = A.get();
  const IExpression* iB = B.get();
  const Constant* rawA = dynamic_cast< const Constant* >( iA );
  const Constant* rawB = dynamic_cast< const Constant* >( iB );
  if( rawA != 0 ){
    if( rawA->m_value == 0. ) return Expression( Constant(0) );
    else if ( rawA->m_value == 1. ) return B;
    if( rawB != 0 ) return Constant( rawA->m_value * rawB->m_value );
  }
  else if( rawB != 0 ){
    if( rawB->m_value == 0. ) return Expression( Constant(0) );
    else if ( rawB->m_value == 1. ) return A;
  }
  return Expression( Product(A,B) ); 

}
Expression AmpGen::operator&&(const Expression& A, const Expression& B){ return Expression( And(A,B)); }

Expression AmpGen::operator/(const Expression& A, const Expression& B){ return Expression( Divide(A,B) ); }

Expression AmpGen::operator+(const Expression& A, const double &B){ return A + Constant(B); }
Expression AmpGen::operator-(const Expression& A, const double &B){ return A - Constant(B); }
Expression AmpGen::operator*(const Expression& A, const double &B){ return A * Constant(B); }
Expression AmpGen::operator/(const Expression& A, const double &B){ return A / Constant(B); }

Expression AmpGen::operator+(const double& A, const Expression &B){ return Constant(A) + B; }
Expression AmpGen::operator-(const double& A, const Expression &B){ return Constant(A) - B; }
Expression AmpGen::operator*(const double& A, const Expression &B){ return Constant(A) * B; }
Expression AmpGen::operator/(const double& A, const Expression &B){ return Constant(A) / B; }

Complex AmpGen::operator*( const Complex& A, const Complex& B){
  return Complex( A.re * B.re - A.im * B.im , A.re*B.im + A.im*B.re );
}
Complex AmpGen::operator+( const Complex& A, const Complex& B){
  return Complex( A.re + B.re, A.im + B.im );
}

Complex::Complex( const std::complex<double>& value ) : re( Constant(value.real() ) ), im( Constant(value.imag())) {}

Expression AmpGen::operator<(const Expression& A, const Expression& B){ return Expression( LessThan( A, B ) ); } 
Expression AmpGen::operator>(const Expression& A, const Expression& B){ return Expression( GreaterThan( A, B ) ); } 

/// derivatives of the elementary functions 

Expression Parameter::d(const Parameter& div) { 
  return Expression( Constant ( div.m_name == m_name ? 1 : 0 ) ); 
}

Expression Constant::d( const Parameter& /*div*/ ) { 
  return Expression( Constant(0) ); 
}

Expression Sum::d(const Parameter& div )  { 
  return lval.d( div ) + rval.d( div ) ; 
}

Expression Sub::d(const Parameter& div )  { 
  return lval.d( div ) - rval.d( div ) ; 
}

Expression Product::d( const Parameter& div ){ 
  return lval.d(div)*rval + lval*rval.d(div) ; 
}

Expression Divide::d( const Parameter& div ){ 
  return lval.d(div)/rval - lval*rval.d(div) / (rval *rval ); 
}

Expression Sqrt::d ( const Parameter& div ){
  return m_expression.d(div)/(2*Sqrt(m_expression)); 
}

Expression Log::d( const Parameter& div ){ 
  return 1./m_expression; 
}

Expression Exp::d( const Parameter& div ){
  return Exp(m_expression)*m_expression.d(div); 
}

Expression Pow::d( const Parameter& div ){
  return Log( m_expression ) * Pow( m_expression, m_coefficient ) * m_coefficient.d(div) +
    Pow( m_expression, m_coefficient - 1)*m_coefficient*m_expression.d(div) ; 
}

Expression Abs::d( const Parameter& div){
  return Ternary( m_expression > Constant(0) , m_expression.d(div), - m_expression.d(div) );  
}


Expression Ternary::d( const Parameter& div ){
  return Ternary( m_cond , m_v1.d(div), m_v2.d(div) );
}

Expression Expression::operator-() const { 
  return Constant(-1.)*m_expression ; 
}

Expression Expression::operator+=(const Expression& other) const {
  return Sum(*this,other);
}

Expression Expression::operator*=(const Expression& other) const {
  return Product(*this,other);
}


Expression::Expression( const double& value ) : m_expression(std::make_shared<Constant>(value)){}
Expression::Expression() : m_expression( std::make_shared<Constant>(0.)) {}

Expression Complex::norm() const { return re*re + im*im ; }
Complex    Complex::conj() const { return Complex(re, -im ) ; }
 
void AmpGen::DEBUG_SYMBOLS( const std::string& name, const std::vector< DBSYMBOL>& expressions, std::ostream& stream ){
  stream << "extern \"C\" void " << name << "_DB( double* P, double* E){" << std::endl;

  for( auto& f : expressions ){
    std::string name = f.first;
    const auto expression= f.second;
    if( expression.to_string() == "NULL")
      stream << "std::cout << \"\\033[1m\" << \""<< name << "\" << \"\\033[0m\" << std::endl ;" << std::endl;
    else {
      name.resize(20,' ');
      stream << "std::cout << \"  \" << \""<< name << "\" << \"=  \"<< " << expression.to_string() 
        << "<< std::endl ;" << std::endl;
    }
  }
  stream << "}" << std::endl;
}

