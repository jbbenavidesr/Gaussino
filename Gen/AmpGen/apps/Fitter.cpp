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
#include "AmpGen/Binning.h"
#include "AmpGen/Chi2Estimator.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/SumPDF.h"
#include "AmpGen/CoherenceFactor.h"
#include "AmpGen/Kinematics.h"
#include "AmpGen/Plots.h"
#include "AmpGen/LatexTable.h"
#include "AmpGen/MintUtilities.h"

#include "TMatrixD.h"
#include "TVectorD.h"
#include "TRandom3.h"
#include "TDecompChol.h"
#include "TH2D.h"
#include "TCanvas.h"

typedef AmpGen::FCNLibrary<std::complex<double>> pdfLib;

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


template<typename PDF >
Minimiser* doFit( PDF& pdf , 
    EventList& data, 
    EventList& mc, 
    std::ofstream& outlog, 
    MinuitParameterSet& MPS,
    FitQuality& fq
 ){

  const std::string fLib = NamedParameter<std::string>("Lib",(std::string)"functions").getVal();

  auto time = std::clock();
  pdf.setPset( &MPS );
  pdf.buildLibrary();
  pdf.setEvents( data );
  pdf.setMC( mc );

  if( fLib == "functions"){
    pdf.compile( "/tmp/MintFcnLib"+std::to_string(::getpid()));
    bool debug = NamedParameter<unsigned int>("debug",1).getVal();
    pdf.link( pdfLib::OPTIONS::RECOMPILE | ( pdfLib::OPTIONS::DEBUG && debug ) , 
      "/tmp/MintFcnLib"+std::to_string(::getpid()) );
    if( debug )
      pdf.debug(); 
  }
  else {   
    pdf.link( 0 , fLib );
  }
  INFO("Fitting PDF with " << pdf.nPDFs() << " components");
  Minimiser* mini = new Minimiser(&pdf);
  mini->doFit(); 
  pdf.reweight( mc, data.size(), 1 );
  unsigned int nBins = NamedParameter<unsigned int>("nBins",100).getVal();
  INFO("Making plots for " << pdf.nPDFs() << " categories" );
  for( unsigned int i = 0 ; i < pdf.nPDFs() ;++i){
    std::cout << "Making plots for category " << i << std::endl; 
    auto mc_plots = 
      mc.makePlots("MC_Category"+std::to_string(i+1)+"_",nBins, i+1);
    for( auto& plot : mc_plots ) plot->Write();
  }

  Chi2Estimator chi2(data, mc , 10, 30);

  TMatrixTSym<double> cov = mini->covMatrixFull();
  outlog << "Covariance Matrix:" << std::endl;
  std::vector<std::string> params;
  MinuitParameterSet& mps = *(mini->parSet());
  unsigned int nParam=0;
  for( unsigned int i = 0 ; i < mps.size(); ++i){
    if( mps.getParPtr(i)->iFixInit() == 0 ) nParam++;
    params.push_back( mps.getParPtr(i)->name() );
  }

  for(int i = 0 ; i < cov.GetNrows(); ++i){
    outlog << params[i]                << "  " 
      << mps.getParPtr(i)->mean() << "  " 
      << mps.getParPtr(i)->err()  << "  ";
    for(int j = 0 ; j < cov.GetNcols(); ++j)
      outlog << cov[i][j] << " ";
    outlog << std::endl;
  }

  INFO( "Chi2 per bin = " << chi2.chi2() / chi2.nBins()  );
  INFO( "Chi2 per dof = " << chi2.chi2() / (chi2.nBins() - nParam-1 ) );

  fq.set( chi2.chi2() / ( chi2.nBins() - nParam-1 ) , pdf.getVal(), ( chi2.nBins() - nParam-1 ) );
  outlog <<
    "End Covariance Matrix\n" <<
    "Chi2 per bin : " << chi2.chi2() / chi2.nBins() << std::endl <<
    "Chi2 per dof : " << chi2.chi2() / ( chi2.nBins() - nParam-1 ) << std::endl <<
    "Likliehood   : " << pdf.getVal() << std::endl;
  INFO( " & " << chi2.chi2() / (chi2.nBins() - nParam-1 ) << " & " << chi2.nBins() - nParam -1 << " & " << pdf.getVal() );
  INFO( "Time = " << (std::clock() - time )  / (double)CLOCKS_PER_SEC );
  return mini;
}

