#include "AmpGen/FastIncoherentSum.h"
#include "AmpGen/LatexTable.h"
#include "AmpGen/Observable.h"

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

std::vector<FitFraction> FastIncoherentSum::fitFractions(AmpGen::Minimiser& minuit ){  
  std::vector<MinuitParameter*> params;
  for( unsigned int i = 0 ; i < minuit.nPars(); ++i ) {
    params.push_back( minuit.getParPtr(i) );
  }
  return fitFractions(minuit.covMatrixFull(), params );
}


std::vector<FitFraction> FastIncoherentSum::fitFractions(
    const TMatrixD& covMatrix, const std::vector<MinuitParameter*>& minuitParameters ){
  std::vector<Complex> co;
  WARNING("This code is not properly tested - use at your peril"); 
  std::map<std::string,Parameter*> mapping;
  std::vector<Parameter> params;
  for( unsigned int i = 0 ; i < minuitParameters.size();++i ){
    auto p = minuitParameters[i];
    params.push_back( Parameter( p->name() , p->mean()  ,true,true) );
    DEBUG("Mapping " << p->name() << " to " << i );
  }
  for( unsigned int i=0;i<params.size();++i){
    mapping[minuitParameters[i]->name()] = &(params[i]);
  }
  for(auto& p : m_minuitParameters ){
    auto re = mapping.find(p.first->name());
    auto im = mapping.find(p.second->name());
    DEBUG("Looking for " << p.first->name() << " " << p.second->name()  << (re == mapping.end()) << "  " << (im == mapping.end()) );
    Expression realPart = (re == mapping.end()) ? Expression(Constant(p.first->mean())) : Expression(*re->second);
    Expression imagPart = (im == mapping.end()) ? Expression(Constant(p.second->mean())):Expression(*im->second);
    co.push_back( Complex(realPart,imagPart )); 
  }

  std::vector<FitFraction> outputFractions;
  std::string pfx = (m_prefix == "" ? "" : m_prefix +"_");

  Expression normalisation; 
  for( unsigned int i=0;i<m_minuitParameters.size();++i){
    normalisation = normalisation + co[i].norm()*m_normalisations[0][i].real();  }

  std::vector<Observable> fractions;
  INFO("Making : " << m_minuitParameters.size() << " fractions ... " ) ; 
  for( unsigned int i=0;i<m_minuitParameters.size();++i){

    Observable FF( co[i].norm()*m_normalisations[0][i].real() / normalisation ,
        m_decayTrees[i]->uniqueString() );

    FF.evaluate( covMatrix, params );
    outputFractions.emplace_back( m_decayTrees[i]->uniqueString(), FF.getVal(), FF.getError(), m_decayTrees[i] );
  }
  std::sort( fractions.begin(), fractions.end() );
  for( auto fraction = fractions.begin() ; fraction != fractions.end(); ++fraction )
    std::cout << std::setw(55) << pfx+fraction->name() << "   " 
      << std::setw(7)  << fraction->getVal() 
      << std::setw(7)  << " +/- " << fraction->getError() << std::endl;
  return outputFractions; 
} 
