#include <iostream>
#include <fstream>
#include <math.h>
#include <dlfcn.h>
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TRandom3.h"
#include <chrono>
#include <complex>
#include "TCanvas.h"

#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/ParsedParameterFile.h"
#include "AmpGen/ParsedParameterFileList.h"
#include "AmpGen/FitParameter.h"
#include "AmpGen/Minimiser.h"

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
#include "AmpGen/MintUtilities.h"

#include "rootLogin.h"

typedef AmpGen::FCNLibrary<std::complex<double>> pdfLib;

std::vector<double> vectorFromLorentz( TLorentzVector* v ){
  return { 1000*v->X() , 1000*v->Y() , 1000*v->Z() , 1000*v->T() };
}

void drawInValidationStyle(const std::string& pName ){
  TH1D* mint_plot = (TH1D*)gFile->Get( (pName +"_Mint3").c_str());
  TH1D*  qft_plot = (TH1D*)gFile->Get( (pName +"_QFT").c_str());
  mint_plot->SetLineColor(kRed);
  mint_plot->SetFillColor(kRed);
  mint_plot->SetFillStyle(3001);
  mint_plot->GetYaxis()->SetTitle("Entries");
  mint_plot->SetStats(0);
  mint_plot->SetMinimum(0);
  INFO("Drawing AmpGen plot");
  mint_plot->Draw("C HIST");
  INFO("Drawing QFT++ plot");
  qft_plot->Draw("E same");
}


using namespace AmpGen;

int main(int argc , char* argv[] ){

  rootLogin(); 
  TFile* f = TFile::Open(   AmpGen::NamedParameter<std::string>("SgIntegratorFname").getVal().c_str() );
  TTree* tree = (TTree*)f->Get("DalitzEventList");

  AmpGen::MinuitParameterSet MPS = MPSFromStream();
  EventType evtType( "D0", {"K-","pi+","pi+","pi-"} );

  EventList events(tree, evtType , MPS.size() / 2 );

  FastIncoherentSum pdf( evtType , MPS , events.getExtendedEventFormat(),  "", true);
  SumPDF<std::complex<double>, FastIncoherentSum&> signalPDF( pdf ); /// PURE signal pdf

  signalPDF.compile( "/tmp/LorentzTest" );
  signalPDF.link( pdfLib::OPTIONS::DEBUG | pdfLib::OPTIONS::RECOMPILE , "/tmp/LorentzTest");

  signalPDF.setEvents( events ); 
  signalPDF.setMC( events );
  signalPDF.debug();

  std::string qft_file = AmpGen::NamedParameter<std::string>("VerificationFile",(std::string)"");
  TFile* qft = TFile::Open(qft_file.c_str());
  EventList validationEvents( (TTree*)qft->Get("DalitzEventList") , evtType, 0 ) ;

  auto plots= validationEvents.makePlots(); 
  std::string plot_file = NamedParameter<std::string>("PlotFile",(std::string)"");

  TFile* outputPlots = TFile::Open(plot_file.c_str(),"RECREATE");
  for( auto& p : plots ) p->Write();
  signalPDF.reweight( events, validationEvents.size(), 1 );
  auto otherPlots = events.makePlots("MC",100,1); 
  for( auto& p : otherPlots ) p->Write();
  auto hcos1 = HelicityCosine(0,3,{1,0});
  auto hcos2 = HelicityCosine(1,2,{2,3}) ;
  auto hcos3 = HelicityCosine(0,2,{0,1,3});
  auto hcos4 = HelicityCosine(0,2,{1,2,3});

  unsigned int nBinsReduced = 50;

  auto kpi_axis   = plotAxis("cos(#theta_{K#pi})",-1.0,1.0,nBinsReduced);
  auto rho_axis   = plotAxis("cos(#theta_{#pi#pi})",-1.0,1.0,nBinsReduced);
  auto kpipi_aaxis = plotAxis("cos(#theta_{K#pi#pi})",-1.0,1.0,nBinsReduced);
  auto pipipi_aaxis = plotAxis("cos(#theta_{#pi#pi#pi})",-1.0,1.0,nBinsReduced);
  auto triple_axis = plotAxis("cos(#phi)",-1.0,1.0,nBinsReduced);
  auto aco_axis   = plotAxis( "#chi [rads]",0,M_PI,nBinsReduced);

  auto no_cut          = [](auto& evt){ return 1; };
  plot1D( events,           TripleProduct, no_cut, triple_axis, "TP_Mint3"  ) ;
  plot1D( validationEvents, TripleProduct, no_cut, triple_axis, "TP_QFT");

  plot1D( events,           hcos1, no_cut, kpi_axis, "hCos1_Mint3"  ) ;
  plot1D( validationEvents, hcos1, no_cut, kpi_axis, "hCos1_QFT");

  plot1D( events,           hcos2, no_cut, rho_axis, "hCos2_Mint3"  ) ;
  plot1D( validationEvents, hcos2, no_cut, rho_axis, "hCos2_QFT");

  plot1D( events,           acoplanarity, no_cut, aco_axis, "chi_Mint3"  ) ;
  plot1D( validationEvents, acoplanarity, no_cut, aco_axis, "chi_QFT");

  plot1D( events,           hcos3, no_cut, rho_axis, "hCos3_Mint3"  ) ;
  plot1D( validationEvents, hcos3, no_cut, rho_axis, "hCos3_QFT");

  plot1D( events,           hcos4, no_cut, rho_axis, "hCos4_Mint3"  ) ;
  plot1D( validationEvents, hcos4, no_cut, rho_axis, "hCos4_QFT");

  Chi2Estimator chi2(validationEvents, events , 50, 100);

  INFO( "Chi2 per bin = " << chi2.chi2() / chi2.nBins()  );

  outputPlots->Close();
}
