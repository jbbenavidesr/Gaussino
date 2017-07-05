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
#define DEFINE_CAST(X) X::operator Expression (){ return Expression( std::make_shared<X>(*this) ) ; }

#define DECLARE_UNARY_OPERATOR(X) struct X : public IUnaryExpression { \
    X( const Expression& other ) ; \
    virtual std::string to_string() const; \
    virtual Expression d( const Parameter& div ) ;\
    virtual std::complex<double> complexEval() const ;\
    virtual double realEval() const ;\
    operator Expression() ; \
    virtual Expression conjugate() const ;\
  };

#define DECLARE_BINARY_OPERATOR(X) struct X : public IBinaryExpression { \
    X( const Expression& l , const Expression& r ) ; \
    virtual std::string to_string() const; \
    virtual Expression d( const Parameter& div ) ;\
    virtual std::complex<double> complexEval() const ;\
    virtual double realEval() const ;\
    operator Expression() ; \
    virtual Expression conjugate() const ;\
  };


#include <iostream>
#include <memory>
#include <algorithm>
#include <sstream>
#include <complex>
#include <map>
#include <vector>

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
    virtual ~IExpression() {};
  };

  typedef std::pair < std::string, Expression > DBSYMBOL;

  void DEBUG_SYMBOLS( const std::string& name,
      const std::vector<DBSYMBOL>& expressions,
      std::ostream& stream ) ;


  struct Expression { /// effective type erasure structre
    std::shared_ptr<IExpression> m_expression;

    std::string to_string() const { 
      if( m_expression == 0 || get() == 0 ){
        ERROR("No expression contained in this node!");
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

  struct IBinaryExpression : public IExpression {
    IBinaryExpression( const Expression& l, const Expression& r ) : lval( l ),rval(r) {};
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      lval.resolveDependencies( dependencies );
      rval.resolveDependencies( dependencies );
    }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      lval.resolveEventMapping( evtMapping );
      rval.resolveEventMapping( evtMapping );
    }
    Expression lval;
    Expression rval;
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
    virtual std::complex<double> complexEval() const { return std::complex<double>(m_defaultValue,0) ; }
    virtual double realEval() const { return m_defaultValue; }

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

  DECLARE_UNARY_OPERATOR(Log)
  DECLARE_UNARY_OPERATOR(Exp)
  DECLARE_UNARY_OPERATOR(Sqrt)
  DECLARE_UNARY_OPERATOR(Abs)

  DECLARE_UNARY_OPERATOR(Sin)
  DECLARE_UNARY_OPERATOR(Cos)
  DECLARE_UNARY_OPERATOR(Tan)

  DECLARE_UNARY_OPERATOR(aSin)
  DECLARE_UNARY_OPERATOR(aCos)
  DECLARE_UNARY_OPERATOR(aTan)

  /// binary relations
  DECLARE_BINARY_OPERATOR(Sum)
  DECLARE_BINARY_OPERATOR(Sub)
  DECLARE_BINARY_OPERATOR(Product)
  DECLARE_BINARY_OPERATOR(Divide)
  DECLARE_BINARY_OPERATOR(LessThan)
  DECLARE_BINARY_OPERATOR(GreaterThan)
  DECLARE_BINARY_OPERATOR(And)
  DECLARE_BINARY_OPERATOR(Pow)

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

} 
/// prefixed binary operators


#endif
