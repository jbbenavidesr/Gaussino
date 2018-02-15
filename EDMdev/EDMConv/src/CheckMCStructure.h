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

/** @class CheckMCStructure CheckMCStructure.h
 *
 * Simple algorithm to debug redecayed MCParticle output
 *
 *  @author Dominik Muller
 *  @date   2016-3-30
 */

typedef std::vector<LHCb::MCParticle> MCPARTICLES;
typedef std::vector<LHCb::MCVertex> MCVERTICES;

namespace LHCb {
class IParticlePropertySvc;
}
class CheckMCStructure
    : public Gaudi::Functional::Consumer<void(
          const MCPARTICLES&, const MCVERTICES&, const LHCb::MCHeader&)> {
  public:
  /// Standard constructor
  CheckMCStructure(const std::string& name, ISvcLocator* pSvcLocator)
      : Consumer(name, pSvcLocator,
                 {{KeyValue{"Particles", LHCb::MCParticleLocation::Default},
                   KeyValue{"Vertices", LHCb::MCVertexLocation::Default},
                   KeyValue{"MCHeader", LHCb::MCHeaderLocation::Default}}}){};

  void operator()(const MCPARTICLES&, const MCVERTICES&,
                  const LHCb::MCHeader&) const override;
  virtual StatusCode initialize() override;

  private:
  LHCb::IParticlePropertySvc* m_ppSvc;

  int printMCParticlesTree(LHCb::MCVertex* vtx, unsigned int& n_particles,
                           unsigned int& n_vertices, int level = 0,
                           int counter = 0) const;
};
