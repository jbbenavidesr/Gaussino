#include "AmpGen/FastIncoherentSum.h"

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
  for( unsigned int i=0;i<m_minuitparameters.size();++i){
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
std::vector<std::string> FastIncoherentSum::fitFractions(
    AmpGen::Minimiser& minuit , std::ofstream& stream ){
  std::vector<Complex> co;
  auto covMatrix = minuit.covMatrixFull();
  std::vector<std::string> formatted;
  std::string pfx = (m_prefix == "" ? "" : m_prefix +"_");

  for( unsigned int i=0;i<m_minuitparameters.size(); ++i){
    co.push_back( Complex( FitParameter( m_minuitparameters[i].first) , 
          FitParameter( m_minuitparameters[i].second ) ) );
  }
  std::vector<Parameter> params;
  for( unsigned int i = 0 ; i < minuit.nPars(); ++i ) 
    params.push_back( Parameter( minuit.getParPtr(i)->name() ) );

  Expression normalisation; 
  for( unsigned int i=0;i<m_minuitparameters.size();++i){
    normalisation = normalisation + co[i].norm()*m_normalisations[0][i].real();
  }
  std::vector<Observable> fractions;
  INFO("Making : " << m_minuitparameters.size() << " fractions ... " ) ; 
  for( unsigned int i=0;i<m_minuitparameters.size();++i){

    Observable FF( co[i].norm()*m_normalisations[0][i].real() / normalisation ,
        m_decayTrees[i]->uniqueString() );

    FF.evaluate( covMatrix, params );
    fractions.push_back(FF);

    auto re = m_minuitparameters[i].first;
    auto im = m_minuitparameters[i].second ; 
    stream << pfx+m_decayTrees[i]->uniqueString() << " : "
      << FF.getVal() << " " << FF.getError() << " "
      << re->mean() << " " 
      << re->err() << " "
      << im->mean() << " " 
      << im->err() << std::endl;
    std::cout  << "$" << m_decayTrees[i]->getTex() 
               << "$ & " << std::setprecision(3) << numberWithError(FF.getVal(),FF.getError(),4) 
               << " & " <<numberWithError(re->mean() , re->err(),3) << "\\\\" << std::endl;
    std::string latexString = numberWithError(FF.getVal(),FF.getError(),4) + " & " +
      numberWithError( re->mean() , re->err(),3) ;
    formatted.push_back( latexString );

  }
  std::sort( fractions.begin(), fractions.end() );
  for( auto fraction = fractions.begin() ; fraction != fractions.end(); ++fraction )
    std::cout << std::setw(55) << pfx+fraction->name() << "   " 
      << std::setw(7)  << fraction->getVal() 
      << std::setw(7)  << " +/- " << fraction->getError() << std::endl;
  return formatted;
} 


