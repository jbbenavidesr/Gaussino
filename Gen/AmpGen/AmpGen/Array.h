#include "AmpGen/Expression.h"

namespace AmpGen { 
  struct Array {

    unsigned int m_address;
    std::vector<Parameter> m_parameters; 
    void resolveEventMapping( const std::map< std::string, unsigned int>& evtMapping ){}

    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      auto it = dependencies.find( m_parameters.begin()->m_name );
      if( it == dependencies.end() ){
        m_address = dependencies.size(); // global offset ///
        for( auto& param : m_parameters ){
          param.m_address = dependencies.size();
          dependencies[ param.m_name ] = std::pair<unsigned int,double>( dependencies.size(), param.m_defaultValue );
        }
      }
      else {
        m_address = it->second.first;
        unsigned int j = m_address ; 
        for( auto& param : m_parameters ) param.m_address = j++;
      }
    }
    void push_back( const Parameter& param) { m_parameters.push_back(param) ; } 
    Expression operator []( const Expression& address ); 
  };

  struct ArrayItem : public IExpression {
    std::shared_ptr<Array> m_parent; 
    Expression m_addressOffset; 
    ArrayItem( const Array& parent, const Expression& address ) : 
      m_parent( std::make_shared<Array>( parent ) ),
      m_addressOffset( address ) 
    {}
    virtual std::string to_string() const {
      return "E["+std::to_string(m_parent->m_address)+"+(int)"+m_addressOffset.to_string()+"]";
    }
    virtual Expression d( const Parameter& div){ return Constant(0) ; } /// what would this even mean? 
    virtual std::complex<double> complexEval() const { return std::complex<double>() ; }
    virtual double realEval() const { return double(); }
    virtual void resolveEventMapping( const std::map< std::string, unsigned int>& evtMapping ){
      m_addressOffset.resolveEventMapping( evtMapping );
    }
    virtual void resolveDependencies( std::map < std::string, std::pair< unsigned int , double> >& dependencies ) {
      m_addressOffset.resolveDependencies( dependencies );
      m_parent->resolveDependencies( dependencies );
    }
    operator Expression(){ return Expression( std::make_shared<ArrayItem>(*this) ); }
    virtual Expression conjugate() const { return Expression( std::make_shared<ArrayItem>(*this) ); }
  };  
}

AmpGen::Expression AmpGen::Array::operator[]( const AmpGen::Expression& expression ){
  return AmpGen::Expression( AmpGen::ArrayItem( *this, expression ) );
}


