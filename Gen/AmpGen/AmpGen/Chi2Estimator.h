//#include "AmpGen/Binning.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/EventType.h"
#include "AmpGen/EventList.h"

#include "AmpGen/BinDT.h"

namespace AmpGen { 
  class Chi2Estimator {

    double m_chi2;
    unsigned int m_nBins;
    BinDT m_binning; 
    public: 
    double chi2(){ return m_chi2 ; }
    double nBins(){ return m_nBins ; } 
    struct moment {
      double x;
      double xx;
      double N;
      moment() : x(0), xx(0), N(0) {}
      void add( const double& value ) {
        x+=value;
        xx+=value*value;
        N++;
      }
      void rescale( const double& val ){
        x*=val;
        xx*=(val*val);
      }
      double  val(){ return x  ; }
      double  var(){ return N == 0 ? 0 : xx  - val() * val() / N ; } 
    };

    void writeBinningToFile( const std::string& filename ) { m_binning.serialize( filename ) ; } 
    void doChi2(const EventList& dataEvents, const EventList& mcEvents ){
      std::vector<moment> data( m_binning.size() , moment() );
      std::vector<moment>   mc( m_binning.size() , moment() );

      INFO("NEW BINNING : Split into " << m_binning.size() );
      unsigned int j = 0;
      for( auto& d : dataEvents ){
        if( j++ % 100000 == 0 ) INFO( "Binned " << j << " events" );
        data[m_binning.getBinNumber(d)].add( d.weight() );
      }
      j=0;  
      for( auto& d : mcEvents ){
        if( j++ % 100000 == 0 ) INFO( "Binned " << j << " events" );
        mc[m_binning.getBinNumber( d )].add( d.weight() );
      }
      double chi2=0;

      for( unsigned int i=0;i < m_binning.size() ; ++i ){
        double delta = data[i].val() - mc[i].val();
        double tChi2 = delta*delta / ( data[i].val() + mc[i].var() );

        DEBUG( data[i].val() << "   " << mc[i].val() << "  " << data[i].val() << " " << mc[i].var() << " " << tChi2 );
        if( tChi2 > 100 ) continue ;
        chi2 += tChi2;
      }
      m_chi2 = chi2;
      m_nBins = m_binning.size();
    };

    Chi2Estimator( 
        const EventList& dataEvents, 
        const EventList&   mcEvents, 
        const unsigned int& minEvents=10 ) : m_binning( BinDT( dataEvents,5,minEvents) ) {
      doChi2(dataEvents, mcEvents);
    }
    Chi2Estimator( 
        const EventList& dataEvents,
        const EventList&   mcEvents,
        const std::string& filename ) : m_binning( BinDT( filename ) ) {
      doChi2(dataEvents, mcEvents);
    }

    
  };
}
