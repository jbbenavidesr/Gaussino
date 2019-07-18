#pragma once

#include <vector>

#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GiGaMT/IGiGaMTSvc.h"
#include "HepMC3/GenEvent.h"
#include "NewRnd/RndAlgSeeder.h"

class IHepMC3ToMCTruthConverter;

/** @class GiGaAlg GiGaAlg.h Algorithms/GiGaAlg.h
 *
 *  Simple algorithm to pass events to the simulation
 *  service to be simulated. Blocks in the call to the
 *  algorithm until the simulation is finished.
 *
 *  @author Dominik Muller
 *  @date   25.6.2018
 *
 */
class GiGaAlg : public Gaudi::Functional::MultiTransformer<std::tuple<G4EventProxies, Gaussino::MCTruthPtrs>(
                    const std::vector<HepMC3::GenEvent>& ),
                Gaudi::Functional::Traits::BaseClass_t<RndAlgSeeder>>
{
public:
  /// Standard constructor
  GiGaAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : MultiTransformer( name, pSvcLocator, {KeyValue{"Input", Gaussino::HepMCEventLocation::Default}},
                          {{KeyValue{"OutputG4Events", Gaussino::G4EventsLocation::Default},
                            KeyValue{"OutputMCTruths", Gaussino::MCTruthsLocation::Default}}} ){};

  virtual ~GiGaAlg() = default;

  std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> operator()( const std::vector<HepMC3::GenEvent>& ) const override;

private:
  ServiceHandle<IGiGaMTSvc> m_gigaSvc{this, "GiGaMTSvc", "GiGaMT"};
  ToolHandle<IHepMC3ToMCTruthConverter> m_converterTool{this, "HepMCConverter", "HepMC3ToMCTruthConverter"};
};
