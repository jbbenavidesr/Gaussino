#include "Defaults/HepMCAttributes.h"
#include "HepMC/Attribute.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/ReaderRootTree.h"
#include "HepMCUtils/CompareGenEvent.h"

int main( int, char* argv[] )
{
  HepMC::ReaderRootTree reader1( argv[1] );
  HepMC::ReaderRootTree reader2( argv[2] );
  std::map<std::pair<int, int>, HepMC::GenEvent> events1;

  while ( true ) {
    HepMC::GenEvent evt;
    reader1.read_event( evt );
    if ( reader1.failed() ) {
      break;
    }
    int eventNumber     = evt.attribute<HepMC::IntAttribute>( Gaussino::HepMC::Attributes::GaudiEventNumber )->value();
    auto key = std::make_pair(eventNumber, evt.event_number());
    events1[key] = evt;
  }

  std::cout << "Read in events for eventnumber matching" << std::endl;

  while ( true ) {
    HepMC::GenEvent evt2;
    reader2.read_event( evt2 );
    if ( reader2.failed() ) {
      break;
    }
    auto eventNumber = evt2.attribute<HepMC::IntAttribute>( Gaussino::HepMC::Attributes::GaudiEventNumber )->value();

    auto key = std::make_pair(eventNumber, evt2.event_number());
    auto evt1        = events1[key];
    if(!HepMC::compareGenEvent( evt1, evt2 )){
      std::cerr << "Event not equal #" << eventNumber << ", " << evt2.event_number() << ". Abort!" << std::endl;
      //return 1;
    }
  }

  return 0;
}
