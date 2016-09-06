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
        std::string( getenv("PWD") ) + std::string( "/functions") ) ){
    ERROR("Library linking / creation failed, exiting");
    return 0 ;
  }
  Generator GENERATOR( sig, eventType );
  TRandom3 rnd;
  GENERATOR.setRandom( &rnd );
  GENERATOR.fillEventList( accepted, AmpGen::NamedParameter<double>("NEvents",10000).getVal() );
  TTree* tree = accepted.tree("DalitzEventList");
 
  auto plots = accepted.makePlots();
  TFile* f = TFile::Open("output_plots.root","RECREATE");
  f->cd();
  tree->Write();
  
  f->Close();
}
