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

  virtual std::tuple<LHCb::GenHeader, LHCb::BeamParameters> operator()() const override;

protected:


private:
  ServiceHandle<IReDecaySvc> m_redecaysvc{this, "ReDecaySvc", "ReDecaySvc"};
  mutable AnyDataHandle<Gaussino::ReDecay::Token> m_tokenhandle{Gaussino::ReDecayToken::Default, Gaudi::DataHandle::Writer, this};
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
  auto token = m_redecaysvc->obtainToken(seedpair);
  m_tokenhandle.put( std::move( token ) );

  return ret;
}

StatusCode GenReDecayInit::finalize()
{
  return GenRndInit::finalize();
}
