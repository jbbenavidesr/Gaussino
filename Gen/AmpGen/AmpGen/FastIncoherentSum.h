#ifndef FASTINCOHERENTSUM_H
#define FASTINCOHERENTSUM_H 1 

/// AmpGen 
#include "AmpGen/FastCoherentSum.h"
/// STL
#include <iomanip>

namespace AmpGen { 
  class FastIncoherentSum : public FastCoherentSum {

    public:
      FastIncoherentSum( const EventType& finalStates , 
          AmpGen::MinuitParameterSet& mps,
          const std::map<std::string, unsigned int>& extendEventFormat = 
          std::map<std::string, unsigned int>(),
          const std::string& prefix="Inco",                                         
          unsigned int options =0 )  ;

      double getVal( const Event& evt ){
        double value(0.);
        DEBUG("Summing " << m_coefficients.size() << " things" );
        for( unsigned int i=0;i<m_coefficients.size();++i){

          value += std::norm( m_coefficients[i]* evt.getCache(m_cacheAddresses[i]) );
          DEBUG("Value = " << value << " " << evt.getCache(m_cacheAddresses[i]) );
        }
        return value;
      }
      double prob( const Event& evt ){
        DEBUG( "global weight = " << m_weight << ", pdf value = " << getVal(evt) <<", norm = " << m_norm );
        return m_weight * getVal(evt) / m_norm ;
      }
      void prepare() ; 
      double norm() const ; 
      //// warning - these functions are not properly tested /// 
      std::vector<FitFraction> fitFractions(AmpGen::Minimiser& minuit ) ;
     std::vector<FitFraction> fitFractions(
    const TMatrixD& covMatrix, const std::vector<MinuitParameter*>& minuitParameters );
  };
}

#endif
