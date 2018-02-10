#ifndef CheckMCStructure_H
#define CheckMCStructure_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class CheckMCStructure CheckMCStructure.h
 *
 * Simple algorithm to debug redecayed MCParticle output
 *
 *  @author Dominik Muller
 *  @date   2016-3-30
 */

namespace LHCb {
class MCVertex;
class MCParticle;
class IParticlePropertySvc;
}
class CheckMCStructure : public GaudiAlgorithm {
  public:
  /// Standard constructor
  CheckMCStructure(const std::string& Name, ISvcLocator* SvcLoc);

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  protected:
  /** accessor to GaussRedecay Service
   *  @return pointer to GaussRedecay Service
   */

  private:
  std::string m_particlesLocation;  ///< Location in TES of output MCParticles.
  std::string m_verticeLocation;  ///< Location in TES of output MCParticles.
  std::string m_mcHeaderLocation;   ///< Location in TES of MCHeader for PV.
  std::set<LHCb::MCVertex*> m_vertices;
  std::set<LHCb::MCParticle*> m_particles;
  LHCb::IParticlePropertySvc* m_ppSvc;

  int printMCParticlesTree(LHCb::MCVertex* vtx, int level = 0, int counter = 0);
};

#endif  // CheckMCStructure_H
