#include "CLHEP/Random/MixMaxRng.h"
#include "CLHEP/Random/RandFlat.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/WriterAscii.h"
#include "LbPythia8/Pythia8ProductionMT.h"
#include "Pythia8/Basics.h"
#include "Pythia8/Pythia.h"
#include "Pythia8HepMC/Pythia8ToHepMC3.h"
#include <thread>

HepMC::GenEvent* convert( Pythia8::Pythia* pythia )
{
  // Convert to HepMC.

  auto theEvent = new HepMC::GenEvent{};
  HepMC::Pythia8ToHepMC3 conversion;
  conversion.set_print_inconsistency( false );
  conversion.fill_next_event( *pythia, theEvent );

  return theEvent;
}

HepMC::GenEvent* run_pythia( int seed, std::string dir, Pythia8::Pythia* pythia )
{

  std::ofstream file;
  std::stringstream outfilename, buffer;
  outfilename << dir << "/" << seed << ".txt";
  file.open( outfilename.str() );
  CLHEP::MixMaxRng engine;
  engine.setSeed( seed );

  // If passing a buffer here, the calls to the random engine are
  // monitored and backtraces printed to the file.
  //RndForPythia rnd_pythia{engine, &buffer};
  RndForPythia rnd_pythia{engine};
  pythia->setRndmEnginePtr( &rnd_pythia );
  pythia->next();
  auto evt = convert(pythia);
  file << buffer.str();
  file.close();
  return evt;
}

void run_seq(std::string name, const std::vector<int> seeds)
{
  Pythia8::Pythia pythia{"/cvmfs/sft.cern.ch/lcg/releases/LCG_93/MCGenerators/pythia8/230/x86_64-centos7-gcc7-opt/share/Pythia8/xmldoc",
                                false};

  pythia.readString("Beams:frameType = 3" );
  pythia.readString("Beams:idA = 2212" );
  pythia.readString("Beams:idB = 2212" );
  //pythia.readString("SoftQCD:all = on" );
  //pythia.readString("SoftQCD:all = off");
  //pythia.readString("SoftQCD:nonDiffractive = on");
  //pythia.readString("SoftQCD:elastic = on");
  pythia.readString("SoftQCD:singleDiffractive = on"); // DOOOO
  //pythia.readString("SoftQCD:doubleDiffractive = on"); // DOOOO
  //pythia.readString("SoftQCD:centralDiffractive = on");
  //pythia.readString("SoftQCD:inelastic = on"); // DOOOO
  //pythia.readString("Beams:allowMomentumSpread = on" );

  // Create a random engine to be used during init()
  CLHEP::MixMaxRng engine;
  engine.setSeed( 42 );
  RndForPythia rnd_pythia1{engine};
  pythia.setRndmEnginePtr( &rnd_pythia1 );
  pythia.init();
  HepMC::WriterAscii writer(name + ".txt");
  for ( auto s1 : seeds ) {
    std::cout << "Doing " << s1 << " from " << name <<std::endl;
    auto evt = run_pythia( s1, name, &pythia );
    // Set an attribute to compare events with the same seed later
    evt->add_attribute("GaudiEventNumber", std::make_shared<HepMC::IntAttribute>(s1));
    writer.write_event(*evt);
  }
}

int main( int, char** )
{
  std::vector <int> seeds1, seeds2;
  for(int i=1; i<=200; i++){
    seeds1.push_back(i);
    seeds2.push_back(i);
  }
  std::random_shuffle(std::begin(seeds2), std::end(seeds2));
  // Event sequence
  //std::thread t1{run_seq, "default1", seeds1};
  //std::thread t2{run_seq, "default2", seeds2};
  //t1.join();
  //t2.join();

  run_seq("sequential1", seeds1);
  run_seq("sequential2", seeds2);
  return 0;
}
