#include "AmpGen/FastIncoherentSum.h"
#include "AmpGen/LatexTable.h"

using namespace AmpGen;


FastIncoherentSum::FastIncoherentSum( const EventType& finalStates , 
    AmpGen::MinuitParameterSet& mps,
    const std::map<std::string, unsigned int>& extendEventFormat ,
    const std::string& prefix,                                         
    unsigned int options) : 
  FastCoherentSum(finalStates,mps,extendEventFormat, prefix, options ) {

    INFO( "Have : "  << m_pdfs.size() << " pdfs");
  }

double FastIncoherentSum::norm() const {
  double norm(0) ; // (0,0);
  for( unsigned int i=0;i<m_minuitParameters.size();++i){
    double val = m_normalisations[0][i].real()*std::norm( m_coefficients[i] );
    norm += val ;
  }
  return norm; //.real();
}

void FastIncoherentSum::prepare(){

  m_prepareCalls++;
  transferParameters(); /// move everything to the "immediate" cache ///
  for( auto& addr : m_addressMapping ) addr->transfer();
  for( unsigned int i = 0 ; i < m_pdfs.size(); ++i){
    auto& pdf = m_pdfs[i];
    if( pdf.hasExternalsChanged() || m_prepareCalls == 1 ){
      if( m_prepareCalls == 1 ){
        int addr = -1; 
        if( m_events != 0 ) addr =  m_events->cacheIndex( pdf ) ;
        if( m_sim    != 0 ) addr =  m_sim->cacheIndex( pdf ) ; 
        if( addr != - 1 ) m_cacheAddresses.push_back(addr);
        else ERROR("No data to evaluate FCNs"); 
      }
      else {
        m_events->updateCache( pdf, m_cacheAddresses[ i ] );
        if( m_sim != 0 ) m_sim->updateCache( pdf, m_cacheAddresses[i] );
      }     
      m_normalisations[0][i] = m_sim->integrate( m_pdfs[i], m_pdfs[i], false );
      pdf.resetExternals();
    }
  }
  m_norm = norm(); /// update normalisation 
}
std::vector<ProcessParameters> FastIncoherentSum::fitFractions(
    AmpGen::Minimiser& minuit ){
  std::vector<Complex> co;
  auto covMatrix = minuit.covMatrixFull();
  for( auto& p : m_minuitParameters ){
    co.push_back( Complex( Parameter( p.first->name() , p.second->mean() ) , 
          Parameter( p.second->name(), p.second->mean() ) ) );
  }
  std::vector<ProcessParameters> latexTable;
  std::string pfx = (m_prefix == "" ? "" : m_prefix +"_");

  std::vector<Parameter> params;
  for( unsigned int i = 0 ; i < minuit.nPars(); ++i ) 
    params.push_back( Parameter( minuit.getParPtr(i)->name() ) );

  Expression normalisation; 
  for( unsigned int i=0;i<m_minuitParameters.size();++i){
    normalisation = normalisation + co[i].norm()*m_normalisations[0][i].real();
  }
  std::vector<Observable> fractions;
  INFO("Making : " << m_minuitParameters.size() << " fractions ... " ) ; 
  for( unsigned int i=0;i<m_minuitParameters.size();++i){

    Observable FF( co[i].norm()*m_normalisations[0][i].real() / normalisation ,
        m_decayTrees[i]->uniqueString() );

    FF.evaluate( covMatrix, params );
    fractions.push_back(FF);

    auto re = *m_minuitParameters[i].first;
    auto im = *m_minuitParameters[i].second ;
    ProcessParameters paramsForThisProcess;
    paramsForThisProcess.setAmplitude( std::complex<double>(re.mean(),im.mean()), std::complex<double>(re.err(),im.err()));
    paramsForThisProcess.setFraction( FF.getVal(), FF.getError());
    paramsForThisProcess.setParticle( m_decayTrees[i]);
    latexTable.push_back( paramsForThisProcess );

  }
  std::sort( fractions.begin(), fractions.end() );
  for( auto fraction = fractions.begin() ; fraction != fractions.end(); ++fraction )
    std::cout << std::setw(55) << pfx+fraction->name() << "   " 
      << std::setw(7)  << fraction->getVal() 
      << std::setw(7)  << " +/- " << fraction->getError() << std::endl;
  return latexTable;
} 
