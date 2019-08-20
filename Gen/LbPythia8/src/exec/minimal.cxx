#include "CLHEP/Random/MixMaxRng.h"
#include "CLHEP/Random/RandFlat.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/WriterAscii.h"
#include "HepMCUtils/CompareGenEvent.h"
#include "Pythia8/Basics.h"
#include "Pythia8/Pythia.h"
#include "Pythia8HepMC3/Pythia8ToHepMC3.h"
#include <map>
#include <thread>

typedef std::map<int, HepMC3::GenEvent*> STORE;

class RndForPythia : public Pythia8::RndmEngine
{
public:
  RndForPythia( CLHEP::HepRandomEngine& engine ) : m_gen( engine, 0, 1 ) {}
  virtual double flat()
  {
    auto val = m_gen();
    return val;
  }

private:
  CLHEP::RandFlat m_gen;
};

HepMC3::GenEvent* convert( Pythia8::Pythia* pythia )
{
  auto theEvent = new HepMC3::GenEvent{};
  HepMC3::Pythia8ToHepMC3 conversion;
  conversion.set_print_inconsistency( false );
  conversion.fill_next_event( *pythia, theEvent );

  return theEvent;
}

HepMC3::GenEvent* run_pythia( int seed, std::string dir, Pythia8::Pythia* pythia )
{
  std::ofstream file;
  std::stringstream outfilename, buffer;
  outfilename << dir << "/" << seed << ".txt";
  CLHEP::MixMaxRng engine;
  engine.setSeed( seed );

  RndForPythia rnd_pythia{engine};
  pythia->setRndmEnginePtr( &rnd_pythia );
  pythia->next();
  auto evt = convert( pythia );
  // HepMC3::WriterAscii writer( outfilename.str() );
  // writer.write_event( *evt );
  return evt;
}

void run_seq( std::string name, const std::vector<int> seeds, STORE* store )
{
  Pythia8::Pythia pythia{
      "/cvmfs/sft.cern.ch/lcg/releases/LCG_95/MCGenerators/pythia8/240/x86_64-centos7-gcc8-opt/share/Pythia8/xmldoc",
      false};

  pythia.readString( "Beams:frameType = 3" );
  pythia.readString( "Beams:idA = 2212" );
  pythia.readString( "Beams:idB = 2212" );
  // pythia.readString("SoftQCD:all = on" );
  // pythia.readString("SoftQCD:all = off");
  // pythia.readString("SoftQCD:nonDiffractive = on");
  // pythia.readString("SoftQCD:elastic = on");
  pythia.readString( "SoftQCD:singleDiffractive = on" ); // DOOOO
  // pythia.readString("SoftQCD:doubleDiffractive = on"); // DOOOO
  // pythia.readString("SoftQCD:centralDiffractive = on");
  // pythia.readString("SoftQCD:inelastic = on"); // DOOOO
  pythia.readString( "Beams:allowMomentumSpread = on" );

  // Create a random engine to be used during init()
  CLHEP::MixMaxRng engine;
  engine.setSeed( 42 );
  RndForPythia rnd_pythia1{engine};
  pythia.setRndmEnginePtr( &rnd_pythia1 );
  pythia.init();
  for ( auto s1 : seeds ) {
    auto evt       = run_pythia( s1, name, &pythia );
    ( *store )[s1] = evt;
  }
}

int main( int, char** )
{
  std::vector<int> seeds1, seeds2, tmp;
  for ( int i = 1; i <= 200; i++ ) {
    seeds1.push_back( i );
  }
  for ( int i = 1; i <= 200; i++ ) {
    seeds2.push_back( i );
  }
  std::random_shuffle( std::begin( seeds2 ), std::end( seeds2 ) );

  STORE store1, store2;

  // Event sequence
  std::thread t1{run_seq, "sequential1", seeds1, &store1};
  std::thread t2{run_seq, "sequential2", seeds2, &store2};
  t1.join();
  t2.join();

  for ( auto& val : store1 ) {
    auto evt1 = val.second;
    auto evt2 = store2[val.first];
    if ( !HepMC3::compareGenEvent( *evt1, *evt2 ) ) {
      std::cerr << "Event with seed " << val.first << " not equal" << std::endl;
      return 1;
    }
  }
  std::cerr << "Everything good!" << std::endl;

  return 0;
}
