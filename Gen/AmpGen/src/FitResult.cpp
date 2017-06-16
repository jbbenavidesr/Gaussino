#include "AmpGen/MsgService.h"
#include "AmpGen/FitResult.h"
#include "AmpGen/Utilities.h"
#include "AmpGen/MinuitParameterSet.h"

using namespace AmpGen; 

FitResult::FitResult( const std::string& filename, const EventType& evtType ) 
{
  m_eventType = evtType; 
  m_fitted = readFile( filename);
}

std::string FitResult::getLastLine(std::ifstream& in) const 
{
  std::string line;
  while (in >> std::ws && std::getline(in, line)) ;
  return line;
}

bool FitResult::readFile( const std::string& fname )
{
  std::ifstream CHECK(fname);
  if( !CHECK.is_open() || CHECK.peek() == std::ifstream::traits_type::eof() ){
    return false;
  }
  if( getLastLine(CHECK) != "End Log" ){
    ERROR( "File not properly close "<< fname ) ;  
    return false;
  }
  CHECK.close();

  auto lines = vectorFromFile( fname );
  std::vector<std::string> parameterLines; 
  for( auto& line : lines ){
    const std::string name = split( line, ' ' )[0];
    if( name == "Parameter" ) parameterLines.push_back( line );
    else if( name == "FitQuality" ) setFitQuality(line);
    else if( name == "FitFraction" )  m_fitFractions.emplace_back( line, m_eventType );
    else if( name == "Observable" ) addToObservables(line);
  }
  unsigned int nParameters = parameterLines.size();
  m_covarianceMatrix.ResizeTo( parameterLines.size(),parameterLines.size() );
  m_mps = std::make_shared<MinuitParameterSet>();
  for( unsigned int i = 0 ; i < nParameters ; ++i ){
    auto tokens = split( parameterLines[i], ' ');
    MinuitParameter* param = new MinuitParameter( tokens[1], stoi( tokens[2] ), stod( tokens[3]), stod( tokens[4] ) ,0,0, *(m_mps) );
    /// the ownership semanatics here are bizarre - fix at earliest opporunity /// 
    for( unsigned int j = 0; j < nParameters; ++j )
      m_covarianceMatrix(i,j) = stod( tokens[5+j] );
  }
  return true;
}

void FitResult::setFitQuality( const std::string& line ) {
  auto tokens = split( line, ' ');
  m_chi2 = stod( tokens[1] );
  m_nBins = stod( tokens[2] );
  m_nParam = stod( tokens[3] );
  m_LL = stod( tokens[4]);
  m_status = stoi( tokens[5] );
}
void FitResult::addToObservables( const std::string& line ){
  auto tokens = split( line, ' ' );
  m_observables[ tokens[1] ] = stod( tokens[2] );
}


void FitResult::writeToFile( const std::string& fname ){
  std::ofstream outlog;
  outlog.open(fname);

  for(int i = 0 ; i < m_covarianceMatrix.GetNrows(); ++i){
    auto param = m_mps->getParPtr(i);
    outlog << "Parameter" 
      << " " << param->name() 
      << " " << param->iFixInit() 
      << " " << param->mean() 
      << " " << m_mps->getParPtr(i)->err() << " ";
    for(int j = 0 ; j < m_covarianceMatrix.GetNcols(); ++j) 
      outlog << m_covarianceMatrix[i][j] << " "; 
    outlog << std::endl; 
  } 
  outlog << "FitQuality " << m_chi2 << " " << m_nBins << " " << m_nParam << " " << m_LL << " " << m_status << "\n"; 
  for( auto& p : m_fitFractions ){
    outlog << "FitFraction " << p.name() << " " << p.val() << " " << p.err() << "\n";
  }
  outlog << "End Log\n";
  outlog.close();
}

FitResult::FitResult(AmpGen::MinuitParameterSet& mps, 
    const std::vector<FitFraction>& fractions, 
    const TMatrixD& covarianceMatrix, 
    const double& Ichi2,
    const double& Inbin,
    const double& ILL,
    const int& Istatus ) : 
  m_fitFractions(fractions),
  m_covarianceMatrix( covarianceMatrix ), 
  m_mps(std::make_shared<MinuitParameterSet>(mps)) {
    m_chi2 = Ichi2;
    m_nBins = Inbin;
    m_LL = ILL;
    m_status = Istatus ;
    unsigned int nParam=0;
    for( unsigned int i=0;i<m_mps->size();++i) {
      auto param = m_mps->getParPtr(i);
      if( param->iFixInit() == 0 ) nParam++;
    }

    m_nParam = nParam;
    INFO( "Chi2 per bin = " << m_chi2 / m_nBins  );
    INFO( "Chi2 per dof = " << m_chi2 / dof() );
    INFO( "-2LL         = " << m_LL );
    INFO( "Fit Status   = " << m_status );

  }

std::vector<AmpGen::MinuitParameter*> FitResult::getParameters() const {
  std::vector<AmpGen::MinuitParameter*> params;
  for( auto& param : *m_mps ) params.push_back( param );
  return params;
}

std::vector<AmpGen::MinuitParameter*> FitResult::getFloating() const {
  std::vector<AmpGen::MinuitParameter*> floating; 
  for( auto& param : *m_mps ) {
    if( param->iFixInit() == 0 ) floating.push_back( param );
  }
  return floating; 
}


TMatrixD FitResult::getReducedCovariance() const {
  std::vector<unsigned int> floating_indices;
  for( unsigned int i = 0 ; i < m_mps->size(); ++i ){
    if( m_mps->getParPtr(i)->iFixInit() == 0 ) floating_indices.push_back(i);
  }
  TMatrixD reducedCov( m_nParam, m_nParam );
  for( unsigned int i = 0 ; i < floating_indices.size(); ++i ){
    for( unsigned int j = 0 ; j < floating_indices.size(); ++j ){
      reducedCov(i,j) = m_covarianceMatrix( floating_indices[i], floating_indices[j] );
    }
  }
  return reducedCov; 
}
