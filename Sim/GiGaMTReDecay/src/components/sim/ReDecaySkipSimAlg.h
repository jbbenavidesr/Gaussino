#pragma once

#include <vector>

#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "HepMC3/GenEvent.h"
#include "HepMCUser/typedefs.h"
#include "NewRnd/RndAlgSeeder.h"
#include "GiGaMTReDecay/Token.h"
#include "GiGaMTReDecay/typedefs.h"
#include "GiGaMTReDecay/IRedecaySvc.h"

class IHepMC3ToMCTruthConverter;
namespace LHCb
{
  class IParticlePropertySvc;
}

/** @class ReDecaySkipSimAlg ReDecaySkipSimAlg.h Algorithms/ReDecaySkipSimAlg.h
 *
 *  Simple algorithm to pass events to the simulation
 *  service to be simulated. Blocks in the call to the
 *  algorithm until the simulation is finished.
 *
 *  @author Dominik Muller
 *  @date   25.6.2018
 *
 */
class ReDecaySkipSimAlg : public Gaudi::Functional::MultiTransformer<std::tuple<Gaussino::MCTruthPtrs, Gaussino::ReDecay::SignalTruths>(
                    const HepMC3::GenEventPtrs&, const HepMC3::GenEventPtrs& ),
                Gaudi::Functional::Traits::BaseClass_t<RndAlgSeeder>>
{
public:
  /// Standard constructor
  ReDecaySkipSimAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : MultiTransformer( name, pSvcLocator, {KeyValue{"InputOriginal", Gaussino::HepMCEventLocation::Default}, KeyValue{"InputSignal", Gaussino::HepMCEventLocation::Signal}},
                          {{KeyValue{"OutputMCTruths", Gaussino::MCTruthsLocation::Default},
                            KeyValue{"OutputSignalTruths", Gaussino::MCTruthsLocation::SignalTruthsMap}}} ){};

  virtual ~ReDecaySkipSimAlg() = default;

  std::tuple<Gaussino::MCTruthPtrs, Gaussino::ReDecay::SignalTruths> operator()( const HepMC3::GenEventPtrs&, const HepMC3::GenEventPtrs& ) const override;

private:
  ToolHandle<IHepMC3ToMCTruthConverter> m_converterTool{this, "HepMCConverter", "HepMC3ToMCTruthConverter"};
  ServiceHandle<LHCb::IParticlePropertySvc> m_ppSvc{this, "PropertyService", "LHCb::ParticlePropertySvc"};
  AnyDataHandle<Gaussino::ReDecay::Token> m_tokenhandle{Gaussino::ReDecayToken::Default, Gaudi::DataHandle::Reader, this};
  ServiceHandle<IReDecaySvc> m_redecaysvc{this, "ReDecaySvc", "ReDecaySvc"};
  std::function<std::string(int)> pid_to_name = [&]( int i ) -> std::string {
      if ( auto pid = m_ppSvc->find( LHCb::ParticleID( i ) ); pid ) {
        return pid->name();
      } else {
        return "UnknownToLHCb";
      }
    };
};
