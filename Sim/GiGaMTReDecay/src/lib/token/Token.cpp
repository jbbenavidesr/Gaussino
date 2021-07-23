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
#include "GiGaMTReDecay/Token.h"
#include "GiGaMTReDecay/IRedecaySvc.h"


namespace Gaussino::ReDecay {
  Token::~Token(){
    if(m_redecay_svc){
      m_redecay_svc->removeToken(*this);
    }
  }
  TokenGuard::~TokenGuard(){
    if(m_svc){
      m_svc->m_currentToken = nullptr;
    }
  }
}
template <>
MsgStream& operator<<<Gaussino::ReDecay::Token>( MsgStream& strm, const Gaussino::ReDecay::Token& token ) {
  strm << " Pair: < " << token.m_this_event_seedpair.first << " , " << token.m_this_event_seedpair.second << " > "
       << " Original: < " << token.m_original_event_seedpair.first << " , " << token.m_original_event_seedpair.second
       << " >\n";

  return strm;
}
