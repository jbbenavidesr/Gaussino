/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#include "NewRnd/RndCommon.h"
#include <sstream>

namespace RndCommon {
  std::vector<long> seedEngine( CLHEP::HepRandomEngine& engine, unsigned int seed1, unsigned int seed2,
                                std::string label ) {
    std::vector<long> seeds;
    const std::string s =
        label + ( boost::io::str( boost::format( "_%1%_%2%" ) %
                                  boost::io::group( std::setfill( '0' ), std::hex, std::setw( 8 ), seed1 ) %
                                  boost::io::group( std::setfill( '0' ), std::hex, std::setw( 16 ), seed2 ) ) );
    auto hashed_named = std::hash<std::string>()( label );

    auto hashed_number = std::hash<std::string>()( s );
    seeds.push_back( hashed_number );
    seeds.push_back( hashed_named );
    engine.setSeeds( seeds.data(), seeds.size() );
    return seeds;
  }
  std::vector<long> seedEngine( HepRandomEnginePtr& engine, unsigned int seed1, unsigned int seed2,
                                std::string label ) {
    return seedEngine( *engine.get(), seed1, seed2, label );
  }
} // namespace RndCommon

HepRandomEnginePtr HepRandomEnginePtr::createSubRndmEngine() {
  if ( !m_constructor ) { throw std::runtime_error( "No RndConsructor set for subengine creation" ); }
  auto              subeng = m_children.emplace_back( m_constructor->construct() );
  auto&             engine = *get();
  std::stringstream lbl;
  lbl << m_label << "_" << m_children.size();
  RndCommon::seedEngine( engine, (unsigned int)engine, (unsigned int)engine, lbl.str() );
  return subeng;
}
