#include "AmpGen/Binning.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/FastCoherentSum.h"
#include "AmpGen/EventList.h"

namespace AmpGen { 
class CoherenceFactor {
  private:
    std::complex<double> m_coherence;
    double m_norm1;
    double m_norm2;
    std::complex<double> m_globalRot;
    EventList& m_data;  
    FastCoherentSum* m_pdf1;
    FastCoherentSum* m_pdf2;
    std::vector<Binning> m_bins; 
  public: 
    double phase(){ return std::arg( m_coherence*m_globalRot / ( sqrt( m_norm1 ) * sqrt( m_norm2 ) ) ); }
    double getR(){ return std::abs( m_coherence*m_globalRot / ( sqrt( m_norm1 ) * sqrt( m_norm2 ) ) ) ; }
    void getNumberOfEventsInEachBin( const EventList& events ); 
    std::vector<Binning> makeCoherentBins(const unsigned int& nBins);
    std::vector<Binning> getBinningFromFile( const std::string& file); 
    void getFitFractions( AmpGen::Minimiser& mini ); 
    std::complex<double> getGlobalCoherence(EventList& _data){
      double globalNorm1=0;
      double globalNorm2=0;
      double real=0;
      double imag=0;
      m_pdf1->prepare();
      m_pdf2->prepare();
      #pragma omp parallel for reduction(+:globalNorm1,globalNorm2,real,imag)
      for( unsigned int i = 0 ; i < _data.size(); ++i){
        const Event& evt = _data[i];
        std::complex<double> pdf1 = m_pdf1->getVal(evt) ;
        std::complex<double> pdf2 = m_pdf2->getVal(evt) ;

        globalNorm1 += std::norm( pdf1);
        globalNorm2 += std::norm( pdf2);
        //INFO( m_pdf2->getVal(evt) );
        real += std::real( pdf1 )* std::real( pdf2 ) + std::imag( pdf1 )*std::imag( pdf2 );
        imag += std::imag( pdf1 )* std::real( pdf2 ) - std::real( pdf1 )*std::imag( pdf2 );
      }; 
      std::complex<double> globalCoherence(real,imag);
      m_coherence = globalCoherence;
      m_norm1 = globalNorm1;
      m_norm2 = globalNorm2;
      globalCoherence /= sqrt( globalNorm1* globalNorm2 );
      return globalCoherence;
    };
    void setVoxelCoherence( Bin& voxel ) ; 

    CoherenceFactor( 
        EventList& _data,  
        FastCoherentSum* pdf1, FastCoherentSum* pdf2 ) : m_data(_data), m_pdf1(pdf1), m_pdf2(pdf2){

      auto globalCoherence = getGlobalCoherence(_data);

      INFO("R_{K3\\pi} = " << std::abs( globalCoherence ) 
          << " ; \\delta_{K3\\pi} = " << std::arg( globalCoherence) );
      double delta = AmpGen::NamedParameter<double>("globalPhase", 2.967);
      double globalRotation = delta - std::arg(globalCoherence) ; /// globalRotation /// 
      m_globalRot = std::complex<double>( cos( globalRotation), sin(globalRotation) );
      std::string binningName = AmpGen::NamedParameter<std::string>("Binning",(std::string)"");
      if( binningName == "" ) m_bins = makeCoherentBins(4);
      else {
        INFO("Loading bins from file :" << binningName );
        m_bins = getBinningFromFile( binningName );
        INFO("Got " << m_bins.size() << " bins");
        for( auto& bin : m_bins ){
          bin.add( m_data );
          for( auto& voxel : bin ) setVoxelCoherence( voxel );
        }
      }
    }
    void printCoherence( std::ostream& stream = std::cout ){
      stream << "Coherence Factor:" << std::endl;
      stream << "R_K3pi = " << getR() << " delta = " << phase() << std::endl; 
      for( auto& bigBin : m_bins ){
        std::complex<double> coh(0,0);
        double n1(0);
        double n2(0);
        for( auto& bin : bigBin ){
          coh+=bin.getValue();
          n1+=bin.getNorm1();
          n2+=bin.getNorm2();
        }
        coh /= sqrt(n1*n2 );
        stream << "| " << std::setw(10) << std::abs(coh) << " | "  << std::setw(10) << std::arg(coh) << " | " << std::setw(10) << n1 << " | " << std::setw(10) << n2 << " | "<< std::endl; 
      };


    };
}; } 
