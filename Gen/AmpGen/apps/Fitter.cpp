#include <iostream>
#include <fstream>
#include <math.h>
#include <dlfcn.h>
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TRandom3.h"
#include <complex>

#include "AmpGen/Particle.h"
#include "AmpGen/FastCoherentSum.h"
#include "AmpGen/FastIncoherentSum.h"
#include "AmpGen/Chi2Estimator.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/SumPDF.h"
#include "AmpGen/CoherenceFactor.h"
#include "AmpGen/Kinematics.h"
#include "AmpGen/Plots.h"
#include "AmpGen/FitResult.h"
#include "AmpGen/LatexTable.h"
#include "AmpGen/MintUtilities.h"
#include "AmpGen/IExtendLikelihood.h"

#include "TMatrixD.h"
#include "TVectorD.h"
#include "TRandom3.h"
#include "TDecompChol.h"
#include "TH2D.h"
#include "TCanvas.h"

typedef AmpGen::FCNLibrary pdfLib;

using namespace AmpGen;

void randomizeStartingPoint( MinuitParameterSet& MPS , TRandom3& rand, bool SplineOnly = false){

  double range = 5;
  for( unsigned int i = 0 ; i < MPS.size() ; ++i ){
    auto param = MPS.getParPtr(i);
    if( param->iFixInit() == 0 ){
      if( SplineOnly && param->name().find("::Spline::") == std::string::npos ) continue ; 
      range = param->maxInit() - param->minInit();
      MPS.getParPtr(i)->setInit( range*rand.Rndm() + param->meanInit() ) ;
      MPS.getParPtr(i)->print();
      std::cout << std::endl;
    }
  }
}


template<typename PDF > Minimiser* doFit( PDF& pdf , 
    EventList& data, 
    EventList& mc, 
    MinuitParameterSet& MPS ){

  const std::string fLib = NamedParameter<std::string>("Lib",std::string("")).getVal();
  const std::string logFile = NamedParameter<std::string>("LogFile",std::string("Fitter.log")).getVal();
  unsigned int nBins = NamedParameter<unsigned int>("nBins",100).getVal();
  bool debug = NamedParameter<unsigned int>("debug",0).getVal();

  std::vector<std::string> ll_config_terms = 
    getVectorArgument<std::string>("LLExtend",std::string(""));

  for( auto& ll_config : ll_config_terms ){
    auto ll_name = split( ll_config , ' ' )[0];
    auto ll_term = Factory<IExtendLikelihood>::get( ll_name );
    if( ll_term != 0 ){
      ll_term->configure( ll_config , MPS );
      pdf.addExtendedTerm( ll_term );
    } 
    else {
      ERROR("LL term : " << ll_name << " not recognised" );
    }
  }  

  auto time = std::clock();
  pdf.setPset( &MPS );
  pdf.buildLibrary();
  pdf.setEvents( data );
  pdf.setMC( mc );
  int options = fLib == "" ? FCNLibrary::OPTIONS::RECOMPILE | ( FCNLibrary::OPTIONS::DEBUG && debug ) : 0;

  pdf.link( options, fLib );

  if( debug ) pdf.debug();
  INFO("Fitting PDF with " << pdf.nPDFs() << " components");
  Minimiser* mini = new Minimiser(&pdf);
  mini->doFit(); 
  pdf.reweight( mc, data.size(), 1 );
  for( unsigned int i = 0 ; i < pdf.nPDFs() ;++i){
    INFO( "Making plots for category " << i );
    auto mc_plots = 
      mc.makePlots("MC_Category"+std::to_string(i+1)+"_",nBins, i+1);
    for( auto& plot : mc_plots ) plot->Write();
  }

  Chi2Estimator chi2(data, mc , 15 );

  MinuitParameterSet& mps = *(mini->parSet());
  FitResult fr( mps, std::get<0>(pdf.pdfs()).fitFractions( *mini  ), mini->covMatrixFull(),
      chi2.chi2(), chi2.nBins(), pdf.getVal(),  mini->GetStatus() );
  fr.writeToFile( logFile);
  
  INFO( "Time = " << (std::clock() - time )  / (double)CLOCKS_PER_SEC );
  return mini;
}

