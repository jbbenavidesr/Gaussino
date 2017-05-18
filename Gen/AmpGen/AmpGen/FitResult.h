#ifndef LOGFILE_H
#define LOGFILE_H

#include "AmpGen/MinuitParameter.h"
#include "AmpGen/EventType.h"
#include "TMatrixD.h"
#include "AmpGen/Particle.h"
#include "AmpGen/Utilities.h"
#include "AmpGen/FitFraction.h"

namespace AmpGen {

  class FitResult {
    private : 

      double m_chi2;
      double m_LL;
      double m_nBins;
      int    m_status;
      double m_nParam; 
      AmpGen::EventType m_eventType;
      std::map<std::string,double> m_observables;
      std::vector<AmpGen::FitFraction> m_fitFractions; 
      TMatrixD m_covarianceMatrix; 
      std::shared_ptr<AmpGen::MinuitParameterSet> m_mps; 

      bool m_fitted;
      /// internally used parsing functions 
      std::string getLastLine( std::ifstream& in ) const ; 
      void addToParameters( const std::string& line );
      void addToObservables( const std::string& line );
      void setFitQuality( const std::string& line );
    public:
      
      FitResult( AmpGen::MinuitParameterSet& mps, 
          const std::vector<AmpGen::FitFraction>& fractions, 
          const TMatrixD& covarianceMatrix, 
          const double& Ichi2,
          const double& Idof,
          const double& ILL,
          const int& Istatus );
      FitResult( const std::string& filename , const AmpGen::EventType& evtType = AmpGen::EventType() );
      FitResult(){};
      bool readFile( const std::string& fname ); 
      void writeToFile( const std::string& fname );

      double chi2() const { return m_chi2 ; }
      double LL() const { return m_LL ; }
      int status() const { return m_status ; } 
      
      double dof() const { return m_nBins - m_nParam - 1; }
      std::vector<AmpGen::MinuitParameter*> getParameters() const ; 
      std::vector<AmpGen::MinuitParameter*> getFloating() const ;
      std::vector<FitFraction> getFitFractions() const { return m_fitFractions ; }
      AmpGen::MinuitParameterSet* MPS() const { return &(*m_mps) ; }
      TMatrixD getReducedCovariance() const ;
      TMatrixD cov() const { return m_covarianceMatrix ; }
  };
}

#endif
