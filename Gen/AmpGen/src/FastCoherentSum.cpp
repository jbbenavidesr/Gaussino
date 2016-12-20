#include "AmpGen/FastCoherentSum.h"
#include "AmpGen/resolveParameters.h"
#include "AmpGen/LatexTable.h"

using namespace AmpGen; 

FastCoherentSum::FastCoherentSum( const EventType& type , 
    AmpGen::MinuitParameterSet& mps,
    const std::map<std::string, unsigned int>& extendEventFormat, 
    const std::string& prefix,
    unsigned int options  ) : 

  m_events(0),
  m_sim(0), 
  m_weight(1),
  m_prepareCalls(0), 
  m_lastPrint(0), 
  m_prefix(prefix),
  m_stateIsGood(true) {
    bool dbThis = options & OPTIONS::DEBUG;
    bool CPConjugate = options & OPTIONS::CPCONJUGATE;
    bool FlavConjugate = options & OPTIONS::FLAVCONJUGATE;

    std::map<std::string, std::pair<AmpGen::MinuitParameter*,AmpGen::MinuitParameter*>> tmpParams;
    std::map<std::string, AmpGen::MinuitParameter*> otherParameters; 

    for( unsigned int i =0 ; i < mps.size(); ++i ){
     AmpGen::MinuitParameter* parameter = mps.getParPtr(i);
      const std::string paramName = parameter->name();
      auto tokens = split( paramName, '_');
      std::string thisDecayName;
      std::string thisPrefix;
      std::string thisSuffix;
      if( tokens.size() == 3 ){ 
        thisPrefix = tokens[0];
        thisDecayName = tokens[1];    
        thisSuffix = tokens[2];
      }
      else if( tokens.size() == 2 ){
        thisDecayName = tokens[0];
        thisSuffix = tokens[1];
        thisPrefix = ""; /// assume no prefix ///
      }
      if( prefix == thisPrefix && ( thisSuffix == "Re" || thisSuffix == "Im" )){
        std::map<std::string, 
          std::pair<AmpGen::MinuitParameter*,
          AmpGen::MinuitParameter*>>::iterator ip = tmpParams.find( thisDecayName );

        if( ip == tmpParams.end() ){
          tmpParams[ thisDecayName ] = 
            std::pair<AmpGen::MinuitParameter*,AmpGen::MinuitParameter*>( 0 , 0 );

          DEBUG( "Adding: " << prefix << "   " << thisDecayName << "   " << thisSuffix );
          ip = tmpParams.find( thisDecayName );
        }
        if( thisSuffix == "Re" ) ip->second.first = parameter;
        else if( thisSuffix == "Im" ) ip->second.second = parameter; 
      }
      else {
        DEBUG("Another parameter = " << paramName );
        otherParameters[ paramName ] = parameter; 
      }
    }
    m_pdfs.reserve( tmpParams.size());
    m_decayTrees.reserve( tmpParams.size());
    for( auto& p : tmpParams ){
      std::vector<std::string> finalStates = type.finalStates();
      std::shared_ptr<Particle> decayTree = 
        std::make_shared<Particle>(p.first, finalStates );

      if( ! decayTree->isStateGood() ){
        ERROR("Decay tree not configured correctly");
        m_stateIsGood = false;
        return; 
      }
      if(CPConjugate){
        decayTree->CPConjugateThis();
        std::shared_ptr<Particle> decayTreeCP = 
          std::make_shared<Particle>(decayTree->uniqueString(),finalStates );
        m_decayTrees.push_back( decayTreeCP );
        INFO("Adding tree = " << decayTreeCP->uniqueString() );
      }
      else {
        m_decayTrees.push_back( decayTree );
        INFO("Adding tree = " << p.first );
      }
      DEBUG("Configuring external parameters");

      std::vector<DBSYMBOL> dbExpressions;
      if( FlavConjugate ) (*m_decayTrees.rbegin())->setConj( true );
      const std::string name = (*m_decayTrees.rbegin())->uniqueString();
      const Expression expression = 
        (*m_decayTrees.rbegin())->getExpression(dbThis?&dbExpressions:NULL);    
      DEBUG("Got expression for this tree");
      m_pdfs.emplace_back( expression , name , extendEventFormat, dbThis?&dbExpressions:NULL );
     
      auto newAddresses = resolveParameters( &(*m_pdfs.rbegin()) , otherParameters );
      for( auto& addr : newAddresses ) m_addressMapping.push_back( addr );
     
      std::pair<AmpGen::MinuitParameter*,AmpGen::MinuitParameter*> parameters = p.second;
      if (parameters.first == 0 || parameters.second == 0 ){
        ERROR("Amplitude " << name 
            << " not properly configured : pointers = (" 
            << parameters.first << " , " 
            << parameters.second << ") , prefix = " << prefix );
        m_stateIsGood = false;
        return; 
      }
      m_coefficients.push_back( 
          std::complex<double>( parameters.first->mean() , parameters.second->mean() ) );
      m_minuitParameters.push_back( parameters );
    }
    for( unsigned int i = 0 ; i < m_decayTrees.size(); ++i){
      std::string name = m_decayTrees[i]->uniqueString();
      m_minuitParameters[i].first->setName( name +"_Re");
      m_minuitParameters[i].second->setName( name +"_Im");
    }
    DEBUG("Configured all parameters");
    if( m_pdfs.size() == 0 ) 
      WARNING("No expressions found for amplitude with prefix = " << m_prefix ); 
    else 
      INFO("PDF has " << m_pdfs.size() << " amplitudes" ) ; 

    m_normalisations.resize( m_pdfs.size(), std::vector<std::complex<double>>( m_pdfs.size() ) );
  }

