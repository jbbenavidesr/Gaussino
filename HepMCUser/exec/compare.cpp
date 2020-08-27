#include "Defaults/HepMCAttributes.h"
#include "HepMC3/Attribute.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/ReaderRoot.h"
#include "HepMC3/ReaderRootTree.h"
#include "HepMCUtils/CompareGenEvent.h"

int main( int, char* argv[] )
{
  HepMC3::Reader* reader1, *reader2;
  if(std::string(argv[1]).find(".txt") != std::string::npos){
      reader1 = new HepMC3::ReaderAscii(argv[1]);
  } else {
      reader1 = new HepMC3::ReaderRootTree(argv[1]);
  }
  if(std::string(argv[2]).find(".txt") != std::string::npos){
      reader2 = new HepMC3::ReaderAscii(argv[2]);
  } else {
      reader2 = new HepMC3::ReaderRootTree(argv[2]);
  }
  //reader1 = new HepMC3::ReaderAscii(argv[1]);
  //reader2 = new HepMC3::ReaderAscii(argv[2]);
  std::map<std::pair<int, int>, HepMC3::GenEvent*> events1;

  while ( true ) {
    auto evt = new HepMC3::GenEvent{};
    reader1->read_event( *evt );
    if ( reader1->failed() ) {
      break;
    }
    int eventNumber = evt->attribute<HepMC3::IntAttribute>( Gaussino::HepMC::Attributes::GaudiEventNumber )->value();
    auto key        = std::make_pair( eventNumber, evt->event_number() );
    events1[key]    = evt;
  }

  std::cout << "Read in events for eventnumber matching" << std::endl;

  unsigned int all{};
  unsigned int passed{};
  int code{0};
  while ( true ) {
    HepMC3::GenEvent evt2;
    reader2->read_event( evt2 );
    if ( reader2->failed() ) {
      break;
    }
    auto eventNumber = evt2.attribute<HepMC3::IntAttribute>( Gaussino::HepMC::Attributes::GaudiEventNumber )->value();

    auto key = std::make_pair( eventNumber, evt2.event_number() );
    if ( events1.find( key ) != std::end( events1 ) ) {
      auto evt1 = events1[key];
      if ( !HepMC3::compareGenEvent( *evt1, evt2 ) ) {
        std::cerr << "Failure: Event not equal #" << eventNumber << ", " << evt2.event_number() << ". Abort!"
                  << std::endl;
        //std::cout << "Printing the entire events:"
                  //<< "\n";
        //std::cout << "From first file:" << std::endl;
        //HepMC3::printChildren( evt1->particles()[0] );
        //std::cout << "From second file:" << std::endl;
        //HepMC3::printChildren( evt2.particles()[0] );
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
  reader1->close();
  reader2->close();
  delete reader1;
  delete reader2;

  return code;
}
