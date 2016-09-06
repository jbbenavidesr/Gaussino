#include "AmpGen/Expression.h"

namespace AmpGen { 
  struct Sin : public IUnaryExpression {
    Sin( const Expression& expression ) : IUnaryExpression(expression){};
    virtual std::string to_string() const { return "sin("+m_expression.to_string()+")" ; };
    virtual Expression d( const Parameter& div ) ;
    virtual double realEval() const { return sin( m_expression.realEval() ); }
    virtual std::complex<double> complexEval() const { return sin( m_expression.complexEval() ) ; }
    operator Expression(){ return Expression( std::make_shared<Sin>(*this) ); }
    virtual Expression conjugate() const { return Sin( m_expression.conjugate() ) ; }
  };


  struct Cos : public IUnaryExpression {
    Cos( const Expression& expression ) : IUnaryExpression(expression){};
    virtual std::string to_string() const { return "cos("+m_expression.to_string()+")"; } ;
    virtual Expression d( const Parameter& div ) ;
    virtual double realEval() const { return cos( m_expression.realEval() ); }
    virtual std::complex<double> complexEval() const { return cos( m_expression.complexEval() ) ; }
    operator Expression(){ return Expression( std::make_shared<Cos>(*this) ); }
    virtual Expression conjugate() const { return Cos( m_expression.conjugate() ) ; }

  };

  struct Tan : public IUnaryExpression {
    Tan( const Expression& expression ) : IUnaryExpression(expression){};
    virtual std::string to_string() const { return "tan("+m_expression.to_string()+")"; } ;
    virtual Expression d( const Parameter& div ) ;
    virtual double realEval() const { return tan( m_expression.realEval() ); }
    virtual std::complex<double> complexEval() const { return tan( m_expression.complexEval() ) ; }
    operator Expression(){ return Expression( std::make_shared<Tan>(*this) ); }
    virtual Expression conjugate() const { return Tan( m_expression.conjugate() ) ; }

  };

  struct aSin : public IUnaryExpression {
    aSin( const Expression& expression ) : IUnaryExpression(expression){};
    virtual std::string to_string() const { return "asin("+m_expression.to_string()+")" ; };
    virtual Expression d( const Parameter& div ) ;
    virtual double realEval() const { return asin( m_expression.realEval() ); }
    virtual std::complex<double> complexEval() const { return asin( m_expression.complexEval() ) ; }
    operator Expression(){ return Expression( std::make_shared<aSin>(*this) ); }
    virtual Expression conjugate() const { return aSin( m_expression.conjugate() ) ; }

  };

  struct aCos : public IUnaryExpression {
    aCos( const Expression& expression ) : IUnaryExpression(expression){};
    virtual std::string to_string() const { return "acos("+m_expression.to_string()+")"; } ;
    virtual Expression d( const Parameter& div ) ;
    virtual double realEval() const { return acos( m_expression.realEval() ); }
    virtual std::complex<double> complexEval() const { return acos( m_expression.complexEval() ) ; }
    operator Expression(){ return Expression( std::make_shared<aCos>(*this) ); }
    virtual Expression conjugate() const { return aCos( m_expression.conjugate() ) ; }

  };

  struct aTan : public IUnaryExpression {
    aTan( const Expression& expression ) : IUnaryExpression(expression){};
    virtual std::string to_string() const { return "atan("+m_expression.to_string()+")"; } ;
    virtual Expression d( const Parameter& div ) ;
    virtual double realEval() const { return atan( m_expression.realEval() ); }
    virtual std::complex<double> complexEval() const { return atan( m_expression.complexEval() ) ; }
    operator Expression(){ return Expression( std::make_shared<aTan>(*this) ); }
    virtual Expression conjugate() const { return aTan( m_expression.conjugate() ) ; }

  };
}


