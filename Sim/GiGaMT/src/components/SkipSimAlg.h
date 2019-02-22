#pragma once

#include <vector>

#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GiGaMT/IGiGaMTSvc.h"
#include "HepMC/GenEvent.h"
#include "NewRnd/RndAlgSeeder.h"

class IHepMC3ToMCTruthConverter;
namespace LHCb {
class IParticlePropertySvc;
}

/** @class SkipSimAlg SkipSimAlg.h Algorithms/SkipSimAlg.h
 *
 *  Simple algorithm that will create an empty Geant4 event
 *  and attach a convert to it that contains the full truth record
 *  for later conversion to the event model for generator only.
 *
 *  @author Dominik Muller
 *  @date   21.2.2019
 *
 */
class SkipSimAlg : public Gaudi::Functional::Transformer<G4EventProxies( const std::vector<HepMC::GenEvent>& ) >
{
public:
  /// Standard constructor
  SkipSimAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : Transformer( name, pSvcLocator, KeyValue{"Input", Gaussino::HepMCEventLocation::Default},
                     KeyValue{"Output", Gaussino::G4EventsLocation::Default} ){};

  virtual ~SkipSimAlg() = default;

  G4EventProxies operator()( const std::vector<HepMC::GenEvent>& ) const override;

private:
  ServiceHandle<LHCb::IParticlePropertySvc> m_ppSvc{this, "PropertyService", "LHCb::ParticlePropertySvc"};
  PublicToolHandle<IHepMC3ToMCTruthConverter> m_converterTool{this, "HepMCConverter", "HepMC3ToMCTruthConverter"};
};
