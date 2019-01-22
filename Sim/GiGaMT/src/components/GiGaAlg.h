#pragma once

#include <vector>

#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GiGaMT/IGiGaMTSvc.h"
#include "HepMC/GenEvent.h"
#include "NewRnd/RndAlgSeeder.h"

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
class GiGaAlg : public Gaudi::Functional::Transformer<G4EventProxies( const std::vector<HepMC::GenEvent>& ),
                                                      Gaudi::Functional::Traits::BaseClass_t<RndAlgSeeder>>
{
public:
  /// Standard constructor
  GiGaAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : Transformer( name, pSvcLocator, KeyValue{"Input", Gaussino::HepMCEventLocation::Default},
                     KeyValue{"Output", Gaussino::G4EventsLocation::Default} ){};

  virtual ~GiGaAlg() = default;

  G4EventProxies operator()( const std::vector<HepMC::GenEvent>& ) const override;

private:
  ServiceHandle<IGiGaMTSvc> m_gigaSvc{this, "GiGaMTSvc", "GiGaMT"};
};
