
#include "Expression.h"


//// special unary functions ////

/*
   struct Exp : public IExpression {
   Exp( const Expression& other ) : m_expression( other ) {};
   virtual std::string to_string() const {
   return "exp(" + m_expression.to_string() +")";
   }
   virtual Expression d( const Parameter& div ) ;
   virtual std::complex<double> complexEval() const { return exp( m_expression.complexEval() ); }
   virtual double realEval() const { return exp( m_expression.realEval() ); }
   virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {

   m_expression.resolveDependencies( dependencies );
   }
   operator Expression(){ return Expression( std::make_shared<Exp>(*this) ); }
   virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
   m_expression.resolveEventMapping( evtMapping );
   }

   Expression m_expression;
   };
   */
namespace AmpGen { 
  struct Fmod : public IExpression {
    Expression A;
    Expression B;
    virtual std::string to_string() const {
      return "fmod("+A.to_string() +","+B.to_string() +")";
    }
    Fmod( const Expression& vA, const Expression& vB) : A(vA), B(vB) {} 
    virtual Expression d( const Parameter& div) { return A.d( div ) ; }
    virtual std::complex<double> complexEval() const { return std::complex<double>(0,0) ; }
    virtual double realEval() const { return fmod( A.realEval(), B.realEval() ) ; }
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      A.resolveDependencies( dependencies );
    }
    operator Expression(){ return Expression( std::make_shared<Fmod>(*this) ); }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){
      A.resolveEventMapping( evtMapping );
      B.resolveEventMapping( evtMapping );
    }
    virtual Expression conjugate() const { return Expression( std::make_shared<Fmod>(*this) ) ; }
  }; 
} 

