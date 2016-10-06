#ifndef FITPARAMETER_H
#define FITPARAMETER_H
#include "AmpGen/Expression.h"
#include "AmpGen/MinuitParameter.h"

namespace AmpGen { 
  struct FitParameter : public IExpression { /// Fit Parameter interface /// 

    FitParameter( AmpGen::MinuitParameter* parameter ) : m_parameter(parameter ) {};
    virtual std::string to_string() const { return "ERROR"; }
    virtual Expression d( const Parameter& div )
    { return div.m_name == m_parameter->name() ? Expression(Constant(1)) : Expression(Constant(0)) ; }
    virtual double realEval() const { return m_parameter->mean(); }
    virtual std::complex<double> complexEval() const { return std::complex<double>( realEval(), 0 ) ; }
    AmpGen::MinuitParameter* m_parameter;
    virtual void resolveDependencies( std::map < std::string, std::pair<unsigned int,double> >& dependencies ){
    }
    operator Expression(){ return Expression( std::make_shared<FitParameter>(*this) ); }
    virtual void resolveEventMapping( const std::map < std::string, unsigned int>& evtMapping ){}
    virtual Expression conjugate() const { return Expression( std::make_shared<FitParameter>(*this) ); }

  }; 
}

#endif
