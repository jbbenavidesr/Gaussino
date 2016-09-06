/** @class IExpression Expression.h AmpGen/Expression.h
  Virtual base class from which the other expressions 
  inherit. 
  @author Tim Evans
  @date 10/10/2015
  */

/*
   Expression engine for fast physics fitting.

   IExpression - Virtual base class from which the other expressions 
   inherit. All operators must implement the following :

   - std::string to_string() 
   Turns the expression tree into a string

   - Expression d( const Parameter* div ) 
   Partial derivative of the expression w.r.t div.

   - std::complex<double> complexEval()
   Evaluate the expression tree at run time and return 
   a complex value.

   - double realEval()
   Evaluate the expression tree at run time and return 
   a double.

   - void resolveDependencies(std::map < std::string, Expression >& dependencies)
   Passes the daughter nodes of a given node up the tree, in order to 
   resolve dependencies for external parameters. 

   Expression  - A wrapper class for a smart pointer to an IExpression, 
   defines the operator overloading that give the primative
   functions their behaviour.

   T.Evans 10/15


 ***********************************************************************/

#ifndef EXPRESSION_H 
#define EXPRESSION_H 1

#define add_debug(X,Y) Y->push_back( DBSYMBOL( std::string(#X), X  ) ) 

#include <iostream>
#include <memory>
#include <algorithm>
#include <sstream>
#include <complex>
#include <map>

#include "AmpGen/MsgService.h"
/// Forward definitions of the expressions

/// The basic binary operators ///

//// The smart-ptr wrapper class 
namespace AmpGen { 
  class Expression;
  class Parameter;

  struct IExpression {
    virtual std::string to_string() const = 0 ; 
    virtual Expression d( const Parameter& div ) = 0;
    virtual std::complex<double> complexEval() const = 0 ;
    virtual double realEval() const = 0;

