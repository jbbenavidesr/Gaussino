#include "Defaults/HepMCAttributes.h"
#include "HepMC/Attribute.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/ReaderRoot.h"
#include "HepMC/ReaderAscii.h"
#include "HepMC/ReaderRootTree.h"
#include "HepMCUtils/CompareGenEvent.h"

int main( int, char* argv[] )
{
  HepMC::ReaderRootTree reader1( argv[1] );
  HepMC::ReaderRootTree reader2( argv[2] );
  std::map<std::pair<int, int>, HepMC::GenEvent*> events1;

  while ( true ) {
    auto evt = new HepMC::GenEvent{};
    reader1.read_event( *evt );
    if ( reader1.failed() ) {
      break;
    }
    int eventNumber = evt->attribute<HepMC::IntAttribute>( Gaussino::HepMC::Attributes::GaudiEventNumber )->value();
    auto key        = std::make_pair( eventNumber, evt->event_number() );
    events1[key]    = evt;
  }

  std::cout << "Read in events for eventnumber matching" << std::endl;

  unsigned int all{};
  unsigned int passed{};
  int code{0};
  while ( true ) {
    HepMC::GenEvent evt2;
    reader2.read_event( evt2 );
    if ( reader2.failed() ) {
      break;
    }
    auto eventNumber = evt2.attribute<HepMC::IntAttribute>( Gaussino::HepMC::Attributes::GaudiEventNumber )->value();

    auto key = std::make_pair( eventNumber, evt2.event_number() );
    if ( events1.find( key ) != std::end( events1 ) ) {
      auto evt1 = events1[key];
      if ( !HepMC::compareGenEvent( *evt1, evt2 ) ) {
        std::cerr << "Failure: Event not equal #" << eventNumber << ", " << evt2.event_number() << ". Abort!"
                  << std::endl;
        //std::cout << "Printing the entire events:"
                  //<< "\n";
        //std::cout << "From first file:" << std::endl;
        //HepMC::printChildren( evt1->particles()[0] );
        //std::cout << "From second file:" << std::endl;
        //HepMC::printChildren( evt2.particles()[0] );
        // return 1;
        std::cout << std::endl;
        code = 3;
      } else {
        passed++;
      }
      all++;
    }
  }
  std::cout << "Fraction of equal events: " << passed << "/" << all <<": " << ( (float)passed * 100. ) / all << "%.\n";
  reader1.close();
  reader2.close();

  return code;
}
