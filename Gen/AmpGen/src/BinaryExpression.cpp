#include "AmpGen/Expression.h"

using namespace AmpGen;

  DEFINE_CAST( Sum )
  DEFINE_CAST( Sub )
  DEFINE_CAST( Product )
  DEFINE_CAST( Divide )
  DEFINE_CAST( And )
  DEFINE_CAST( GreaterThan )
  DEFINE_CAST( LessThan )
  DEFINE_CAST( Pow )

Sum::Sum( const Expression& l, const Expression& r) : IBinaryExpression(l,r) {}
std::string Sum::to_string() const {
  const std::string& lstring = lval.to_string();
  const std::string& rstring = rval.to_string();
  return "(" + lstring + " + " + rstring + ")"; 
} 

std::complex<double> Sum::complexEval() const { return lval.complexEval() + rval.complexEval() ; }
double Sum::realEval() const { return lval.realEval() + rval.realEval() ; }
Expression Sum::conjugate() const { return lval.conjugate() + rval.conjugate() ; }

Sub::Sub( const Expression& l, const Expression& r) : IBinaryExpression(l,r) {}
std::string Sub::to_string() const {
  const std::string& lstring = lval.to_string();
  const std::string& rstring = rval.to_string();
  if( lstring == std::to_string(0) ) return "-" + rstring;
  else if( rstring == std::to_string(0) ) return lstring;
  else return "(" + lstring + " - " + rstring +")";
}
std::complex<double> Sub::complexEval() const { return lval.complexEval() - rval.complexEval() ; }
double Sub::realEval() const { return lval.realEval() - rval.realEval() ; }
Expression Sub::conjugate() const { return lval.conjugate() - rval.conjugate() ; }

Product::Product( const Expression& l, const Expression& r) : IBinaryExpression(l,r) {} 

std::string Product::to_string() const {
  const std::string& lstring = lval.to_string();
  const std::string& rstring = rval.to_string();
  if( lstring != std::to_string(0) && rstring != std::to_string(0) ){ 
    if( lstring == std::to_string(1) ) return rstring; 
    if( rstring == std::to_string(1) ) return lstring;
    return lstring + "*" + rstring ; 
  }
  else return std::to_string(0);
}
std::complex<double> Product::complexEval() const {
  return lval.complexEval() * rval.complexEval() ; }
  double Product::realEval() const { return lval.realEval() * rval.realEval() ; }
  Expression Product::conjugate() const { return lval.conjugate() * rval.conjugate() ; }

  Divide::Divide( const Expression& l, const Expression& r) : IBinaryExpression(l,r) {} 
  std::string Divide::to_string() const {
    const std::string& lv = lval.to_string();
    const std::string& rv = rval.to_string();

    if( lv == std::to_string(0) ) return lv;
    if( rv == std::to_string(0) ) return "NaN";
    if( rv == std::to_string(1) ) return lv;
    if( lv == rv ) return "1.";
    return "((" + lv + ")/(" + rv + "))";
  }
double Divide::realEval() const { 
  return lval.realEval() / rval.realEval() ; }
  std::complex<double> Divide::complexEval() const { 
    return lval.complexEval() / rval.complexEval() ; }
    Expression Divide::conjugate() const { return lval.conjugate() / rval.conjugate() ; }


    LessThan::LessThan( const Expression& lval, const Expression& rval ) : IBinaryExpression(lval,rval) {}

    std::string LessThan::to_string() const { 
      return "("+lval.to_string()+"<"+rval.to_string()+")";
    }

Expression LessThan::d( const Parameter& div){ return Expression( Constant(0) ); }

std::complex<double> LessThan::complexEval() const { return std::complex<double>() ; }
double    LessThan::realEval() const { return lval.realEval() < rval.realEval() ; }
Expression LessThan::conjugate() const { return Expression( std::make_shared<LessThan>(*this) ) ; } 

GreaterThan::GreaterThan( const Expression& lval, const Expression& rval ) : IBinaryExpression(lval,rval) {}
std::string GreaterThan::to_string() const {
  return "("+lval.to_string()+">"+rval.to_string()+")";
}
Expression GreaterThan::d( const Parameter& div){
  return Expression( Constant(0 ) );
}
std::complex<double> GreaterThan::complexEval() const { return std::complex<double>() ; }
double    GreaterThan::realEval() const { return lval.realEval() > rval.realEval() ; }
Expression GreaterThan::conjugate() const { return Expression(std::make_shared<GreaterThan>(*this) ); }

And::And( const Expression& l, const Expression& r) : IBinaryExpression(l,r) {} 

std::string And::to_string() const { 
  return "("+lval.to_string()+"&&"+rval.to_string()+")";
}
Expression And::d( const Parameter& div){
  return Expression( Constant(0 ) );
}
std::complex<double> And::complexEval() const { return std::complex<double>() ; }
double    And::realEval() const { return lval.realEval() && rval.realEval() ; }
Expression And::conjugate() const { return Expression( std::make_shared<And>(*this) ) ; } 

Pow::Pow( const Expression& l, const Expression& r ) : IBinaryExpression(l,r) {} 

std::string Pow::to_string() const 
{ 
  return "pow(" + lval.to_string() + ", " + rval.to_string() + ")"; 
}
Expression Pow::conjugate() const { return Pow( lval.conjugate(), rval.conjugate() ) ; }

std::complex<double> Pow::complexEval() const { 
  return pow( lval.complexEval() , rval.complexEval() ); 
}
double Pow::realEval() const { return pow( lval.realEval() , rval.realEval() ); }


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


Expression Pow::d( const Parameter& div ){
  return Log( lval ) * Pow( lval, rval ) * rval.d(div) +
    Pow( lval, rval - 1)*rval*lval.d(div) ; 
}
