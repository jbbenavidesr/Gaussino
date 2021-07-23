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
#include "RndInit/GenRndInit.h"
#include "GiGaMTReDecay/Token.h"
#include "GiGaMTReDecay/IRedecaySvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenReDecayInit
//
// 2018-01-29: Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
class GenReDecayInit : public GenRndInit
{

public:
  /// Standard constructor
  using GenRndInit::GenRndInit;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual void printEventRun( long long evt, int run, std::vector<long int>* seeds = 0 ) const override;

  virtual std::tuple<LHCb::GenHeader, LHCb::BeamParameters> operator()() const override;

protected:


private:
  ServiceHandle<IReDecaySvc> m_redecaysvc{this, "ReDecaySvc", "ReDecaySvc"};
  mutable DataObjectWriteHandle<Gaussino::ReDecay::Token> m_tokenhandle{Gaussino::ReDecayToken::Default, this};
};

DECLARE_COMPONENT( GenReDecayInit )

StatusCode GenReDecayInit::initialize()
{
  StatusCode sc = GenRndInit::initialize();
  return sc;
}

std::tuple<LHCb::GenHeader, LHCb::BeamParameters> GenReDecayInit::operator()() const
{
  debug() << "==> Execute" << endmsg;
  auto ret = GenRndInit::operator()();
  auto seedpair = GetSeedPair();
  auto [event, run] = seedpair;
  auto token = m_redecaysvc->obtainToken(seedpair);
  info() << "Evt " << event << ",  Run " << run;
  info() << ",  Nr. in job = " << eventCounter();
  if(token.IsOriginal()){
    info() << " Original";
  } else {
    info() << " ReDecayed";
  }
  info() << endmsg;
  m_tokenhandle.put( std::move( token ) );

  return ret;
}

StatusCode GenReDecayInit::finalize()
{
  return GenRndInit::finalize();
}

void GenReDecayInit::printEventRun( long long , int , std::vector<long int>* ) const
{
}
