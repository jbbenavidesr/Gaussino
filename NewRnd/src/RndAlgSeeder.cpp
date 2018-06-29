#include "NewRnd/RndAlgSeeder.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "boost/format.hpp"
#include <type_traits>

template <typename T>
T RndAlgSeeder::createRndmEngine() const
{
  static_assert( std::is_base_of<CLHEP::HepRandomEngine, T>::value,
                 "Random engine must inherit from CLHEP::HepRandomEngine" );

  // m_rndtool->seed( m_runNumber, eventNumber, seeds );

  auto context                  = Gaudi::Hive::currentContext();
  unsigned int event_number     = context.evt();
  unsigned long long run_number = context.eventID().run_number();

  std::vector<long int> seeds;
  int seed1a = event_number & 0x7FFFFFFF;

  // Make two 31 bit seeds out of run_number
  int seed2a = (int)( run_number & 0x7FFFFFFF );
  int seed2b = (int)( ( run_number >> 32 ) & 0x7FFFFFFF );

  if ( 0 != seed1a ) {
    seeds.push_back( seed1a );
  }
  if ( 0 != seed2a ) {
    seeds.push_back( seed2a );
  }
  if ( 0 != seed2b ) {
    seeds.push_back( seed2b );
  }

  // Get last seed by hashing string containing seed1 and seed2
  const std::string s =
      name() + ( boost::io::str( boost::format( "_%1%_%2%_%3%" ) %
                                 boost::io::group( std::setfill( '0' ), std::hex, std::setw( 8 ), event_number ) %
                                 boost::io::group( std::setfill( '0' ), std::hex, std::setw( 16 ), run_number ) %
                                 this->name() ) );
  auto hashed_named = std::hash<std::string>()( s );

  seeds.push_back( hashed_named );
  seeds.push_back( 0 );

  if ( MSG::DEBUG ) {
    debug() << "Unique string " << s << endmsg;
    debug() << "using seeds " << seeds << endmsg;
  }

  T engine;
  if ( m_forcedSeed == 0 ) {
    engine.setSeeds( seeds.data() );
  } else {
    engine.setSeed( m_forcedSeed );
  }
  return engine;
}

template CLHEP::MixMaxRng RndAlgSeeder::createRndmEngine() const;
