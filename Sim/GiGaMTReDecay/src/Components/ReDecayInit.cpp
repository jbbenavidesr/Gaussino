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
#include "GiGaMTReDecay/IRedecaySvc.h"
#include "GiGaMTReDecay/Token.h"
#include "RndInit/GenRndInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenReDecayInit
//
// 2018-01-29: Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
class GenReDecayInit : public GenRndInit {

public:
  using GenRndInit::GenRndInit;

  virtual std::tuple<LHCb::GenHeader, LHCb::BeamParameters, LHCb::ODIN, Gsino::SimHeader> operator()() const override;

private:
  ServiceHandle<IReDecaySvc>                              m_redecaysvc{ this, "ReDecaySvc", "ReDecaySvc" };
  mutable DataObjectWriteHandle<Gaussino::ReDecay::Token> m_tokenhandle{ Gaussino::ReDecayToken::Default, this };
};

DECLARE_COMPONENT( GenReDecayInit )

std::tuple<LHCb::GenHeader, LHCb::BeamParameters, LHCb::ODIN, Gsino::SimHeader> GenReDecayInit::operator()() const {
  debug() << "==> Execute" << endmsg;
  auto ret          = GenRndInit::operator()();
  auto seedpair     = GetSeedPair();
  auto [event, run] = seedpair;
  auto token        = m_redecaysvc->obtainToken( seedpair );
  info() << "Evt " << event << ",  Run " << run;
  info() << ",  Nr. in job = " << eventCounter();
  if ( token.IsOriginal() ) {
    info() << " Original";
  } else {
    info() << " ReDecayed";
  }
  info() << endmsg;
  m_tokenhandle.put( std::move( token ) );

  return ret;
}
