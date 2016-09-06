#include "AmpGen/CoherenceFactor.h"
#include "AmpGen/Utilities.h"

using namespace AmpGen;

std::vector<Binning> CoherenceFactor::makeCoherentBins( const unsigned int& nBins){
  bool needToSplitMore = false;
  Bin bin0;
  std::vector<std::pair<double,double>> limits;
  for( auto& indices : gChi2Indices ) limits.push_back( m_data.getEventType().minmax(indices ));

  for( unsigned int i=0;i < limits.size();++i )
    bin0.setBoundary(i , limits[i]);
  bin0.addData( m_data.begin(), m_data.end(), true );
  Binning dataBins;
  dataBins.push_back( bin0 );

  do {
    needToSplitMore = false;
    Binning newBinning;
    INFO("Splitting " << dataBins.size() << " bins" );
    for( auto& bin : dataBins ){
      if( bin.getN() > 100 ){
        //  INFO("Splitting bin with population = " << bin.getN() );
        needToSplitMore = true;
        newBinning.add( bin.binSplit() );
      }
      else newBinning.add( bin );
    }
    dataBins = newBinning;
  } while( needToSplitMore );

  /// hyper-voxelate the data /// 

  for( auto& voxel : dataBins ) setVoxelCoherence( voxel );
  std::sort( dataBins.begin(), dataBins.end(), []( auto& b1, auto& b2 ){ return std::arg( b1.getValue() ) < std::arg( b2.getValue() ) ; } );

  double norm=0 ;
  for( auto& bin : dataBins ) norm += bin.getNorm1();
  norm /= (double)nBins ; 

//  auto begin = dataBins.begin();
//  auto it = dataBins.begin();
  std::vector<Binning> coherentBins;
  std::vector<std::pair<double,double>> binLimits; 
  std::ofstream binPersist; 
  binPersist.open( ("EqualWSBinning.dat"));
  coherentBins.push_back( Binning() );
  binLimits.push_back( std::make_pair( -M_PI , 0 ) );
  auto currentBin = coherentBins.rbegin(); 
  auto currentBinLimits = binLimits.rbegin();
  double currentNorm = 0 ;
  for( auto& voxel : dataBins ){
    currentNorm += voxel.getNorm1();
    currentBin->push_back( voxel );
    double maxPhase = std::arg( voxel.getValue() ) ; 
    if( currentNorm > norm ){ /// the logic here is a little tricky, 
      currentNorm = 0 ;
      currentBinLimits->second = maxPhase;
      coherentBins.push_back( Binning() );
      currentBin = coherentBins.rbegin() ;
      binLimits.push_back( std::make_pair( maxPhase, 0 ) );
      currentBinLimits = binLimits.rbegin() ;

    }
  };
  for( auto& bins : coherentBins ){
    bins.print( binPersist );
    binPersist << std::endl;
  }
  for( auto& limits : binLimits ){
    INFO( "Bin Limits [" << limits.first << ", " << limits.second << "]" );
  }

  binPersist.close();
  return coherentBins;
}


void CoherenceFactor::setVoxelCoherence( Bin& voxel ){
  double norm1=0;
  double norm2=0;
  std::complex<double> coherence=0;

  for( auto evt = voxel.beginData() ; evt != voxel.endData(); ++evt ){
    norm1 += std::norm( m_pdf1->getVal( *evt ));
    norm2 += std::norm( m_pdf2->getVal( *evt ));
    coherence += m_pdf1->getVal( *evt ) * std::conj( m_pdf2->getVal( *evt )) * m_globalRot ;
  }
  voxel.setValue( coherence, norm1, norm2 );
}

void CoherenceFactor::getFitFractions( AmpGen::Minimiser& minuit ){
  for( auto& bin : m_bins ){
    EventList events = bin.events( m_data.getEventType() );
    
    m_pdf1->setMC( events );
    m_pdf1->prepare();
    m_pdf1->fitFractions(minuit, std::cout);
  }
}

void CoherenceFactor::getNumberOfEventsInEachBin(const EventList& events ){

  std::vector<unsigned int> counter( m_bins.size() , 0 );
  for( auto& event : events ){
    for( unsigned int i = 0 ; i < m_bins.size(); ++i ){
      if( m_bins[i].isIn( event ) ){ counter[i]++; break ; }
    };
  };
  for( unsigned int i = 0 ; i < m_bins.size(); ++i){
    INFO("Bin " << i << " # = " << counter[i] );
  };

}


std::vector<Binning> CoherenceFactor::getBinningFromFile( const std::string& filename ){ 

  std::vector<std::vector<std::string>> binsAsStrings;
  std::string tmp;
  std::ifstream inFile( filename.c_str() );
  std::vector<Binning> bins; 
  while( inFile.good() )
  {
    std::getline( inFile, tmp );
    if( tmp.length() != 0 ) 
      bins.emplace_back( tmp );  
  }
  return bins; 
}
