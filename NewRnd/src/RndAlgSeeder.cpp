#include "NewRnd/RndAlgSeeder.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "boost/format.hpp"
#include <type_traits>

const unsigned long MASK32 = 0xffffffff;

template <typename T>
T RndAlgSeeder::createRndmEngine() const
{
  static_assert( std::is_base_of<CLHEP::HepRandomEngine, T>::value,
                 "Random engine must inherit from CLHEP::HepRandomEngine" );

  // m_rndtool->seed( m_runNumber, eventNumber, seeds );

  auto & context              = Gaudi::Hive::currentContext();
  unsigned int event_number = context.evt();
  long run_number = context.eventID().run_number();
  //long run_number = 42;

  std::vector<long> seeds;
  //int seed1a = event_number & 0x7FFFFFFF;

  // Make two 31 bit seeds out of run_number
  //int seed2a = (int)( run_number & 0x7FFFFFFF );
  //int seed2b = (int)( ( run_number >> 32 ) & 0x7FFFFFFF );

  // if ( 0 != seed1a ) {
  // seeds.push_back( seed1a );
  //}
  // if ( 0 != seed2a ) {
  // seeds.push_back( seed2a );
  //}
  // if ( 0 != seed2b ) {
  // seeds.push_back( seed2b );
  //}
  // seeds.push_back(event_number);
  // seeds.push_back(run_number);

  // Get last seed by hashing string containing seed1 and seed2
  // const std::string s =
  // name() + ( boost::io::str( boost::format( "_%1%_%2%_%3%" ) %
  // boost::io::group( std::setfill( '0' ), std::hex, std::setw( 8 ), event_number ) %
  // boost::io::group( std::setfill( '0' ), std::hex, std::setw( 16 ), run_number ) %
  // this->name() ) );
  const std::string s =
      name() + ( boost::io::str( boost::format( "_%1%_%2%" ) %
                                 boost::io::group( std::setfill( '0' ), std::hex, std::setw( 8 ), event_number ) %
                                 boost::io::group( std::setfill( '0' ), std::hex, std::setw( 16 ), run_number ) ) );
  auto hashed_named = std::hash<std::string>()( this->name() );

  auto hashed_number = std::hash<std::string>()( s );
  seeds.push_back( hashed_number );
  seeds.push_back( hashed_named );

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Unique string " << s << endmsg;
    debug() << "using seeds " << seeds << endmsg;
  }

  T engine(hashed_number);
  if ( m_forcedSeed != (size_t)0 ) {
    warning() << "Using fixed seed: " << m_forcedSeed << endmsg;
    engine.setSeed( m_forcedSeed );
  } else if ( m_simpleSeed ) {
    warning() << "Using simple seed: " << event_number + 1 << endmsg;
    engine.setSeed( event_number + 1 );
  } else {
    engine.setSeeds( seeds.data(), seeds.size() );
  }
  return engine;
}

template CLHEP::MixMaxRng RndAlgSeeder::createRndmEngine() const;
template CLHEP::RanluxEngine RndAlgSeeder::createRndmEngine() const;
