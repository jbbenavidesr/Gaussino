// $Id: GaussRDCtrFilter.h,v 1.4 2007-01-12 15:23:41 ranjard Exp $
#ifndef GaussRDPrintMCParticles_H
#define GaussRDPrintMCParticles_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class GaussRDPrintMCParticles GaussRDPrintMCParticles.h
 *
 * Simple algorithm to debug redecayed MCParticle output
 *
 *  @author Dominik Muller
 *  @date   2016-3-30
 */

namespace LHCb {
class MCVertex;
}
class GaussRDPrintMCParticles : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRDPrintMCParticles(const std::string& Name, ISvcLocator* SvcLoc);

  virtual ~GaussRDPrintMCParticles();  ///< Destructor

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  protected:
  /** accessor to GaussRD Service
   *  @return pointer to GaussRD Service
   */

  private:
  std::string m_particlesLocation;  ///< Location in TES of output MCParticles.
  std::string m_mcHeaderLocation;   ///< Location in TES of MCHeader for PV.

  int printMCParticlesTree(LHCb::MCVertex* vtx, int level = 0, int counter = 0);
};

#endif  // GaussRDPrintMCParticles_H
