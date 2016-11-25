#ifndef GaussRedecayPrintMCParticles_H
#define GaussRedecayPrintMCParticles_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class GaussRedecayPrintMCParticles GaussRedecayPrintMCParticles.h
 *
 * Simple algorithm to debug redecayed MCParticle output
 *
 *  @author Dominik Muller
 *  @date   2016-3-30
 */

namespace LHCb {
class MCVertex;
class MCParticle;
}
class GaussRedecayPrintMCParticles : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRedecayPrintMCParticles(const std::string& Name, ISvcLocator* SvcLoc);

  virtual ~GaussRedecayPrintMCParticles();  ///< Destructor

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

  int printMCParticlesTree(LHCb::MCVertex* vtx, int level = 0, int counter = 0);
};

#endif  // GaussRedecayPrintMCParticles_H
