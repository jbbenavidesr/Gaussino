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
