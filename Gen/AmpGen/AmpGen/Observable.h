#include "TMatrixTSym.h"

#include "AmpGen/Expression.h"
#include "AmpGen/MsgService.h"

namespace AmpGen { 
  class Observable {
    private:
      Expression m_expression;
      std::string m_name;
      double m_value;

      double m_variance;
    public:
      Observable( const Expression& expression, const std::string& name ) : 
        m_expression(expression), m_name(name) , m_value(0) , m_variance(0) {}
      void evaluate( const TMatrixTSym<double>& covMatrix,
          const std::vector<Parameter>& params ){

        std::vector<double> gradient( params.size() );
        for( unsigned int i = 0 ; i < params.size() ; ++i )
          gradient[i] = m_expression.d( params[i] ).realEval();
        m_value = m_expression.realEval();
        m_variance = 0.;
        for( unsigned int i=0;i<params.size();++i){
          //std::cout << params[i].to_string() << std::endl;
          DEBUG( "gradient " << i << " =  " << gradient[i] );
          for( unsigned int j=0;j<params.size();++j){
            DEBUG( "cov_ " << i << ","<<j<< " = " << covMatrix(i,j) );
            m_variance += gradient[i]*covMatrix(i,j)*gradient[j];
          }
        } 
        //std::cout << m_variance << std::endl; 
      }
      double getVal() const { return m_value ;}
      double getError() const { return sqrt( fabs( m_variance )); } 
      std::string name() const { return m_name;}

      bool operator>( const Observable& other ) const { return fabs(getVal()) > fabs( other.getVal()) ; }
      bool operator<( const Observable& other ) const { return fabs(getVal()) < fabs( other.getVal()) ; }

  }; 
} 
