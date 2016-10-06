
#include "EventList.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"
#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/Utilities.h"

/// takes an event, plots 
namespace AmpGen { 

  template < class FUNCTION, class PDF, class CUT > 
    void plot1D( const EventList&   data,
                 const EventList&     mc,
                 const FUNCTION&    func,
                 const CUT&          cut,
                 const PDF&          pdf,
                 const plotAxis&    axis,
                 const std::string& name ){
       plot1D( data, func, cut, axis, "Data_"+name);
       makePerAmplitudePlot( mc, pdf, func, cut, axis, name );
    }


  template < class T1 , class T2 , class TC> 
    TH2D* plot2D( const EventList& evts, 
        const T1& f1, 
        const T2& f2, 
        const TC& fCut , 
        double min1, 
        double max1, double min2, double max2 ,
        const std::string& plotName, const std::string& f1Label, const std::string& f2Label
        ){
      TH2D* plot = new TH2D( plotName.c_str(), plotName.c_str(), 50, min1, max1, 50, max1, max2 );
      for( auto& evt : evts ){

        if( fCut(evt)) plot->Fill( f1( evt ) , f2( evt) , evt.weight() );
      }
      return plot; 
    }

  template < class FUNCTION,  class CUT > 
    void plot1D( const EventList& evts,
        const FUNCTION& f1,
        const CUT& cut, 
        const plotAxis axis,
        const std::string& plotName ){
      gFile->cd();
      TH1D* plot = new TH1D( plotName.c_str(), "" , axis.nBins , axis.min, axis.max );
      plot->GetXaxis()->SetTitle( axis.title.c_str() );
      for( auto& evt : evts ) if( cut(evt) ) plot->Fill( f1(evt), evt.weight() );
      plot->Write();
      delete plot;
    }

  template< class FX, class FY,  class CUT>
    TH2D* plot2D( const EventList& evts, const FX& fx, const FY& fy, double minX, double maxX, double minY, double maxY, const std::string& name, const CUT& cut ){
      TH2D* plot = new TH2D( name.c_str(), "", 25, minX, maxX, 25, minY, maxY );
      for( auto& evt : evts ) if( cut(evt)) plot->Fill( fx(evt), fy(evt), evt.weight() );
      return plot;
    }


  template < class PDF, class FUNC, class CUT> 

    void makePerAmplitudePlot( 
        const EventList& evts, 
                 PDF pdf, 
        const FUNC& func, 
        const CUT& cut, 
        const plotAxis axis, 
        const std::string& plotName){
      TDirectory* dir = gFile->mkdir( plotName.c_str() );
      dir->cd();
      INFO("Making plot : " << plotName << "   " << axis.name.c_str() );
      for( unsigned int i = 0 ; i < pdf.size(); ++i){

        for( unsigned int j=i;j<pdf.size();++j){
          auto pdf_i = pdf.pdf(i);
          auto amp_i = pdf.amplitude(i);
          auto pdf_j = pdf.pdf(j);
          auto amp_j = pdf.amplitude(j);

          unsigned int index_i = pdf.cacheAddress(i);
          unsigned int index_j = pdf.cacheAddress(j);
          const std::string name = pdf_i.name()+"x"+pdf_j.name();
          INFO("Making plot : " << plotName << "/" << name );
          TH1D* real_projection = new TH1D(name.c_str(),"",axis.nBins, axis.min, axis.max );
          real_projection->GetXaxis()->SetTitle( axis.title.c_str() );
          std::string title = pdf.getTexTitle(i, true).c_str() ;
          if( i!=j ) title += " x " + pdf.getTexTitle(j,true) ;
          real_projection->SetTitle( title.c_str());
          for( auto evt = evts.begin() ; evt != evts.end(); ++evt ){
            if( ! cut(*evt) ) continue; 
            double f = func(*evt);
            std::complex<double> pdfValue = 
              evt->getCache(index_i) * amp_i * 
              std::conj( evt->getCache(index_j)*amp_j );  
            real_projection->Fill( f , (i==j?1:2)*pdfValue.real()*evt->weight(0) / evt->genPdf() );
          }
          real_projection->Write();
          delete real_projection; 
        }
      }
      dir->Write();
      gFile->cd();  
    }

  template < class PDF, class FX, class FY, class CUT>
    void makePerAmplitudePlot2D( const EventList& evts, PDF& pdf, FX fx, FY fy, const CUT& cut, const plotAxis& xAxis, const plotAxis& yAxis, const std::string& name){
      gFile->mkdir(name.c_str());
      gFile->cd(name.c_str());
      for( unsigned int i = 0 ; i < pdf.size(); ++i){

        for( unsigned int j=i;j<pdf.size();++j){
          auto pdf_i = pdf.pdf(i);
          auto amp_i = pdf.amplitude(i);
          auto pdf_j = pdf.pdf(j);
          auto amp_j = pdf.amplitude(j);

          unsigned int index_i = pdf.cacheAddress(i);
          unsigned int index_j = pdf.cacheAddress(j);
          TH2D* real_projection = new TH2D((pdf_i.name()+"x"+pdf_j.name()).c_str(),"", xAxis.nBins, xAxis.min, xAxis.max , yAxis.nBins, yAxis.min, yAxis.max);
          real_projection->GetXaxis()->SetTitle( xAxis.title.c_str() );
          real_projection->GetYaxis()->SetTitle( yAxis.title.c_str() );

          if( i==j ) real_projection->SetTitle( pdf.getTexTitle(i, true).c_str());
          else real_projection->SetTitle( ( pdf.getTexTitle(i,true) + " x " + pdf.getTexTitle(j,true)).c_str() );
          for( auto evt = evts.begin() ; evt != evts.end(); ++evt ){
            if( cut(*evt) ){
              double x = fx(*evt);
              double y = fy(*evt);
              std::complex<double> pdfValue = evt->getCache(index_i)*amp_i*std::conj( evt->getCache(index_j)*amp_j );
              real_projection->Fill( x,y , (i==j?1:2)*pdfValue.real()*evt->weight(0) / evt->genPdf() );
            }
          }
          real_projection->Write();
        }
      }
    }
}