    void compile( std::ostream& stream, 
        const std::string& fcn_name , 
        const std::string& return_type="double",
        const std::string& type="s") const {
      if( type == "d" ) //// flag to enable dynmamic linking ////
        stream << "extern \"C\" ";
      stream 
        << return_type + "   " << fcn_name 
        << "( double *__restrict__ P, double *__restrict E ) { return " 
        << to_string() << ";}" << std::endl; 
    }

    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) = 0 ;
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping )=0;
    virtual Expression conjugate() const = 0 ;
  };

  typedef std::pair < std::string, Expression > DBSYMBOL;

  void DEBUG_SYMBOLS( const std::string& name,
      const std::vector<DBSYMBOL>& expressions,
      std::ostream& stream ) ;


  struct Expression { /// effective type erasure structre /// (ETES)
    std::shared_ptr<IExpression> m_expression;

    std::string to_string() const { 
      if( m_expression == 0 || get() == 0 ){
        ERROR("WHAT THE SHIT LANA?");
      };
      return m_expression->to_string() ; }
    IExpression* get() const { return m_expression.get() ; }
    Expression d( const Parameter& div ){ 
      return Expression( m_expression->d( div ) ) ; }

    Expression( const std::shared_ptr<IExpression>& expression ) 
      : m_expression(expression ) {}
    Expression( const double& value ) ; 
    Expression() ;
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      m_expression->resolveDependencies( dependencies );
    }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      m_expression->resolveEventMapping( evtMapping );
    }

    std::complex<double> complexEval() const { return m_expression->complexEval() ; }
    double realEval() const { return m_expression->realEval(); }
    void compile( std::ostream& stream,
        const std::string& fcn_name ,
        const std::string& return_type="double",
        const std::string& type="s" ) const { 
      return m_expression->compile( stream, fcn_name, return_type, type ); }
    virtual Expression conjugate() const { return m_expression->conjugate() ; }  
    Expression operator+=( const Expression& other ) const;
    Expression operator*=( const Expression& other ) const;
    Expression operator-() const ;   
  };



  struct Complex : public IExpression { 

    Complex( const Expression& _re, const Expression& _im) : re( _re ), im(_im ) {}
    Complex( const double& _re, const double& _im ) : re( _re ), im(_im ) {} 
    Complex( const std::complex<double>& value ) ;
    virtual Expression d( const Parameter& div){
      return Expression( Complex( re.d( div ), im.d(div) ) );
    }
    virtual std::string to_string() const { 
      return " std::complex<double>( "+re.to_string() +","+im.to_string() +")";}
    virtual std::complex<double> complexEval() const { 
      return std::complex<double>( re.realEval(), im.realEval() ) ; }
    virtual double realEval() const { return double(); }

    //// complex specific operators ////
    Expression norm() const ; 
    Complex conj() const ;
    operator Expression(){ return Expression( std::make_shared<Complex>(*this) ); }
    Expression re;
    Expression im;
    Expression real() const { return re; }
    Expression imag() const { return im; }
    virtual Expression conjugate() const { 
      return Complex( re.conjugate(), -im.conjugate() );
    }; 
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      re.resolveDependencies( dependencies );
      im.resolveDependencies( dependencies );
    };
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      re.resolveEventMapping( evtMapping );
      im.resolveEventMapping( evtMapping );
    }


  } ; //// complex number definition////

  Expression operator<(const Expression& A, const Expression& B);
  Expression operator>(const Expression& A, const Expression& B);

  Expression operator+( const Expression& A, const Expression& B);
  Expression operator-( const Expression& A, const Expression& B);
  Expression operator*( const Expression& A, const Expression& B);
  Expression operator/( const Expression& A, const Expression& B);

  Expression operator+( const Expression& A, const double& B);
  Expression operator-( const Expression& A, const double& B);
  Expression operator*( const Expression& A, const double& B);
  Expression operator/( const Expression& A, const double& B);

  Complex operator*( const Complex& A, const Complex& B);
  Complex operator+( const Complex& A, const Complex& B);

  Expression operator+( const double& A, const Expression& B);
  Expression operator-( const double& A, const Expression& B);
  Expression operator*( const double& A, const Expression& B);
  Expression operator/( const double& A, const Expression& B);

  Expression operator&&(const Expression& A, const Expression& B);



  struct Constant : public IExpression {
    Constant( const double& value ) : m_value(value) {}
    virtual std::string to_string() const {   
      return m_value != 0 ? std::to_string( m_value ) : std::to_string(0);
    }
    virtual Expression d( const Parameter& /*div*/ ) ;
    virtual std::complex<double> complexEval() const { 
      return std::complex<double>(m_value, 0.) ; }
    virtual double realEval() const { return m_value; }
    double m_value;
    virtual Expression conjugate() const { return Expression( std::make_shared<Constant>(*this) ) ; } 
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {};
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){};

    operator Expression(){ return Expression( std::make_shared<Constant>(*this) ); }

  };

  struct Parameter : public IExpression {
    Parameter( const std::string& name, double defaultValue=0, bool resolved = false, bool isEventProperty=false) : 
      m_name(name), 
      m_resolved(resolved), 
      m_isEventProperty(isEventProperty),
      m_address(9999), 
      m_defaultValue(defaultValue) {}   
    std::string  m_name;   
    bool         m_resolved; 
    bool         m_isEventProperty;
    unsigned int m_address;
    double       m_defaultValue;
    virtual Expression conjugate() const { return Expression( std::make_shared<Parameter>(*this) ) ; }
    virtual std::string to_string() const { 
      if( m_resolved ) return m_name;
      else if( m_address != 9999  ) 
        return (m_isEventProperty?"P[":"E[") +std::to_string(m_address) +"]";
      else {
        WARNING( "Returning default value for " << m_name << " = " << m_defaultValue );
        return std::to_string( m_defaultValue );
      }
    } 
    virtual Expression d(const Parameter& div)  ; 
    virtual std::complex<double> complexEval() const { return std::complex<double>() ; }
    virtual double realEval() const { return double(); }

    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      if( ! m_resolved &&  !m_isEventProperty ){
        auto it = dependencies.find( m_name );
        if( it == dependencies.end() ){
          m_address = dependencies.size();
          dependencies[ m_name ] = std::pair<unsigned int, double>( m_address, m_defaultValue);
        }
        else m_address = it->second.first;
      }
    }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      if( !m_isEventProperty || m_resolved ) return;

      auto res = evtMapping.find( m_name );
      if( res != evtMapping.end() ){
        //  DEBUG(" Resolving " << m_name << " to " << res->second );
        m_address = res->second;
      }
      else { 
        ERROR( "Event parameter : " << m_name << " not defined in event format");
      }
    }
    operator Expression(){ return Expression( std::make_shared<Parameter>(*this) ); }
  } ; 

  struct Pow : public IExpression { 
    Pow( const Expression& other, const Expression& co ) : 
      m_expression(other), m_coefficient(co) {} ;
    Pow( const Expression& other, const double& n) : 
      m_expression( other ) , m_coefficient( Expression(Constant(n) ) ) {};
    virtual std::string to_string() const { 
      return "pow(" + m_expression.to_string() + ", " 
        + m_coefficient.to_string() + ")"; }
    virtual Expression d( const Parameter& div ) ;
    virtual Expression conjugate() const { return Pow( m_expression.conjugate(), m_coefficient.conjugate() ) ; }

    virtual std::complex<double> complexEval() const { 
      return pow( m_expression.complexEval() , m_coefficient.complexEval() ); }
    virtual double realEval() const { 
      return pow(m_expression.realEval() , m_coefficient.realEval() ); }
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      m_expression.resolveDependencies( dependencies );
      m_coefficient.resolveDependencies( dependencies );
    }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      m_expression.resolveEventMapping( evtMapping );
      m_coefficient.resolveEventMapping( evtMapping );
    }
    operator Expression(){ return Expression( std::make_shared<Pow>(*this) ); }

    Expression m_expression;
    Expression m_coefficient;
  };

  struct IUnaryExpression : public IExpression {
    IUnaryExpression( const Expression& other ) : m_expression( other ) {};
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      m_expression.resolveDependencies( dependencies );
    }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      m_expression.resolveEventMapping( evtMapping );
    }

    Expression m_expression;
  };

  struct Sqrt : public IUnaryExpression { 
    Sqrt( const Expression& other ) :  IUnaryExpression(other)  {} ;
    virtual std::string to_string() const { 
      return "sqrt(" + m_expression.to_string() +")";}
    virtual Expression d ( const Parameter& div ) ;
    virtual std::complex<double> complexEval() const { return sqrt( m_expression.complexEval() ); }
    virtual double realEval() const { return sqrt(m_expression.realEval() ); }
    operator Expression(){ return Expression( std::make_shared<Sqrt>(*this) ); }
    virtual Expression conjugate() const { return Sqrt(m_expression.conjugate()) ; }

  };


  struct Exp : public IUnaryExpression {
    Exp( const Expression& other ) : IUnaryExpression(other) {};
    virtual std::string to_string() const {
      return "exp(" + m_expression.to_string() +")";
    }
    virtual Expression d( const Parameter& div ) ;  
    virtual std::complex<double> complexEval() const { return exp( m_expression.complexEval() ); }
    virtual double realEval() const { return exp( m_expression.realEval() ); }
    operator Expression(){ return Expression( std::make_shared<Exp>(*this) ); }
    virtual Expression conjugate() const { return Exp(m_expression.conjugate()) ; }


  };

  struct Log : public IUnaryExpression {
    Log( const Expression& other ) : IUnaryExpression( other ) { };
    virtual std::string to_string() const {
      return "log(" + m_expression.to_string() +")";
    }
    virtual Expression d( const Parameter& div ) ;
    virtual std::complex<double> complexEval() const { 
      return log( m_expression.complexEval() ); }
    virtual double realEval() const { 
      return log( m_expression.realEval() ); }
    operator Expression(){ return Expression( std::make_shared<Log>(*this) ) ; } 
    virtual Expression conjugate() const { return Log(m_expression.conjugate()) ; }

  };

  struct Abs : public IUnaryExpression {
    Abs( const Expression& expression ) : IUnaryExpression(expression) {};
    virtual std::string to_string() const {
      return "fabs("+m_expression.to_string() +")";
    }
    virtual Expression d( const Parameter& div ) ;

    double realEval() const {
      return abs(m_expression.realEval()); }
    virtual std::complex<double> complexEval() const {
      return m_expression.complexEval(); }
    operator Expression(){ return Expression( std::make_shared<Abs>(*this) ); }
    virtual Expression conjugate() const { return Expression( std::make_shared<Abs>(*this) )  ; }

  };

  /// binary relations
  struct Sum : public IExpression { 

    Sum( const Expression& l, const Expression& r) : lval(l), rval(r) {}

    virtual std::string to_string() const {
      const std::string& lstring = lval.to_string();
      const std::string& rstring = rval.to_string();
      return "(" + lstring + " + " + rstring + ")"; 
    } 
    virtual Expression d(const Parameter& div );
    virtual std::complex<double> complexEval() const {
      return lval.complexEval() + rval.complexEval() ; }
    virtual double realEval() const {
      return lval.realEval() + rval.realEval() ; }
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int, double> >& dependencies ) {
      lval.resolveDependencies( dependencies );
      rval.resolveDependencies( dependencies );
    }
    operator Expression(){ return Expression( std::make_shared<Sum>(*this) ); }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      lval.resolveEventMapping( evtMapping );
      rval.resolveEventMapping( evtMapping );
    }
    virtual Expression conjugate() const { return lval.conjugate() + rval.conjugate() ; }

    Expression lval;
    Expression rval;
  };


  struct Sub : public IExpression {

    Sub( const Expression& l, const Expression& r) : lval(l), rval(r) {}
    virtual std::string to_string() const {
      const std::string& lstring = lval.to_string();
      const std::string& rstring = rval.to_string();
      if( lstring == std::to_string(0) ) return "-" + rstring;
      else if( rstring == std::to_string(0) ) return lstring;
      else return "(" + lstring + " - " + rstring +")";
    }
    virtual Expression d(const Parameter& div );
    virtual std::complex<double> complexEval() const {
      return lval.complexEval() - rval.complexEval() ; }
    double realEval() const {
      return lval.realEval() - rval.realEval() ; }
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      lval.resolveDependencies( dependencies );
      rval.resolveDependencies( dependencies );
    }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      lval.resolveEventMapping( evtMapping );
      rval.resolveEventMapping( evtMapping );
    }
    virtual Expression conjugate() const { return lval.conjugate() - rval.conjugate() ; }

    operator Expression(){ return Expression( std::make_shared<Sub>(*this) ); }

    Expression lval;
    Expression rval;

  };


  struct Product : public IExpression {

    Product( const Expression& l, const Expression& r) : lval(l), rval(r) {} 

    virtual std::string to_string() const {
      const std::string& lstring = lval.to_string();
      const std::string& rstring = rval.to_string();
      if( lstring != std::to_string(0) && rstring != std::to_string(0) ){ 
        if( lstring == std::to_string(1) ) return rstring; 
        if( rstring == std::to_string(1) ) return lstring;
        return lstring + "*" + rstring ; 
      }
      else return std::to_string(0);
    }
    virtual Expression d (const Parameter& div );
    virtual std::complex<double> complexEval() const {
      return lval.complexEval() * rval.complexEval() ; }
    double realEval() const { return lval.realEval() * rval.realEval() ; }
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {

      lval.resolveDependencies( dependencies );
      rval.resolveDependencies( dependencies );
    }
    operator Expression(){ return Expression( std::make_shared<Product>(*this) ); }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      lval.resolveEventMapping( evtMapping );
      rval.resolveEventMapping( evtMapping );
    }
    virtual Expression conjugate() const { return lval.conjugate() * rval.conjugate() ; }

    Expression lval;
    Expression rval; 
  } ; 


  struct Divide : public IExpression {
    Divide( const Expression& a, const Expression& b ) : lval(a), rval(b) {}
    virtual std::string to_string() const {
      const std::string& lv = lval.to_string();
      const std::string& rv = rval.to_string();

      if( lv == std::to_string(0) ) return lv;
      if( rv == std::to_string(0) ) return "NaN";
      if( rv == std::to_string(1) ) return lv;
      if( lv == rv ) return "1.";
      return "((" + lv + ")/(" + rv + "))";
    }
    virtual Expression d( const Parameter& div );
    double realEval() const { 
      return lval.realEval() / rval.realEval() ; }
    virtual std::complex<double> complexEval() const { 
      return lval.complexEval() / rval.complexEval() ; }
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      lval.resolveDependencies( dependencies );
      rval.resolveDependencies( dependencies );
    }         
    operator Expression(){ return Expression( std::make_shared<Divide>(*this) ); }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      lval.resolveEventMapping( evtMapping );
      rval.resolveEventMapping( evtMapping );
    }
    virtual Expression conjugate() const { return lval.conjugate() / rval.conjugate() ; }

    Expression lval;
    Expression rval;

  };

  struct Ternary : public IExpression {
    Ternary( const Expression& cond,
        const Expression& v1,
        const Expression& v2 ) : m_cond(cond), m_v1(v1), m_v2(v2) {}
    virtual std::string to_string() const {
      return "(" + m_cond.to_string() +"?"+m_v1.to_string()+":"+m_v2.to_string()+")";
    }
    virtual Expression d( const Parameter& div) ;

    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {

      m_cond.resolveDependencies( dependencies );
      m_v1.resolveDependencies( dependencies );
      m_v2.resolveDependencies( dependencies );

    }         
    Expression m_cond;
    Expression m_v1;
    Expression m_v2;
    virtual double realEval() const { 
      return m_cond.realEval() ? m_v1.realEval() : m_v2.realEval() ; }
    virtual std::complex<double> complexEval() const { 
      return m_cond.realEval() ? m_v1.complexEval() : m_v2.complexEval() ; }
    operator Expression(){ return Expression( std::make_shared<Ternary>(*this) ); }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      m_cond.resolveEventMapping( evtMapping );
      m_v1.resolveEventMapping( evtMapping );
      m_v2.resolveEventMapping( evtMapping );
    }
    virtual Expression conjugate() const { return Ternary( m_cond, m_v1.conjugate(), m_v2.conjugate() ) ; }
  };

  struct LessThan : public IExpression {
    LessThan( const Expression& lval, const Expression& rval ) : m_lval(lval), m_rval(rval) {}

    virtual std::string to_string() const { 
      return "("+m_lval.to_string()+"<"+m_rval.to_string()+")";
    }

    virtual Expression d( const Parameter& div){ return Expression( Constant(0) ); }
    Expression m_lval;
    Expression m_rval;
    virtual std::complex<double> complexEval() const { return std::complex<double>() ; }
    virtual double    realEval() const { return m_lval.realEval() < m_rval.realEval() ; }
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      m_lval.resolveDependencies( dependencies );
      m_rval.resolveDependencies( dependencies );
    }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      m_lval.resolveEventMapping( evtMapping );
      m_rval.resolveEventMapping( evtMapping );
    }

    operator Expression(){ return Expression( std::make_shared<LessThan>(*this) ); }
    virtual Expression conjugate() const { return Expression( std::make_shared<LessThan>(*this) ) ; } 

  };

  struct GreaterThan : public IExpression {
    GreaterThan( const Expression& lval, const Expression& rval ) : m_lval(lval), m_rval(rval) {}
    virtual std::string to_string() const {
      return "("+m_lval.to_string()+">"+m_rval.to_string()+")";
    }
    virtual Expression d( const Parameter& div){
      return Expression( Constant(0 ) );
    }
    Expression m_lval;
    Expression m_rval;
    virtual std::complex<double> complexEval() const { return std::complex<double>() ; }
    virtual               double    realEval() const { return m_lval.realEval() > m_rval.realEval() ; }

    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {

      m_lval.resolveDependencies( dependencies );
      m_rval.resolveDependencies( dependencies );
    }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      m_lval.resolveEventMapping( evtMapping );
      m_rval.resolveEventMapping( evtMapping );
    }
    operator Expression(){ return Expression( std::make_shared<GreaterThan>(*this )); }

    virtual Expression conjugate() const { return Expression(std::make_shared<GreaterThan>(*this) ); }
  };

  struct And : public IExpression {
    And( const Expression& lval, const Expression& rval) : m_lval(lval), m_rval(rval) {} ;

    Expression m_lval;
    Expression m_rval;
    virtual std::string to_string() const { 
      return "("+m_lval.to_string()+"&&"+m_rval.to_string()+")";
    }
    virtual Expression d( const Parameter& div){
      return Expression( Constant(0 ) );
    }
    virtual std::complex<double> complexEval() const { return std::complex<double>() ; }
    virtual               double    realEval() const { return m_lval.realEval() && m_rval.realEval() ; }
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {

      m_lval.resolveDependencies( dependencies );
      m_rval.resolveDependencies( dependencies );
    }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      m_lval.resolveEventMapping( evtMapping );
      m_rval.resolveEventMapping( evtMapping );
    }
    virtual Expression conjugate() const { return Expression( std::make_shared<And>(*this) ) ; } 

    operator Expression(){ return Expression( std::make_shared<And>(*this) ); }


  };
} 
/// prefixed binary operators


#endif