int main(int argc , char* argv[] ){

  std::string dataFile = NamedParameter<std::string>("SigFileName") ; 
  std::string mcFile   = NamedParameter<std::string>("SgIntegratorFname");
  std::string flatMC   = NamedParameter<std::string>("MCCoherence","NONE");

  std::string plotFile = NamedParameter<std::string>("Plots",(std::string)"plots.root");
  std::string latexOut = NamedParameter<std::string>("Plots",(std::string)"fit.tex");
  std::string epsFile  = NamedParameter<std::string>("EpsFile",(std::string)"default.eps");

  unsigned int nThreads    = NamedParameter<unsigned int>("nCores",1);
  unsigned int nBins       = NamedParameter<unsigned int>("nBins",100);
  unsigned int morePlots   = NamedParameter<unsigned int>("makeExtendedPlots",0);

  std::vector<std::string> evtType_particles = NamedParameter<std::string>("EventType").getVector();

  double fPDF     = NamedParameter<double>("fPDF",1.0);
  double fComb    = NamedParameter<double>("fComb",1.0);
  double fMisID   = NamedParameter<double>("fMisID",0.0);
  double prescale = NamedParameter<double>("Prescale",1.0).getVal();

  /// coherence factor ///
  double globalPhase      = NamedParameter<double>     ("Coherence::GlobalPhase",2.967);
  std::string binningName = NamedParameter<std::string>("Coherence::BinningName",std::string(""));
  unsigned int co_nBins   = NamedParameter<unsigned int>("Coherence::nBins"      ,4);
  unsigned int seed       = NamedParameter<unsigned int>("Seed",0);
  TRandom3* rndm = new TRandom3();
  rndm->SetSeed( seed );

  omp_set_num_threads( nThreads );
  omp_set_dynamic(0);

  MinuitParameterSet MPS = MPSFromStream();
  EventType evtType( evtType_particles );

  EventList events ( dataFile , evtType , MPS.size() / 2 ) ;

  FastCoherentSum pdf( evtType 
      , MPS 
      , events.getExtendedEventFormat()
      , ""
      , FastCoherentSum::OPTIONS::DEBUG  );

  FastIncoherentSum bkg( evtType
      , MPS
      , events.getExtendedEventFormat()
      , "Inco"
      , FastCoherentSum::OPTIONS::DEBUG );

  FastCoherentSum misID( evtType
      , MPS
      , events.getExtendedEventFormat()
      , "MisID"
      , FastCoherentSum::OPTIONS::DEBUG );

  if( ! pdf.isStateGood() || ! bkg.isStateGood() || ! misID.isStateGood() ){
    ERROR("Amplitude incorrectly configured");
    return -1;
  };

  pdf.setWeight( fPDF );
  bkg.setWeight( fComb ); 
  misID.setWeight( fMisID );
  TTree* tree_mc_events = (TTree*)TFile::Open( mcFile.c_str(), "READ")->Get("DalitzEventList") ;
  if( tree_mc_events == nullptr ){
    ERROR("Could not find : " << mcFile << ".DalitzEventList" );
    return -1;
  }
  EventList eventsMC( tree_mc_events , evtType , MPS.size() / 2 ,
      [&rndm,&prescale](const Event&){return prescale==1 || rndm->Uniform(0,1) < prescale; }  );

  SumPDF<FastCoherentSum&> signalPDF( pdf ); /// PURE signal pdf

  SumPDF<FastIncoherentSum&> bkgPDF( bkg );
  SumPDF<FastCoherentSum&, FastIncoherentSum&> 
    signalAndOneBackground( pdf, bkg );

  SumPDF<FastCoherentSum&, FastIncoherentSum&, FastCoherentSum&> 
    signalAndTwoBackground( pdf, bkg, misID );

  TFile* output = TFile::Open(plotFile.c_str(),"RECREATE");
  output->cd();

  Minimiser* mini = 0 ;

  if( fPDF == 1.0 ){
    INFO("Fitting with single background");
    mini = doFit( signalPDF, events, eventsMC, MPS  );
  }
  else if( fPDF == 0.0 && fComb == 1.0 ){
    INFO("Fitting pure combinatoric background" ); 
    mini = doFit( bkgPDF, events, eventsMC, MPS  );
  }
  else if( fMisID == 0 ){
    INFO("Fitting with one background");
    mini = doFit( signalAndOneBackground, events, eventsMC, MPS  );
  }
  else if( fMisID != 0  ) {
    mini = doFit( signalAndTwoBackground, events, eventsMC, MPS );
  }

  int status = mini->GetStatus();
  if( status != 0 ){
    ERROR("Fit not converged!");
  }
  INFO("Completed fit");

  /// From here is just making plots and finalising the output //// 
  output->cd();

  std::vector<TH1D*> plots = events.makePlots("Data_", nBins );
  for( auto& plot : plots ) plot->Write();

  if( morePlots ){
    unsigned int nBinsReduced = NamedParameter<unsigned int>("nBinsReduced",50).getVal();
    auto kpi_mid   = [](const Event& evt){ return fabs( sqrt( evt.s({0,1}) ) - 897.6 ) < 75;  } ;
    auto pipi_mid  = [](const Event& evt){ return fabs( sqrt( evt.s({2,3}) ) - 770. ) < 100; } ;
    auto kpi_high  = [](const Event& evt){ return evt.s({0,1}) > 1100*1100;  } ;
    auto pipi_high = [](const Event& evt){ return evt.s({2,3}) > 1000.*1000.; } ;
    auto kpi_low   = [](const Event& evt){ return evt.s({0,1}) < 1200.*1200.; } ;
    auto pipi_low  = [](const Event& evt){ return evt.s({2,3}) < 550*550; };
    auto no_cut    = [](const Event& evt){ return 1; };
    auto kstarrho_window = [&kpi_mid,&pipi_mid](const Event& evt){ return kpi_mid(evt) && pipi_mid(evt) ; };

    auto kstar_hcos = HelicityCosine(0,3,{1,0});
    auto rho_hcos   = HelicityCosine(1,2,{2,3}) ;
    auto kpi_axis   = plotAxis("cos(#theta_{K#pi})",-1.0,1.0,nBinsReduced);
    auto rho_axis   = plotAxis("cos(#theta_{#pi#pi})",-1.0,1.0,nBinsReduced);
    auto triple_axis = plotAxis("#phi [rads]",-1.0,1.0,nBinsReduced);
    auto aco_axis   = plotAxis( "#chi [rads]",0,M_PI*2,nBinsReduced);

    plot1D( events, eventsMC, kstar_hcos   , kpi_mid          , pdf , kpi_axis  , "hCos_kpi_mid"  ) ;
    plot1D( events, eventsMC, rho_hcos     , pipi_mid         , pdf , rho_axis  , "hCos_pipi_mid"  )  ;
    plot1D( events, eventsMC, acoplanarity , kstarrho_window  , pdf , aco_axis  , "aco_kstarrho" ) ;

    plot1D( events, eventsMC, kstar_hcos   , kpi_high         , pdf , kpi_axis  , "hCos_kpi_high"  ) ;
    plot1D( events, eventsMC, kstar_hcos   , no_cut           , pdf , kpi_axis  , "hCos_kpi_all" ) ;
    plot1D( events, eventsMC, rho_hcos     , no_cut           , pdf , rho_axis  , "hCos_pipi_all" ) ;

    plot1D( events, eventsMC, TripleProduct , kpi_mid         , pdf , triple_axis  , "tp_kpi_mid"  ) ;
    plot1D( events, eventsMC, TripleProduct , pipi_mid        , pdf , triple_axis  , "tp_pipi_mid"  ) ;
    plot1D( events, eventsMC, TripleProduct , kstarrho_window , pdf , triple_axis  , "tp_kstarrho" ) ;
    plot1D( events, eventsMC, TripleProduct , kpi_high        , pdf , triple_axis  , "tp_kpi_high"  ) ;

    /*
    auto defaultAxes = eventsMC.defaultProjections();

    for( unsigned int i = 0 ; i < defaultAxes.size(); ++i ){
      auto axis = defaultAxes[i];
      auto sij = [&axis]( const Event& evt){ 
        //INFO( "returning : " << evt.s( axis.indices ) / (1000.*1000.) );
        return evt.s( axis.indices) / (1000.*1000.) ; } ;
      gFile->cd();
      makePerAmplitudePlot( eventsMC, pdf ,sij, no_cut, axis, "MC_"+axis.name+"_allAmps");

      for( unsigned int j = i +1 ; j < defaultAxes.size(); ++j){
        auto& yAxis = defaultAxes[j];
        auto s2 = [&yAxis]( const Event& evt){
          return evt.s( yAxis.indices) / (1000.*1000.) ; } ;

        makePerAmplitudePlot2D( eventsMC, bkg, sij, s2, no_cut, axis, 
          yAxis, "MC_" + axis.name + "_"+yAxis.name +"_allAmps");
      };

      plot1D( events, eventsMC, sij,  kpi_mid  , pdf , axis , axis.name+"_kpi_mid" ) ;
      plot1D( events, eventsMC, sij,  pipi_mid , pdf, axis , axis.name+"_pipi_mid"  ) ;
      plot1D( events, eventsMC, sij,  kpi_high , pdf, axis , axis.name+"_kpi_high"  ) ;
      plot1D( events, eventsMC, sij,  pipi_high, pdf, axis , axis.name+"_pipi_high"  ) ;
      plot1D( events, eventsMC, sij,  pipi_low , pdf, axis , axis.name+"_pipi_low"  ) ;
      plot1D( events, eventsMC, sij,  kpi_low  , pdf, axis , axis.name+"_kpi_low"  ) ;
    }
    */
  }

  mini->covMatrixFull().Write();
  mini->covMatrix().Write();
  if( mini == 0 ){   
    return 0 ; 
  }

  if( flatMC != "NONE" ){

    EventList flatEvts( flatMC , evtType , MPS.size() / 2  );
    signalAndTwoBackground.setMC( flatEvts );
    signalAndTwoBackground.getVal();

    CoherenceFactor rk3pi( &flatEvts, &pdf, &misID );
    rk3pi.setGlobalPhase( globalPhase );
    rk3pi.makeCoherentMapping( co_nBins ) ;
    rk3pi.writeToFile("test.dat");
    rk3pi.getNumberOfEventsInEachBin( events );

    output->cd();
  };


  output->Write();
  output->Close();
  INFO("Finalising output");

  return 0;

}
