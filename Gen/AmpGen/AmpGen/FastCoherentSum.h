#ifndef FASTCOHERENTSUM_H
#define FASTCOHERENTSUM_H 1 

/// AmpGen 
#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/MinuitParameter.h"
#include "AmpGen/ParticlePropertiesList.h"
#include "AmpGen/Minimiser.h"
#include "AmpGen/Particle.h"
#include "AmpGen/CompiledExpression.h"
#include "AmpGen/EventList.h"
#include "AmpGen/Utilities.h"
#include "AmpGen/EventType.h"
#include "AmpGen/CacheTransfer.h"
#include "AmpGen/FitFraction.h"

/// STL
#include <iomanip>
#include <chrono>
#include <ctime>

#ifdef __USE_OPENMP__
#include <omp.h>
#endif

#include <fstream>

namespace AmpGen { 

  class FastCoherentSum {
    protected:

      /* 
         all of these things are the same size
         we should see about doing this better,
         but it would probably screw up the cache alignment
         should also maybe partition some of the behaviour
         for fast incoherent sums, more general stuff blah blah
         */
      std::vector<std::shared_ptr<Particle>> m_decayTrees; //// decay tree storage ///
      std::vector<CompiledExpression<std::complex<double>>> m_pdfs;    
      std::vector<std::complex<double>> m_coefficients;
      std::vector<std::pair<AmpGen::MinuitParameter*,
        AmpGen::MinuitParameter*>> m_minuitParameters; //// minuit link ///
      std::vector<std::vector<std::complex<double>>> m_normalisations; //// bilinear normalisation terms ////
      double m_norm; 
      EventList* m_events;
      EventList* m_sim; 
      double m_weight; /// global weight, i.e. the yield ///
      std::vector<unsigned int> m_cacheAddresses; /// the addresses in the event cache for each PDF /// 
      std::vector<std::shared_ptr<CacheTransfer<std::complex<double>>>> m_addressMapping; /// complicated transfer logic //// 
      int m_prepareCalls;
      int m_lastPrint;
      double m_weightIntegral;
    protected:
      std::string m_prefix;
      bool m_stateIsGood;
    public:
      enum OPTIONS { 
        DEBUG        = (1<<0),
        CPCONJUGATE  = (1<<1),
        FLAVCONJUGATE = (1<<2)
      };
      std::vector<std::shared_ptr<Particle>> decayTrees() const 
      { return m_decayTrees ; }
      std::string getTexTitle( const unsigned int& i )
      { return m_decayTrees[i]->getTeX() ; }
      std::complex<double> norm ( const unsigned int& x, const unsigned int & y ) const 
      { return m_normalisations[x][y] ; }
      std::string uniqueString( const unsigned int& index ) const 
      { return m_decayTrees[index]->uniqueString(); }
      CompiledExpression<std::complex<double>>& pdf( const unsigned int& i ) 
      { return m_pdfs[i] ; } 
      std::string prefix() const 
      { return m_prefix ; } 
      inline unsigned int cacheAddress( unsigned int i ) const {
        return m_cacheAddresses[i];
      }
      inline std::complex<double> amplitude( unsigned int i ) const {
        return m_coefficients[i];
      }
      double getWeight() const { return m_weight; }
      void setWeight( const double& weight ){ m_weight = weight ; }
      unsigned int size() const { return m_decayTrees.size(); }

      void reset(bool resetEvents=false){
        m_prepareCalls=0;
        m_lastPrint=0;
        m_cacheAddresses.clear();
        if( resetEvents ){ 
          m_events = 0;
          m_sim = 0 ; 
        };
      }
      void setEvents( EventList& list ){
        DEBUG("Setting events to size = " << list.size() ); 
        reset();
        m_events=&(list); 
      }
      void setMC(EventList& sim ){
        reset();
        m_sim = &sim;
      };
      FastCoherentSum( const EventType& type , 
          AmpGen::MinuitParameterSet& mps,
          const std::map<std::string, unsigned int>& extendEventFormat = 
          std::map<std::string, unsigned int>(), 
          const std::string& prefix="",
          unsigned int options = 0 ) ;

      //// get the value of the PDF at this point //// 
      std::complex<double> getVal( const Event& evt ) const {
        std::complex<double> value(0.,0.);
        for( unsigned int i=0;i<m_coefficients.size();++i){
          value += m_coefficients[i]*evt.getCache(m_cacheAddresses[i]);
        }

        return value;
      }
      bool isStateGood(){ return m_stateIsGood ; } 
      double prob( const Event& evt ){
        return m_weight * std::norm( getVal(evt) ) / m_norm ; 
      }
      double norm() const {
        std::complex<double> norm(0,0);
        for( unsigned int i=0;i<m_minuitParameters.size();++i){
          for( unsigned int j=0;j<m_minuitParameters.size();++j){
            auto val = m_normalisations[i][j]*m_coefficients[i]*std::conj(m_coefficients[j]);
            norm += val ;
          }
        }
        return norm.real();
      }
      void transferParameters(){
        for( unsigned int i=0;i<m_minuitParameters.size();++i ){
          m_coefficients[i]=std::complex<double>( 
              m_minuitParameters[i].first->mean(), m_minuitParameters[i].second->mean() );
        }
      }
      void prepare() ;

      std::vector<CompiledExpression<std::complex<double>>>& getExpressions(){ 
        INFO("Returning " << m_pdfs.size() << " expressions for " << m_prefix << " pdf" ) ;
        return m_pdfs;}
      void setWeightIntegral( const EventList& evts ){
        m_weightIntegral=0;
        for( auto& evt : evts ) m_weightIntegral += evt.weight(0) / evt.genPdf(); 
      }
      void debug( const unsigned int& N=0, const std::string& nameMustContain="") ; 
      double weightIntegral(){ return m_weightIntegral ; }

      std::vector<FitFraction> fitFractions(AmpGen::Minimiser& minuit); 
      std::vector<FitFraction> fitFractions(const TMatrixD& covMatrix, const std::vector<MinuitParameter*>& minuitParameters); 

      void makeBinary( const std::string& fname , const double& normalisation=1) ; 
  }; 
} 

#endif
