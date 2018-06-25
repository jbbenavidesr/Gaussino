#pragma once

#include <vector>

#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "HepMC/GenEvent.h"
#include "NewRnd/RndAlgSeeder.h"

class IGiGaMTSvc;
/** @class GiGaAlg GiGaAlg.h Algorithms/GiGaAlg.h
 *
 *  Simple algorithm to pass events to the simulation
 *  service to be simulated. Blocks in the call to the
 *  algorithm until the simulation is finished.
 *
 *  TODO: GiGaMT cannot currently return things.
 *
 *  @author Dominik Muller
 *  @date   25.6.2018
 *
 */
class GiGaAlg : public Gaudi::Functional::Consumer<void( const std::vector<HepMC::GenEvent>& ),
                                                   Gaudi::Functional::Traits::BaseClass_t<RndAlgSeeder>>
{
public:
  /// Standard constructor
  GiGaAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : Consumer( name, pSvcLocator, {KeyValue{"Input", Gaussino::HepMCEventLocation::Default}} ){};
  Gaudi::Property<std::string> m_gigaMTname{this, "GiGaMTSvc", "GiGaMT"};

  virtual ~GiGaAlg() = default;

  void operator()( const std::vector<HepMC::GenEvent>& ) const override;
  virtual StatusCode finalize() override;
  virtual StatusCode initialize() override;

private:
  IGiGaMTSvc* m_gigaSvc = nullptr;
};
