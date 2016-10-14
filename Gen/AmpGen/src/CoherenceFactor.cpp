#include "AmpGen/CoherenceFactor.h"
#include "AmpGen/Utilities.h"

using namespace AmpGen;

 /// this function maps voxels to bin groups /// 

void CoherenceFactor::makeCoherentMapping( const unsigned int& nBins){
  m_voxels = BinDT( *m_data, 5, 100 );
  m_nBins = nBins; 
  m_voxelID2Bin.clear();

  std::vector<std::complex<double>> voxel_r( m_voxels.size(), 0 );
  std::vector<double> voxel_N1( m_voxels.size(), 0);
  std::vector<double> voxel_N2( m_voxels.size(), 0);

  double norm = 0 ;
  for( auto& evt : *m_data ){
    unsigned int binNumber =  m_voxels.getBinNumber(evt );
    voxel_r[binNumber] += m_pdf1->getVal( evt ) * std::conj( m_pdf2->getVal( evt )) * m_globalRot ; 
    voxel_N1[binNumber] += std::norm( m_pdf1->getVal( evt ));
    voxel_N2[binNumber] += std::norm( m_pdf2->getVal( evt ));
    norm += std::norm( m_pdf1->getVal( evt ));
  }
  typedef std::pair<double,unsigned int> voxel_info;
  std::vector<voxel_info> fromPhaseToVoxelNumber ; 
  for( unsigned int i = 0 ; i < m_voxels.size() ; ++i ){
    double binPhase = std::arg( voxel_r[i]);
    fromPhaseToVoxelNumber.emplace_back( binPhase, i );
  }
  std::sort( fromPhaseToVoxelNumber.begin(),
             fromPhaseToVoxelNumber.end(),
             []( const voxel_info& b1,const voxel_info& b2 ){ return b1.first < b2.first ; } );

  norm /= (double)nBins ; 
  double currentNorm = 0 ;
  unsigned int currentBin=0;
  std::vector<std::pair<double,double>> binLimits;
  binLimits.emplace_back( -M_PI , 0 );

  auto currentBinLimits = binLimits.rbegin();

  for( auto& pairObj : fromPhaseToVoxelNumber ){
    unsigned int voxelId = pairObj.second; 
    currentNorm += voxel_N1[ voxelId ]; 
    m_voxelID2Bin[ voxelId ] = currentBin; 
    double maxPhase = std::arg( voxel_r[ voxelId ]  ) ; 
    if( currentNorm > norm ){ /// the logic here is a little tricky, 
      currentNorm = 0 ;
      currentBinLimits->second = maxPhase;
      currentBin++;
      binLimits.emplace_back( maxPhase, 0 );
      currentBinLimits = binLimits.rbegin() ;
    }
  };
  for( auto& limits : binLimits ){
    INFO( "Bin Limits [" << limits.first << ", " << limits.second << "]" );
  }

}

/*
void CoherenceFactor::getFitFractions( AmpGen::Minimiser& minuit ){
  for( auto& bin : m_bins ){
    EventList events = bin.events( m_data->getEventType() );

    m_pdf1->setMC( events );
    m_pdf1->prepare();
    m_pdf1->fitFractions(minuit, std::cout);
  }
}
*/

void CoherenceFactor::getNumberOfEventsInEachBin(const EventList& events ) const {

  std::vector<unsigned int> counter( m_nBins , 0 );
  for( auto& event : events ){
    counter[ getBinNumber( event )] ++;
  }
  for( unsigned int i = 0 ; i < m_nBins; ++i){
    INFO("Bin " << i << " # = " << counter[i] );
  };

}

/*
void CoherenceFactor::readBinsFromFile( const std::string& binName ){
  INFO("Loading bins from file :" << binName );
  m_bins = getBinningFromFile( binName );
  for( auto& bin : m_bins ) bin.lowerBoundSort( 0 );
  INFO("Got " << m_bins.size() << " bins");
  
  for( auto& bin : m_bins ){
    bin.add( m_data );
    for( auto& voxel : bin ) setVoxelCoherence( voxel );
  }
  
}
*/
