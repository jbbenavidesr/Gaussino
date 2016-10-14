#include "AmpGen/BinDT.h"
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

      EventList* m_data;  
      FastCoherentSum* m_pdf1;
      FastCoherentSum* m_pdf2;
      BinDT m_voxels; 
      std::map<unsigned int, unsigned int> m_voxelID2Bin;
      unsigned int m_nBins; 
    public: 
      double phase(){ 
        return std::arg( m_coherence*m_globalRot / ( sqrt( m_norm1 ) * sqrt( m_norm2 ) ) ); 
      }
      double getR(){ 
        return std::abs( m_coherence*m_globalRot / ( sqrt( m_norm1 ) * sqrt( m_norm2 ) ) ) ; 
      }
      void getNumberOfEventsInEachBin( const EventList& events ) const; 
      void getFitFractions( AmpGen::Minimiser& mini ); 
      std::complex<double> getGlobalCoherence(EventList* _data){
        double globalNorm1=0;
        double globalNorm2=0;
        double real=0;
        double imag=0;
        m_pdf1->prepare();
        m_pdf2->prepare();
        #pragma omp parallel for reduction(+:globalNorm1,globalNorm2,real,imag)
        for( unsigned int i = 0 ; i < _data->size(); ++i){
          const Event& evt = (*_data)[i];
          std::complex<double> pdf1 = m_pdf1->getVal(evt) ;
          std::complex<double> pdf2 = m_pdf2->getVal(evt) ;
          globalNorm1 += std::norm( pdf1);
          globalNorm2 += std::norm( pdf2);
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

      void writeToFile( const std::string& filename ) { 
        std::ofstream output;
        output.open( filename );
        std::vector< std::vector<unsigned int> > flatMap( m_nBins );
        for( auto& voxel : m_voxels ) 
          flatMap[ m_voxelID2Bin[ voxel->binNumber() ] ].push_back( voxel->binNumber() );
        for( auto& bin : flatMap ){
          for( auto& voxel : bin ){
            output << voxel << " ";
          }
          output << "\n";
        }
        output << "VOXELS" << std::endl;
        m_voxels.serialize( output );
        output.close();
      }
      CoherenceFactor( const std::string& filename ){
        std::ifstream stream;
        stream.open( filename );
        std::string line; 
        unsigned int binNumber = 0 ; 
        INFO( "Reading file = " << filename );
        while( getline(stream,line) && line != "VOXELS" ){
          auto voxels = split( line, ' ' );
         
          for( auto& voxel : voxels ) m_voxelID2Bin[ stoi(voxel) ] = binNumber;
          binNumber++;
        };   
        m_nBins = binNumber;
        m_voxels = BinDT( stream );
        stream.close();
        INFO( "Got " << m_nBins << " coherent bins and " << m_voxels.size() << " voxels" );
      };
      CoherenceFactor() {};
      CoherenceFactor( 
          EventList* _data,  
          FastCoherentSum* pdf1, 
          FastCoherentSum* pdf2 ) : m_data(_data), m_pdf1(pdf1), m_pdf2(pdf2){

        m_coherence = getGlobalCoherence(_data);
        INFO("R = " << std::abs( m_coherence ) 
            << " ; \\delta = " << std::arg( m_coherence ) );
      }
      void setGlobalPhase( const double& delta ){
        double globalRotation = delta - std::arg(m_coherence) ; /// globalRotation /// 
        m_globalRot = std::complex<double>( cos( globalRotation), sin(globalRotation) );
      }
      void makeCoherentMapping( const unsigned int& nBins) ;       
      void readBinsFromFile( const std::string& binName ); /// 
      unsigned int getBinNumber( const Event& event ) const {
        unsigned int voxelID = m_voxels.getBinNumber( event );
        return m_voxelID2Bin.find( voxelID )->second ;
      }
  };

} 
