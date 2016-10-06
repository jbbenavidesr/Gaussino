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

  AmpGen::MinuitParameterSet MPS = MPSFromStream();

  std::vector<std::string>  eventTypeNames = NamedParameter<std::string>("EventType").getVector();
  unsigned int nCores  = NamedParameter<unsigned int>("nCores",16); 
  unsigned int nEvents = NamedParameter<double>("NEvents",10000);
  unsigned int useRoot = NamedParameter<unsigned int>("useRoot",0);
  std::string output   = NamedParameter<std::string>("OutputFile",std::string("output.root") );
  
  omp_set_num_threads(nCores );
  omp_set_dynamic(0);

  EventType eventType(  eventTypeNames  );

  EventList accepted( eventType );

  FastCoherentSum sig( eventType , MPS , accepted.getExtendedEventFormat(),  "", true);

  SumPDF<std::complex<double>, FastCoherentSum&> pdf( sig ); /// PURE signal pdf
  pdf.setPset( &MPS );
  pdf.buildLibrary();
  typedef FCNLibrary<std::complex<double>> pdfLib;

  if( ! pdf.link( pdfLib::OPTIONS::RECOMPILE | pdfLib::OPTIONS::DEBUG, 
        std::string( getenv("PWD") ) + std::string( "/functions") ) ){
    ERROR("Library linking / creation failed, exiting");
    return 0 ;
  }
  Generator signalGenerator( sig, eventType );
  TRandom3 rnd;
  
  signalGenerator.setRandom( &rnd );
  signalGenerator.fillEventList( accepted, nEvents, []( auto& evt ){ return evt.s(0,1) > 1000*1000 ; }  );
  
  INFO("Making output files");
  TFile* f = TFile::Open( output.c_str(),"RECREATE");
  f->cd();
  accepted.tree("DalitzEventList")->Write();
  
  auto plots = accepted.makePlots();
  for( auto& plot : plots ) plot->Write();
  auto plots2d = accepted.makePlots2D("",0,50); 
  for( auto& plot2d : plots2d ) plot2d->Write();
  INFO("Writing output file ");  

  f->Close();
}
