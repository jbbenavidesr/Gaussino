#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TRandom3.h"
#include "TGenPhaseSpace.h"
#include <complex>
#include <chrono>
#include <omp.h>
#include <algorithm>
#include "TCanvas.h"

#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/ParsedParameterFile.h"
#include "AmpGen/SumPDF.h"
#include "AmpGen/FastCoherentSum.h"
#include "AmpGen/EventType.h"

#include "AmpGen/MintUtilities.h"
#include "AmpGen/Generator.h"
#include "AmpGen/EventType.h"


using namespace AmpGen;

int main( int /*argc */, char** /*argv*/ ){

  /// first estimate normalisation constant //// 

  INFO("Integral Test application");

  AmpGen::MinuitParameterSet MPS = MPSFromStream(); 

  EventType eventType( 
      AmpGen::NamedParameter<std::string>("Mother") 
      , AmpGen::NamedParameter<std::string>("EventType").getVector()  );

  EventList accepted( eventType );

  FastCoherentSum sig( eventType , MPS , accepted.getExtendedEventFormat(),  "", true);

  SumPDF<std::complex<double>, FastCoherentSum&> pdf( sig ); /// PURE signal pdf
  pdf.setPset( &MPS );
  pdf.buildLibrary();
  typedef FCNLibrary<std::complex<double>> pdfLib;

  if( ! pdf.link( pdfLib::OPTIONS::RECOMPILE | pdfLib::OPTIONS::DEBUG, 
        std::string( getenv("MINTDIR") ) + std::string( "/src/Users/Tim/ToyMCGenerator/functions") ) ){
    ERROR("Library linking / creation failed, exiting");
    return 0 ;
  }
  Generator gen( sig, eventType );
  TRandom3 rnd;
  gRandom = &rnd;
  gen.setRandom( &rnd );
  INFO("Libraries ready - doing integral test: with 100 events");
  TH1D* hist = new TH1D("hist","",98,104,50);
  for( unsigned int i = 0 ; i < 500 ; ++i ){
    INFO("Test #" << i );
    EventList evts( eventType );
    gen.fillEventListPhaseSpace( evts, 400000 );  
    sig.setMC( evts );
    sig.prepare();
    hist->Fill( std::real( sig.norm(0,0) ) );
  };

   
  TFile* f = TFile::Open("test.root","RECREATE");

  hist->Write();
  f->Close();
  
};
