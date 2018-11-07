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

  auto[event_number, run_number] = *m_forseed.get();

  std::vector<long> seeds;
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

  T engine( hashed_number );
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
