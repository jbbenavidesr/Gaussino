#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/NamedParameter.h"
#include "AmpGen/FitParameter.h"
#include "AmpGen/Minimiser.h"
#include "AmpGen/SumPDF.h"
#include "AmpGen/FastCoherentSum.h"
#include "AmpGen/EventType.h"
#include "AmpGen/MintUtilities.h"
#include "AmpGen/PhaseSpace.h"
#include "AmpGen/Generator.h"

#include "TRandom3.h"

using namespace AmpGen;

int main( int argc , char** argv ){

  std::vector<std::string> oEventType 
                          = NamedParameter<std::string>("EventType").getVector();
  std::string sourceFile  = NamedParameter<std::string>("sourceFile");
  unsigned int NormEvents = NamedParameter<unsigned int>("NormEvents",1000000);
  double safetyFactor     = NamedParameter<double>("SafefyFactor",3);

  EventType eventType( oEventType );
  

  AmpGen::MinuitParameterSet MPS = MPSFromStream();

  FastCoherentSum sig( eventType , MPS , eventType.getEventFormat() ,  "", true);

  SumPDF<std::complex<double>, FastCoherentSum&> pdf( sig ); /// PURE signal pdf

  pdf.setPset( &MPS );
  pdf.buildLibrary();

  /// This is just to calculate the overall normalisation of the PDF 
  pdf.link();
  Generator phsp( sig, eventType );
  TRandom3 rnd;

  phsp.setRandom( &rnd );

  EventList phspEvents; 
  phsp.fillEventListPhaseSpace( phspEvents,NormEvents );
  sig.setEvents( phspEvents );
  sig.prepare();

  double pMax=0;
  for( auto& evt : phspEvents ){
    double n = std::norm( sig.getVal( evt ) );
    if( n > pMax ){
      pMax = n ;
    }
  } 
  sig.makeBinary( sourceFile, pMax * safetyFactor );
}