int main(int argc , char* argv[] ){

  //rootLogin();
  std::string fname = NamedParameter<std::string>("SigFileName").getVal();
  NamedParameter<std::string> SgIntegratorFname("SgIntegratorFname");
  std::string logFile  = NamedParameter<std::string>("LogFile",(std::string)"Fitter.log").getVal();
  std::string plotFile = NamedParameter<std::string>("Plots",(std::string)"plots.root").getVal();

  omp_set_num_threads( NamedParameter<unsigned int>("nCores",1).getVal() );
  omp_set_dynamic(0);

  MinuitParameterSet MPS = MPSFromStream();
  EventType evtType( "D0", NamedParameter<std::string>("EventType").getVector()  );

  if( argc == 2 ){
    unsigned int n = atoi(argv[1]);
    TRandom3* rand  = new TRandom3();
    INFO("Set Seed = " << n );
    
    rand->SetSeed( n );
    gRandom = rand; 
   // randomizeStartingPoint( MPS, rand , 
  //      NamedParameter<int>("RandomiseSpline",0).getVal() 
  //   );
    //auto logTokens = split( logFile, '.');
    logFile = logFile + "." + argv[1]; // logTokens[0]+argv[1]+"."+logTokens[1];
    auto nameTokens = split( fname, '.');
    fname = nameTokens[0] + argv[1] +".root";
    INFO("Writing to log = " << logFile );
    //auto plotTokens = split( plotFile, '.');
    //plotFile = plotTokens[0]+argv[1]+"."+plotTokens[1];
    INFO("Writing plots = " << plotFile );
  }
  INFO("Reading file " << fname );
  TFile* f = TFile::Open( fname.c_str() );
  if( f == 0 ) ERROR("Filename " << fname << " not found" );
  TTree* tree = (TTree*)f->Get("DalitzEventList");
  EventList events (tree, evtType , MPS.size() / 2 ) ;

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

  double fPDF = NamedParameter<double>("fPDF",1.0);
  double fComb = NamedParameter<double>("fComb",1.0);
  double fMisID = NamedParameter<double>("fMisID",0.0);
  pdf.setWeight( fPDF );
  bkg.setWeight( fComb ); 
  misID.setWeight( fMisID );
  TFile* mc = TFile::Open(SgIntegratorFname.getVal().c_str());
  TTree* tmc = (TTree*)mc->Get("DalitzEventList"); 
  
  double prescale = NamedParameter<double>("Prescale",1.0).getVal();
  int changeLabel = NamedParameter<int>("ChangeMCLabel",(int)0).getVal();
  EventList eventsMC(tmc, evtType , MPS.size() / 2  , changeLabel, prescale );

  SumPDF<std::complex<double>, FastCoherentSum&> signalPDF( pdf ); /// PURE signal pdf

  SumPDF<std::complex<double>, FastIncoherentSum&> bkgPDF( bkg );
  SumPDF<std::complex<double>, FastCoherentSum&, FastIncoherentSum&> 
    signalAndOneBackground( pdf, bkg );

  SumPDF<std::complex<double>, FastCoherentSum&, FastIncoherentSum&, FastCoherentSum&> 
    signalAndTwoBackground( pdf, bkg, misID );

  std::ofstream logstream;
  logstream.open( logFile );
  TFile* output = TFile::Open(plotFile.c_str(),"RECREATE");
  output->cd();
  FitQuality fq(0,0,0);

  Minimiser* mini = 0 ;
  if( fPDF == 1.0 ){
    INFO("Fitting with single background");
    mini = doFit( signalPDF, events, eventsMC, logstream, MPS , fq );
  }
  else if( fPDF == 0.0 && fComb == 1.0 ){
    INFO("Fitting pure combinatoric background" ); 
    mini = doFit( bkgPDF, events, eventsMC, logstream, MPS ,fq );
  }
  else if( fMisID == 0 ){
    INFO("Fitting with one background");
    mini = doFit( signalAndOneBackground, events, eventsMC, logstream, MPS ,fq );
  }
  else if( fMisID != 0  ) {
    mini = doFit( signalAndTwoBackground, events, eventsMC, logstream, MPS, fq );
  }
  int status = mini->GetStatus();
   logstream << "status " << status << std::endl; 
  if( status != 0 ){
    ERROR("Fit not converged!");
  };
  INFO("Completed fit");

  /// From here is just making plots and finalising the output //// 
  output->cd();

  unsigned int nBins=NamedParameter<unsigned int>("nBins",100).getVal();
  std::vector<TH1D*> plots = events.makePlots("Data_", nBins );
  for( auto& plot : plots ) plot->Write();
 // std::vector<TH2D*> plots2D = events.makePlots2D("Data_");
 // for( auto& plot : plots2D ) plot->Write();

  if( NamedParameter<int>("makeExtendedPlots",0).getVal() ){
    unsigned int nBinsReduced = NamedParameter<unsigned int>("nBinsReduced",50).getVal();
//  double norm = 0 ;
//  for( auto& evt : eventsMC ) norm += evt.weight();
//  norm /= eventsMC.size();  
    auto kpi_mid   = [](auto& evt){ return fabs( sqrt( evt.s({0,1}) ) - 897.6 ) < 75;  } ;
    auto pipi_mid     = [](auto& evt){ return fabs( sqrt( evt.s({2,3}) ) - 770. ) < 100; } ;
    auto kpi_high = [](auto& evt){ return evt.s({0,1}) > 1100*1100;  } ;
    auto pipi_high   = [](auto& evt){ return evt.s({2,3}) > 1000.*1000.; } ;
    auto kpi_low      = [](auto& evt){ return evt.s({0,1}) < 1200.*1200.; } ;
    auto pipi_low    = [](auto& evt){ return evt.s({2,3}) < 550*550; };
    auto no_cut          = [](auto& evt){ return 1; };
    auto kstarrho_window = [&kpi_mid,&pipi_mid](auto& evt){ return kpi_mid(evt) && pipi_mid(evt) ; };

    auto kstar_hcos = HelicityCosine(0,3,{1,0});
    auto rho_hcos   = HelicityCosine(1,2,{2,3}) ;
    auto kpi_axis   = plotAxis("cos(#theta_{K#pi})",-1.0,1.0,nBinsReduced);
    auto rho_axis   = plotAxis("cos(#theta_{#pi#pi})",-1.0,1.0,nBinsReduced);
    auto triple_axis = plotAxis("#phi [rads]",-1.0,1.0,nBinsReduced);
    auto aco_axis   = plotAxis( "#chi [rads]",0,M_PI*2,nBinsReduced);

    plot1D( events, kstar_hcos   , kpi_mid     , kpi_axis  , "Data_HelicityCosineKstar"  ) ;
    plot1D( events, rho_hcos     , pipi_mid       , rho_axis  , "Data_HelicityCosineRho"  )  ;
    plot1D( events, acoplanarity , kstarrho_window  , aco_axis  , "Data_Acoplanarity" ) ;
    
    plot1D( events, kstar_hcos   , kpi_high         ,  kpi_axis  , "Data_HelicityCosineKHighMass"  ) ;
    plot1D( events, kstar_hcos   , no_cut           , kpi_axis  , "Data_HCosKPi_NoCut" ) ;
    plot1D( events, rho_hcos     , no_cut           , rho_axis  , "Data_HCosPiPi_NoCut" ) ;

    plot1D( events, TripleProduct , kpi_mid          , triple_axis  , "Data_tripleProductKpi"  ) ;
    plot1D( events, TripleProduct , pipi_mid         , triple_axis  , "Data_tripleProductPiPi"  )  ;
    plot1D( events, TripleProduct , kstarrho_window  , triple_axis  , "Data_tripleProductKstarRho" ) ;
    plot1D( events, TripleProduct , kpi_high         , triple_axis  , "Data_tripleProductKpiMass"  ) ;


    makePerAmplitudePlot( eventsMC, pdf, kstar_hcos , kpi_mid , kpi_axis, "HelicityCosineKstar"     ); 
   
    makePerAmplitudePlot( eventsMC, pdf, rho_hcos  , pipi_mid , rho_axis, "HelicityCosineRho"          );
    makePerAmplitudePlot( eventsMC, pdf, kstar_hcos , kpi_high, kpi_axis, "HelicityCosineKHighMass");
    makePerAmplitudePlot( eventsMC, pdf, acoplanarity , kstarrho_window, aco_axis, "Acoplanarity"        );
    makePerAmplitudePlot( eventsMC, pdf, kstar_hcos     , no_cut , kpi_axis, "HCosKPi_NoCut"             );
    makePerAmplitudePlot( eventsMC, pdf, rho_hcos     , no_cut , rho_axis, "HCosPiPi_NoCut"              );

    makePerAmplitudePlot( eventsMC,pdf, TripleProduct , kpi_mid           , triple_axis, "tripleProductKpi"  ) ;
    makePerAmplitudePlot( eventsMC,pdf, TripleProduct , pipi_mid          , triple_axis, "tripleProductPiPi"  )  ;
    makePerAmplitudePlot( eventsMC,pdf, TripleProduct , kstarrho_window   , triple_axis, "tripleProductKstarRho" ) ;
    makePerAmplitudePlot( eventsMC,pdf, TripleProduct , kpi_high          , triple_axis, "tripleProductKpiMass"  ) ;

    makePerAmplitudePlot( eventsMC, bkg , kstar_hcos , kpi_mid , kpi_axis,     "Inco_HelicityCosineKstar"    ); 
    makePerAmplitudePlot( eventsMC, bkg , rho_hcos  , pipi_mid , rho_axis,        "Inco_HelicityCosineRho"      );
    makePerAmplitudePlot( eventsMC, bkg , kstar_hcos , kpi_high, kpi_axis,    "Inco_HelicityCosineKHighMass");
    makePerAmplitudePlot( eventsMC, bkg , acoplanarity , kstarrho_window, aco_axis, "Inco_Acoplanarity"           );
    makePerAmplitudePlot( eventsMC, bkg , kstar_hcos     , no_cut , kpi_axis,       "Inco_HCosKPi_NoCut"          );
    makePerAmplitudePlot( eventsMC, bkg , rho_hcos     , no_cut , rho_axis,         "Inco_HCosPiPi_NoCut"         );

  auto defaultAxes = eventsMC.defaultProjections();

  for( unsigned int i = 0 ; i < defaultAxes.size(); ++i ){
    auto axis = defaultAxes[i];
    auto sij = [&axis](auto& evt){ 
      //INFO( "returning : " << evt.s( axis.indices ) / (1000.*1000.) );
      return evt.s( axis.indices) / (1000.*1000.) ; } ;
    gFile->cd();
    makePerAmplitudePlot( eventsMC, pdf ,sij, no_cut, axis, "MC_"+axis.name+"_allAmps");
 
   for( unsigned int j = i +1 ; j < defaultAxes.size(); ++j){
    auto& yAxis = defaultAxes[j];
    auto s2 = [&yAxis](auto& evt){
              return evt.s( yAxis.indices) / (1000.*1000.) ; } ;
          
      makePerAmplitudePlot2D( eventsMC, bkg, sij, s2, no_cut, axis, yAxis, "MC_" + axis.name + "_"+yAxis.name +"_allAmps");
    };

    makePerAmplitudePlot( eventsMC, pdf, sij, kpi_mid, axis, "MC_"+axis.name+"_kpi_mid");
    plot1D( events,sij, kpi_mid, axis , "Data"+axis.name+"_kpi_mid" ) ;
    makePerAmplitudePlot( eventsMC, pdf, sij, pipi_mid, axis, "MC_"+axis.name+"_pipi_mid");
    plot1D( events,sij,  pipi_mid ,axis,  "Data"+axis.name+"_pipi_mid"  ) ;
    makePerAmplitudePlot( eventsMC, pdf, sij, kpi_high, axis,  "MC_"+axis.name+"_kpi_high" );
    plot1D( events,sij,  kpi_high ,axis,  "Data"+axis.name+"_kpi_high"  ) ;
    makePerAmplitudePlot( eventsMC, pdf, sij, pipi_high, axis, "MC_"+axis.name+"_pipi_high" );
    plot1D( events,sij,  pipi_high ,axis,  "Data"+axis.name+"_pipi_high"  ) ;
    makePerAmplitudePlot( eventsMC, pdf, sij, pipi_low, axis, "MC_"+axis.name+"_pipi_low" );
    plot1D( events,sij,  pipi_low ,axis,  "Data"+axis.name+"_pipi_low"  ) ;
    makePerAmplitudePlot( eventsMC, pdf, sij, kpi_low, axis, "MC_"+axis.name+"_kpi_low" );
    plot1D( events,sij,  kpi_low ,axis,  "Data"+axis.name+"_kpi_low"  ) ;


  }

  }

  mini->covMatrixFull().Write();
  mini->covMatrix().Write();
  if( mini == 0 ){   
    return 0 ; 
  }
  if( fComb == 1 ){
    std::vector<std::string> fitfractions = bkg.fitFractions( *mini , logstream ) ;
    std::string latexOutput = 
      NamedParameter<std::string>("LatexFile",(std::string)"fits.tex").getVal();
    LatexTable( latexOutput ).makeTable( bkg.decayTrees() , fitfractions, fq , true );
  }
  else  {
    std::vector<std::string> fitfractions = pdf.fitFractions( *mini , logstream );
    std::string latexOutput = 
      NamedParameter<std::string>("LatexFile",(std::string)"fits.tex").getVal();
    LatexTable( latexOutput ).makeTable( pdf.decayTrees() , fitfractions, fq );
  }
  std::string flatMC = 
    NamedParameter<std::string>("MCCoherence", (std::string)"NONE" ).getVal();
/*
  if( flatMC != "NONE" ){
  
    TFile* mc = TFile::Open(flatMC.c_str());
    TTree* tmc = (TTree*)mc->Get("DalitzEventList");
    EventList flatMC(tmc, evtType , MPS.size() / 2  );

    TH2D* coherence = new TH2D("coherenceFactor","coherenceFactor",200,0,1,100,-M_PI,M_PI);
    auto covariance = mini->covMatrix(); 
    TDecompChol decomposed( covariance );
    decomposed.Decompose();
    TMatrixD A = decomposed.GetU();
    std::vector<IMinuitParameter*> floatingParams;
    std::vector<double> initialValues;
    for( unsigned int i = 0 ; i < MPS.size(); ++i){
      if( MPS.getParPtr(i)->iFixInit() == 0 ){
        floatingParams.push_back( MPS.getParPtr(i) );
         initialValues.push_back( MPS.getParPtr(i)->mean() );
      };
    }
   
    signalAndTwoBackground.setMC( flatMC );
    signalAndTwoBackground.getVal();
    CoherenceFactor rk3pi( flatMC, &pdf, &misID );
    rk3pi.printCoherence( logstream ) ;
    //rk3pi.getFitFractions( *mini );
    //rk3pi.getNumberOfEventsInEachBin( events );
    
    
    TRandom3* randomMatrixErrorPropagator = new TRandom3();
    
    for( unsigned int i = 0 ; i < 10000 ; ++i ){
      GAUSSIAN_PERTURBATION( floatingParams, A, randomMatrixErrorPropagator );
      signalAndTwoBackground.getVal();
      auto coh = rk3pi.getGlobalCoherence(flatMC);
      coherence->Fill( std::abs(coh), std::arg(coh));
      INFO( "RK3pi = " << std::abs( coh ) << " delta = " << std::arg( coh ) ); 
      for( unsigned int i = 0 ; i < floatingParams.size() ; ++i ) 
        floatingParams[i]->setCurrentFitVal( initialValues[i] );
    }
    
    output->cd();
    coherence->Write(); 
  };
  */
  logstream << "End Log" << std::endl; 
  
  std::string epsFile = 
    NamedParameter<std::string>("EpsFile",(std::string)"default.eps").getVal();
//  TCanvas* c1 = MakePlots( output );
//  c1->SaveAs( epsFile.c_str() );
  output->Write();
  output->Close();
  logstream.close(); 
  INFO("Finalising output");

  return 0;

}
