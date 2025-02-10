/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

// Gaussino
#include "GaussinoMLBase/ModelServerBase.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"

Gsino::ML::ModelServerBase::ModelServerBase( const std::string& name ) : m_name( name ){};

void Gsino::ML::ModelServerBase::setLevel( MSG::Level lvl ) {
  if ( m_stream ) m_stream->setLevel( lvl );
  m_lvl = lvl;
};

void Gsino::ML::ModelServerBase::setMsgStream( std::unique_ptr<MsgStream> str ) { m_stream = std::move( str ); };

MsgStream* Gsino::ML::ModelServerBase::msgStream() { return m_stream.get(); }

void Gsino::ML::ModelServerBase::print( const std::string& msg, MSG::Level lvl ) const {
  if ( level() > lvl ) return;
  std::lock_guard<std::mutex> lock( m_printingLock );
  std::string                 tmsg = "[" + m_name + "] " + msg;
  if ( m_stream ) {
    *m_stream << lvl << tmsg << ::endmsg;
  } else {
    lvl = ( lvl >= MSG::Level::NUM_LEVELS ) ? MSG::Level::ALWAYS : ( lvl < MSG::Level::NIL ) ? MSG::Level::NIL : lvl;
    if ( lvl >= m_lvl ) { std::cout << levelName( lvl ) << ": " << tmsg << std::endl; }
  }
}

MSG::Level Gsino::ML::ModelServerBase::ModelServerBase::level() const {
  if ( !m_stream ) { return m_lvl; }
  return m_stream->level();
}

const char* Gsino::ML::ModelServerBase::levelName( MSG::Level level ) const {
  switch ( level ) {
  case MSG::Level::VERBOSE:
    return "VERBOSE";
  case MSG::Level::DEBUG:
    return "DEBUG";
  case MSG::Level::INFO:
    return "INFO";
  case MSG::Level::WARNING:
    return "WARNING";
  case MSG::Level::ERROR:
    return "ERROR";
  case MSG::Level::FATAL:
    return "FATAL";
  case MSG::Level::ALWAYS:
    return "ALWAYS";
  default:
    throw std::invalid_argument( "Invalid level name" );
  }
}
