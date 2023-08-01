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

// Gaudi
#include "GaudiAlg/FunctionalUtilities.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/ServiceHandle.h"

// Gaussino
#include "Defaults/Locations.h"
#include "GiGaMT/IGiGaMTSvc.h"
#include "NewRnd/RndAlgSeeder.h"
#include "HepMCUser/typedefs.h"

// HepMC3
#include "HepMC3/GenEvent.h"

class IHepMC3ToMCTruthConverter;
namespace LHCb
{
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
class SkipSimAlg : public Gaudi::Functional::Transformer<Gaussino::MCTruthPtrs( const HepMC3::GenEventPtrs& ),
                                                         Gaudi::Functional::Traits::useLegacyGaudiAlgorithm>
{
public:
  /// Standard constructor
  SkipSimAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : Transformer( name, pSvcLocator, KeyValue{"Input", Gaussino::HepMCEventLocation::Default},
                     KeyValue{"Output", Gaussino::MCTruthsLocation::Default} ){};

  virtual ~SkipSimAlg() = default;

  Gaussino::MCTruthPtrs operator()( const HepMC3::GenEventPtrs& ) const override;

private:
  ServiceHandle<LHCb::IParticlePropertySvc> m_ppSvc{this, "PropertyService", "LHCb::ParticlePropertySvc"};
  ToolHandle<IHepMC3ToMCTruthConverter> m_converterTool{this, "HepMCConverter", "HepMC3ToMCTruthConverter"};
};
