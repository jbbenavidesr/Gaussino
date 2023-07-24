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
#include "GiGaMTCoreMessage/IMessage.h"
thread_local std::string Gsino::Message::NameTag{"Master"};

using namespace std::string_literals;

Gsino::Message::~Message() {
  if ( m_msg ) { delete m_msg; }
}

Gsino::Message::Message( Gsino::Message&& right ) {
  m_msg       = right.m_msg;
  right.m_msg = nullptr;
}

void Gsino::Message::debug( std::string message ) const {
  if ( !m_msg || !printDebug() ) return;
  m_msg->debug( "[ "s + NameTag + " ] "s + message );
}

void Gsino::Message::verbose( std::string message ) const {
  if ( !m_msg || !printVerbose() ) return;
  m_msg->verbose( "[ "s + NameTag + " ] "s + message );
}

void Gsino::Message::error( std::string message, unsigned int mx ) const {
  if ( !m_msg ) return;
  auto toprint = "[ "s + NameTag + " ] "s;
  if ( mx > 0 ) {
    const size_t num = increment( m_errors, message );
    if ( num > mx ) return;
    if ( num == mx ) {
      m_msg->error( toprint + "The ERROR message is suppressed : '"s + message + "'"s );
      return;
    }
  }
  m_msg->error( toprint + message );
}

void Gsino::Message::warning( std::string message, unsigned int mx ) const {
  if ( !m_msg ) return;
  auto toprint = "[ "s + NameTag + " ] "s;
  if ( mx > 0 ) {
    const size_t num = increment( m_warnings, message );
    if ( num > mx ) return;
    if ( num == mx ) {
      m_msg->warning( toprint + "The WARNING message is suppressed : '"s + message + "'"s );
      return;
    }
  }
  m_msg->warning( toprint + message );
}

void Gsino::Message::info( std::string message, unsigned int mx ) const {
  if ( !m_msg ) return;
  auto toprint = "[ "s + NameTag + " ] "s;
  if ( mx > 0 ) {
    const size_t num = increment( m_infos, message );
    if ( num > mx ) return;
    if ( num == mx ) {
      m_msg->info( toprint + "The INFO message is suppressed : '"s + message + "'"s );
      return;
    }
  }
  m_msg->info( toprint + message );
}

void Gsino::Message::always( std::string message ) const {
  if ( !m_msg ) return;
  m_msg->always( "[ "s + NameTag + " ] "s + message );
}
