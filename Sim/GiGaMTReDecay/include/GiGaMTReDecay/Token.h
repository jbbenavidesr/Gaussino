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
#pragma once
#include "GaudiKernel/MsgStream.h"
#include "NewRnd/RndAlgSeeder.h"
class IReDecaySvc;
class ReDecaySvc;

namespace Gaussino::ReDecay {

  // Class to store all relevant information on the current event
  // in terms of ReDecay. It is produced by the ReDecay service
  // when provided with the SeedPair of the current event. This
  // token is later used to determine what kind of processing the
  // current event needs to perform.
  // The destruction of the Token will signal to the service that
  // the processing is complete.
  class Token {
    friend ReDecaySvc;
    friend IReDecaySvc;

  public:
    friend MsgStream& operator<<<Token>( MsgStream&, const Token& );
    Token()               = default;
    Token( const Token& ) = delete;
    Token( Token&& right ) {
      m_redecay_svc             = right.m_redecay_svc;
      right.m_redecay_svc       = nullptr;
      m_original                = right.m_original;
      m_this_event_seedpair     = right.m_this_event_seedpair;
      m_original_event_seedpair = right.m_original_event_seedpair;
    };
    ~Token();
    bool IsOriginal(){
      return m_original_event_seedpair == m_this_event_seedpair;
    }

  private:
    Random::SeedPair m_this_event_seedpair{};
    Random::SeedPair m_original_event_seedpair{};
    IReDecaySvc*     m_redecay_svc{nullptr};
    bool             m_original{false};
  };

  class TokenGuard {
    friend IReDecaySvc;
    private:
      TokenGuard() = delete;
      TokenGuard(IReDecaySvc* svc): m_svc{svc}{}
      TokenGuard(const TokenGuard &) = delete;
    public:
      ~TokenGuard();
    private:
      IReDecaySvc* m_svc{nullptr};
  };
} // namespace Gaussino::ReDecay
template <>
MsgStream& operator<<<Gaussino::ReDecay::Token>( MsgStream& strm, const Gaussino::ReDecay::Token& token );
