#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/NamedParameter.h"
#include "AmpGen/FitParameter.h"
#include "AmpGen/Minimiser.h"
#include "AmpGen/SumPDF.h"
#include "AmpGen/FastCoherentSum.h"
#include "AmpGen/EventType.h"
#include "AmpGen/EventType.h"
#include "AmpGen/MintUtilities.h"

using namespace AmpGen;

int main( int argc , char** argv ){

  
  EventType eventType( NamedParameter<std::string>("EventType").getVector()  );
  

  AmpGen::MinuitParameterSet MPS = MPSFromStream();

  FastCoherentSum sig( eventType , MPS , eventType.getEventFormat() ,  "", true);

  SumPDF<std::complex<double>, FastCoherentSum&> pdf( sig ); /// PURE signal pdf

  pdf.setPset( &MPS );
  pdf.buildLibrary();
  sig.makeBinary( NamedParameter<std::string>("sourceFile",std::string("test.cpp")) );

}