void FastCoherentSum::prepare(){
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

void FastCoherentSum::debug( const unsigned int& N, const std::string& nameMustContain){ 
  for( auto& pdf : m_pdfs ) pdf.resetExternals();
  if( nameMustContain == "" ) for( auto& pdf : m_pdfs ) pdf.debug( m_events->getEvent(N) );
  else 
    for( auto& pdf: m_pdfs ) if( pdf.name().find( nameMustContain ) != std::string::npos ) pdf.debug( m_events->getEvent(N) );
  prepare();
  INFO( "Pdf = " << prob( m_events->at(N) )); 
}

std::vector<FitFraction> FastCoherentSum::fitFractions(AmpGen::Minimiser& minuit ){  
  return fitFractions(minuit.covMatrixFull() );
}

std::vector<FitFraction> FastCoherentSum::fitFractions( const TMatrixD& covMatrix){
  
  std::vector<Complex> co;
  std::vector<Observable> fractions;
  std::vector<Observable> interferenceTerms;
  std::vector<FitFraction> outputFractions; 
  std::vector<Parameter> params;
  Expression normalisation; 
  Expression diagonalFitFraction; 
  for( auto& p : m_minuitParameters ){
    unsigned int s = params.size();
    params.push_back( Parameter( p.first->name() , p.first->mean()  ,true,true) );
    params.push_back( Parameter( p.first->name(), p.second->mean() ,true,true) );
    co.push_back( Complex(params[s],params[s+1]  )); 
  }
  for( unsigned int i=0;i<m_minuitParameters.size();++i){

    diagonalFitFraction = diagonalFitFraction + co[i].norm()*m_normalisations[i][i].real();
    for( unsigned int j=0; j < m_minuitParameters.size(); ++j )
      normalisation = normalisation + co[i] * co[j].conj() * m_normalisations[i][j] ;
  }
  for( unsigned int i=0;i<m_minuitParameters.size();++i){

    Observable FF( co[i].norm()*m_normalisations[i][i].real() / normalisation ,
        m_decayTrees[i]->uniqueString() );
    FF.evaluate( covMatrix , params );
    fractions.push_back(FF);
    for( unsigned int j=i+1; j < m_minuitParameters.size(); ++j ){
      Complex fij = co[i]*co[j].conj()*m_normalisations[i][j] ;
      Observable IF( fij.real() / normalisation,
          m_decayTrees[i]->uniqueString() + " x " + m_decayTrees[j]->uniqueString() );
      IF.evaluate( covMatrix, params);
      interferenceTerms.push_back( IF);
    }   
    outputFractions.emplace_back( m_decayTrees[i]->uniqueString(), FF.getVal(),FF.getError(), m_decayTrees[i] );
  }
  Observable OFF( diagonalFitFraction / normalisation ,"SumOfFitFractions");
  OFF.evaluate( covMatrix, params ); 
  INFO( "Diagonal Fit Fraction = " << numberWithError( OFF.getVal() , OFF.getError(),4)  );
  DEBUG( fractions.size() << " fit fraction observables");
  DEBUG( interferenceTerms.size() << " interference observables");

  std::sort( fractions.begin(), fractions.end() );
  std::sort( interferenceTerms.begin(), interferenceTerms.end() );

  for( auto fraction = fractions.begin() ; fraction != fractions.end(); ++fraction )
    std::cout << std::setw(55) << fraction->name() << "   " 
      << std::setw(7)  << fraction->getVal() 
      << std::setw(7)  << " +/- " << fraction->getError() << std::endl;

  std::cout << "#################################################" << std::endl; 
  return outputFractions ; 
}

void FastCoherentSum::makeBinary( const std::string& fname, const double& normalisation ){
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
  stream << " return std::norm(amplitude) / "<< normalisation << " ; }" << std::endl; 
  stream.close();
}

