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

#include <vector>

#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GiGaMT/IGiGaMTSvc.h"
#include "HepMC3/GenEvent.h"
#include "HepMCUser/typedefs.h"
#include "NewRnd/RndAlgSeeder.h"

class IHepMC3ToMCTruthConverter;
namespace LHCb {
  class IParticlePropertySvc;
}

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
                                                               const HepMC3::GenEventPtrs& ),
                                                           Gaudi::Functional::Traits::BaseClass_t<RndAlgSeeder>> {
public:
  /// Standard constructor
  GiGaAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : MultiTransformer( name, pSvcLocator, { KeyValue{ "Input", Gaussino::HepMCEventLocation::Default } },
                          { KeyValue{ "OutputG4Events", Gaussino::G4EventsLocation::Default },
                            KeyValue{ "OutputMCTruths", Gaussino::MCTruthsLocation::Default } } ){};

  virtual ~GiGaAlg() = default;

  std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> operator()( const HepMC3::GenEventPtrs& ) const override;

private:
  ServiceHandle<IGiGaMTSvc>                 m_gigaSvc{ this, "GiGaMTSvc", "GiGaMT" };
  ServiceHandle<LHCb::IParticlePropertySvc> m_ppSvc{ this, "PropertyService", "LHCb::ParticlePropertySvc" };
};
