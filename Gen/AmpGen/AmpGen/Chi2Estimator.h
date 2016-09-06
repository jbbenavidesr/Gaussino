#include "AmpGen/Binning.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/EventType.h"
#include "AmpGen/EventList.h"

namespace AmpGen { 
  class Chi2Estimator {

    double m_chi2;
    unsigned int m_nBins;

    public: 
    double chi2(){ return m_chi2 ; }
    double nBins(){ return m_nBins ; } 
    Chi2Estimator( 
        const EventList& _data, 
        const EventList& _mc , 
        const unsigned int& minEvents=10,
        const unsigned int& maxEvents=100 ){

      std::vector<std::pair<double,double>> _limits;
      for( auto& indices : gChi2Indices ) _limits.push_back( _data.getEventType().minmax(indices ));
      Bin bin0;
      for( unsigned int i=0;i<_limits.size();++i ) 
        bin0.setBoundary(i ,_limits[i]);
      bin0.addData( _data.begin(), _data.end(), true );
      Binning dataBins;
      dataBins.push_back( bin0 );
      bool needToSplitMore = false;
      INFO( "Defining binning with " << _data.size() );

      do {
        needToSplitMore = false;
        Binning newBinning;
        INFO("Splitting " << dataBins.size() << " bins" );
        for( auto& bin : dataBins ){
          if( bin.getN() > maxEvents ){
            //  INFO("Splitting bin with population = " << bin.getN() );
            needToSplitMore = true;
            newBinning.add( bin.binSplit() );
          }
          else newBinning.add( bin );
        }
        dataBins = newBinning;
      } while( needToSplitMore );     
      INFO(" Split into " << dataBins.size() );
      dataBins.lowerBoundSort(0);
      Binning mcBins = dataBins;
      mcBins.clear();
      //unsigned int counter = 0;
      INFO("Final number of bins = " << dataBins.size() );
      for( auto event = _mc.begin(); event != _mc.end(); ++event  ){
        const Event& evt = *event;
        if( (event - _mc.begin() ) % 100000 == 0 ){
          INFO( "Added " << (event - _mc.begin()) << " integration events"); 
        }
        auto mcBin =  std::lower_bound( mcBins.begin(), mcBins.end(), event->s( gChi2Indices[0] ) ) ; // , []( 
        if( mcBin == mcBins.end() ) mcBin--;
        //double lowerBound = mcBin->min(0) ;
        for( ; mcBin != mcBins.begin() -1; --mcBin ) 
          if( mcBin->add(evt) ) break;
        if( mcBin == mcBins.begin() -1 ){ DEBUG( "Failed to add event to bin!" ); }
      }
      std::reverse( dataBins.begin(), dataBins.end() );
      double chi2=0;
      TH1D* chi2Distribution = new TH1D("chi2","chi2", 500,0.,100.);
      for( unsigned int i = 0 ; i < mcBins.size(); ++i ){
        double dataPopulation = dataBins[i].getVal();
        double dataVariance = dataPopulation ;
        double mcPopulation = mcBins[i].getVal();
        double mcVariance = mcBins[i].variance();
        DEBUG( dataPopulation << " +/- " << sqrt(dataVariance) << " ; " << mcPopulation << " +/- " << sqrt(mcVariance) );
        double delta = dataPopulation - mcPopulation;
        if( dataVariance == 0 ){ WARNING("No MC in bin " << i ) ; continue ; } 
        double tChi2 = delta*delta / (dataVariance + mcVariance );
        chi2Distribution->Fill( tChi2 );
        if( tChi2 > 100 ) continue ; 
        chi2 += tChi2;
      }
      m_chi2 = chi2;
      m_nBins = mcBins.size();
      chi2Distribution->Write();
    }
  };
}
