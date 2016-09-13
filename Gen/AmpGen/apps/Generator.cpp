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

  omp_set_num_threads(NamedParameter<unsigned int>("nCores",16).getVal() );
  omp_set_dynamic(0);

  EventType eventType( 
    NamedParameter<std::string>("Mother") 
  , NamedParameter<std::string>("EventType").getVector()  );

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
  Generator GENERATOR( sig, eventType );
  TRandom3 rnd;
  GENERATOR.setRandom( &rnd );
  GENERATOR.fillEventList( accepted, 
      NamedParameter<double>("NEvents",10000).getVal() ,
      NamedParameter<int>("useRoot",0).getVal() );
  
  INFO("Making output files");
  TFile* f = TFile::Open("output_plots.root","RECREATE");
  f->cd();
  TTree* tree = accepted.tree("DalitzEventList");
  tree->Write();
  auto plots = accepted.makePlots();
  for( auto& plot : plots ) plot->Write();
  INFO("Writing output file ");  
  f->Close();
}
