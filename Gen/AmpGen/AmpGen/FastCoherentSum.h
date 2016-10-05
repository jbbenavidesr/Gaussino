#ifndef FASTCOHERENTSUM_H
#define FASTCOHERENTSUM_H 1 

/// AmpGen 
#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/MinuitParameter.h"
#include "AmpGen/ParticlePropertiesList.h"
#include "AmpGen/Minimiser.h"

/// CGEN 
#include "AmpGen/Particle.h"
#include "AmpGen/CompiledExpression.h"
#include "AmpGen/Observable.h"
#include "AmpGen/FitParameter.h"
#include "AmpGen/EventList.h"
#include "AmpGen/Utilities.h"
#include "AmpGen/EventType.h"
#include "AmpGen/CacheTransfer.h"

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

    /* all of these things are the same size --- 
       we should seee about doing this better, 
       but it would probably screw up the cache alignment 
       should also maybe partition some of the behaviour
       for fast incoherent sums, more general stuff blah blah
       */
    /*
       struct addressMapping {
       AmpGen::MinuitParameter* source;
       unsigned int pdf;
       unsigned int param;
       addressMapping( AmpGen::MinuitParameter* _source, unsigned int _pdf, unsigned int _param ) : source(_source), pdf(_pdf), param(_param) {};
       }; 
       */
    std::vector<std::shared_ptr<Particle>> m_decayTrees; //// decay tree storage ///
    std::vector<CompiledExpression<std::complex<double>>> m_pdfs;    
    std::vector<std::complex<double>> m_coefficients;
    std::vector<std::pair<AmpGen::MinuitParameter*,
      AmpGen::MinuitParameter*>> m_minuitparameters; //// minuit link ///
    std::vector<std::vector<std::complex<double>>> m_normalisations; //// bilinear normalisation terms ////
    double m_norm; 
    EventList* m_events;
    EventList* m_sim; 
    double m_weight; /// global weight, i.e. the yield ///
    std::vector<unsigned int> m_cacheAddresses; /// the addresses in the event cache for each PDF /// 
    std::vector<std::shared_ptr<CacheTransfer>> m_addressMapping; /// complicated transfer logic //// 
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
    std::string getTexTitle( const unsigned int& i , bool isRoot=false)
      { return m_decayTrees[i]->getTex(isRoot) ; }
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
    };
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
    std::complex<double> getVal( const Event& evt ){
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
      for( unsigned int i=0;i<m_minuitparameters.size();++i){
        for( unsigned int j=0;j<m_minuitparameters.size();++j){
          auto val = m_normalisations[i][j]*m_coefficients[i]*std::conj(m_coefficients[j]);
          norm += val ;
        }
      }
      return norm.real();
    }
    void transferParameters(){
      for( unsigned int i=0;i<m_minuitparameters.size();++i ){
        m_coefficients[i]=std::complex<double>( 
            m_minuitparameters[i].first->mean(), m_minuitparameters[i].second->mean() );
      }
    }
    void prepare(){
      m_prepareCalls++;
      transferParameters(); /// move everything to the "immediate" cache ///
      for( auto& addr : m_addressMapping ) addr->transfer();
      std::vector<unsigned int> changedPdfIndices;
      auto t_total = std::chrono::high_resolution_clock::now();
      for( unsigned int i = 0 ; i < m_pdfs.size(); ++i){
        auto& pdf = m_pdfs[i];
        if( pdf.hasExternalsChanged() || m_prepareCalls == 1 ){
          DEBUG("Calling prepare for the first time");
          auto t_start = std::chrono::high_resolution_clock::now();
          if( m_prepareCalls == 1 ){
            if( m_events != 0 ){
              DEBUG("Caching PDF for data");
              m_cacheAddresses.push_back( m_events->cacheIndex( pdf ) );
            }
            if( m_sim != 0 && m_events == 0 ){
               DEBUG("Caching PDF for MC (no data)");
               m_cacheAddresses.push_back( m_sim->cacheIndex( pdf ) );
            }
            if( m_sim != 0 && m_events != 0 ){
              DEBUG("Caching PDF for MC");
              m_sim->cacheIndex(pdf);
            }
          }
          else {
            m_events->updateCache( pdf, m_cacheAddresses[ i ] ); 
            if( m_sim != 0 ) m_sim->updateCache( pdf, m_cacheAddresses[i] );   
          }
          auto t_end = std::chrono::high_resolution_clock::now();
          double time = std::chrono::duration<double, std::milli>(t_end-t_start).count() ;
          if( m_prepareCalls > m_lastPrint + 100 || m_prepareCalls == 1 ){
            INFO(pdf.name() << " ( t = " << time << " ms, nCalls = " << m_prepareCalls << ")" );    
            m_lastPrint = m_prepareCalls;
          }
          changedPdfIndices.push_back( i ); 
          pdf.resetExternals();
        }
      }
      if( m_sim == 0 ) return; 
      auto t_start = std::chrono::high_resolution_clock::now();
      double iTime=0;

      unsigned int nIntegrals = 0 ;
      std::vector<std::vector<bool>> integralHasChanged( m_pdfs.size(), std::vector<bool>(m_pdfs.size(), 0 ));
      for( auto& i : changedPdfIndices ){
        for( unsigned int j = 0 ; j < m_pdfs.size(); ++j){
          if( j==i ){
            if( !integralHasChanged[i][j] ) {
              auto iStart = std::chrono::high_resolution_clock::now();
              m_normalisations[i][j] = m_sim->integrate( m_pdfs[i], m_pdfs[j], false );
              iTime += std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() -iStart).count() ; 
              nIntegrals++;
              integralHasChanged[i][j] = true;
            }
          }
          else if( !integralHasChanged[i][j] ){
            auto iStart = std::chrono::high_resolution_clock::now();
            m_normalisations[i][j] = m_sim->integrate( m_pdfs[i], m_pdfs[j] , false ) ;
            iTime += std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() -iStart).count() ; 
            m_normalisations[j][i] = std::conj( m_normalisations[i][j] );
            nIntegrals++;
            integralHasChanged[i][j] = true;
            integralHasChanged[j][i] = true;
          }
        }
      }
      if( changedPdfIndices.size() != 0 && m_prepareCalls == m_lastPrint ){
        auto t_end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(t_end-t_start).count() ;
        double total = std::chrono::duration<double, std::milli>(t_end-t_total).count() ;
        INFO( this << " prepare performance : nIntegrals = " << nIntegrals << ", time = " << time << " ms, total prepare time = " << total  << " integrals = " << iTime );
      }
      m_norm = norm(); /// update normalisation 
    }
    std::vector<CompiledExpression<std::complex<double>>>& getExpressions(){ 
      INFO("Returning " << m_pdfs.size() << " expressions for " << m_prefix << " pdf" ) ;
      return m_pdfs;}
    void setWeightIntegral( const EventList& evts ){
      m_weightIntegral=0;
      for( auto& evt : evts ) m_weightIntegral += evt.weight(0) / evt.genPdf(); 
    }
    void debug( const unsigned int& N=0, const std::string& nameMustContain=""){ 
      for( auto& pdf : m_pdfs ) pdf.resetExternals();
      if( nameMustContain == "" ) for( auto& pdf : m_pdfs ) pdf.debug( m_events->getEvent(N) );
      else 
        for( auto& pdf: m_pdfs ) if( pdf.name().find( nameMustContain ) != std::string::npos ) pdf.debug( m_events->getEvent(N) );
      prepare();
      INFO( "Pdf = " << prob( m_events->at(N) )); 
    }
    double weightIntegral(){ return m_weightIntegral ; }

    std::vector<std::string> fitFractions(AmpGen::Minimiser& minuit , std::ostream& stream){
      std::vector<Complex> co;
      std::vector<std::string> formatted;
      auto covMatrix = minuit.covMatrixFull();
      DEBUG("Covariance matrix size = " << covMatrix.GetNcols() << " x " << covMatrix.GetNrows()  );
      
      for( unsigned int i=0;i<m_minuitparameters.size();++i ) 
        co.push_back( Complex( AmpGen::FitParameter( m_minuitparameters[i].first) , 
              AmpGen::FitParameter( m_minuitparameters[i].second ) ) );
      
      DEBUG("Collected 2x" << m_minuitparameters.size() << " parameters" );
      std::vector<Parameter> params;
      for( auto& param : m_minuitparameters ){
        params.push_back( Parameter( param.first->name() ) );
        params.push_back( Parameter( param.second->name() ) );
      }
      DEBUG("Collected " << params.size() << 
          " named parameters ( normalisations = " << m_normalisations.size() );
      Expression normalisation; 
      Expression diagonalFitFraction; 
      for( unsigned int i=0;i<m_minuitparameters.size();++i){
        normalisation = normalisation + co[i].norm()*m_normalisations[i][i].real();
        diagonalFitFraction = diagonalFitFraction + co[i].norm()*m_normalisations[i][i].real();
        for( unsigned int j=i+1; j < m_minuitparameters.size(); ++j )
          normalisation = normalisation + 2*( co[i]*co[j].conj()*m_normalisations[i][j] ).real() ; 
      }
      DEBUG("Calculated normalisations");
      std::vector<Observable> fractions;
      std::vector<Observable> interferenceTerms;
      std::vector<std::string> latexTable;
      
      for( unsigned int i=0;i<m_minuitparameters.size();++i){

        Observable FF( co[i].norm()*m_normalisations[i][i].real() / normalisation ,
            m_decayTrees[i]->uniqueString() );
        FF.evaluate( covMatrix , params );
        fractions.push_back(FF);
        for( unsigned int j=i+1; j < m_minuitparameters.size(); ++j ){
          Complex fij = co[i]*co[j].conj()*m_normalisations[i][j] ;
          Observable IF( fij.real() / normalisation,
              m_decayTrees[i]->uniqueString() + " x " + m_decayTrees[j]->uniqueString() );
          IF.evaluate( covMatrix, params);
          interferenceTerms.push_back( IF);
        }   
        stream << m_decayTrees[i]->uniqueString() << " : " 
          << FF.getVal() << " " << FF.getError() << " " 
          << m_minuitparameters[i].first->mean() << " " 
          << m_minuitparameters[i].first->err() << " " 
          << m_minuitparameters[i].second->mean() << " " 
          << m_minuitparameters[i].second->err() << std::endl;
        std::string latexString = numberWithError(FF.getVal(),FF.getError(),4) + " & " +
                                numberWithError(m_minuitparameters[i].first->mean() , m_minuitparameters[i].first->err(),3) + " & " +
                                numberWithError(m_minuitparameters[i].second->mean() , m_minuitparameters[i].second->err(),3) ;  
        formatted.push_back( latexString );
      }
      Observable OFF( diagonalFitFraction / normalisation ,"SumOfFitFractions");
      OFF.evaluate( covMatrix, params ); 
      INFO( "Diagonal Fit Fraction = " << numberWithError( OFF.getVal() , OFF.getError(),4)  );
      formatted.push_back( numberWithError( OFF.getVal(), OFF.getError(),4) );
      DEBUG( fractions.size() << " fit fraction observables");
      DEBUG( interferenceTerms.size() << " interference observables");

      std::sort( fractions.begin(), fractions.end() );
      std::sort( interferenceTerms.begin(), interferenceTerms.end() );

      for( auto fraction = fractions.begin() ; fraction != fractions.end(); ++fraction )
        std::cout << std::setw(55) << fraction->name() << "   " 
          << std::setw(7)  << fraction->getVal() 
          << std::setw(7)  << " +/- " << fraction->getError() << std::endl;

      std::cout << "#################################################" << std::endl; 

      std::cout << "Re(Interference) (+/-) stat." << std::endl;
      for( auto& fraction : interferenceTerms )
        std::cout << std::setw(55) << fraction.name() << "   " 
          << std::setw(7)  << fraction.getVal() 
          << std::setw(7)  << " +/- " << fraction.getError() << std::endl;
      return formatted ; 
    }


  void makeBinary( const std::string& fname ){
    std::ofstream stream( fname );
    stream << "#include <complex>" << std::endl;
    stream << "#include <vector>" << std::endl; 
    for( auto& p : m_pdfs ) p.compile( stream );
    transferParameters();
    stream << std::setprecision(10) ; 
    for( auto& p : m_pdfs ) p.compileWithParameters( stream );
    
    stream << "extern \"C\" double FCN( double* E , const int& parity){" << std::endl;
    stream << " std::complex<double> amplitude = " << std::endl;
    for( unsigned int i = 0 ; i < m_pdfs.size() ; ++i ){
      int parity = m_decayTrees[i]->finalStateParity();
      //INFO( m_decayTrees[i]->uniqueString() << " parity = " << parity );
      if( parity == -1 ) stream << " double(parity) * ";
      stream << "std::complex<double>(" << std::real(m_coefficients[i]) << " , " << std::imag( m_coefficients[i] ) << ") * ";
      stream << "r" << m_pdfs[i].hash() << "( E )";
      stream << ( i==m_pdfs.size()-1 ? ";" : "+" ) << std::endl;  
    };
    stream << " return std::norm(amplitude) ; }" << std::endl; 
    stream.close();
  };
}; } 

#endif
