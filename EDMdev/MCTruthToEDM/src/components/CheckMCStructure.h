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

// Include files
// from Gaudi
#include <vector>
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/GaudiAlgorithm.h"

// Event.
#include "Event/MCHeader.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

#include "Defaults/Locations.h"

/** @class CheckMCStructure CheckMCStructure.h
 *
 * Simple algorithm to debug redecayed MCParticle output
 *
 *  @author Dominik Muller
 *  @date   2016-3-30
 */

namespace LHCb {
class IParticlePropertySvc;
}
class CheckMCStructure
    : public Gaudi::Functional::Consumer<void(
          const LHCb::MCParticles&, const LHCb::MCVertices&, const LHCb::MCHeader&)> {
  public:
  /// Standard constructor
  CheckMCStructure(const std::string& name, ISvcLocator* pSvcLocator)
      : Consumer(name, pSvcLocator,
                 {KeyValue{"Particles", Gaussino::MCParticleLocation::Default},
                  KeyValue{"Vertices", Gaussino::MCVertexLocation::Default},
                  KeyValue{"MCHeader", LHCb::MCHeaderLocation::Default}}){};

  void operator()(const LHCb::MCParticles&, const LHCb::MCVertices&,
                  const LHCb::MCHeader&) const override;

  private:
  ServiceHandle<LHCb::IParticlePropertySvc> m_ppSvc{this, "PropertyService", "LHCb::ParticlePropertySvc"};

  int printMCParticlesTree(LHCb::MCVertex* vtx, unsigned int& n_particles,
                           unsigned int& n_vertices, int level = 0,
                           int counter = 0) const;
};
