#ifndef MCCLONER_H
#define MCCLONER_H 1

#include <map>
#include <vector>
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"

class MCCloner {
  public:
  MCCloner();
  virtual ~MCCloner();

  LHCb::MCParticle* cloneMCP(const LHCb::MCParticle* mcp);
  LHCb::MCParticle* getStoredMCP(const LHCb::MCParticle* mcp);
  LHCb::MCParticles* getClonedMCPs();

  LHCb::MCVertex* cloneMCV(const LHCb::MCVertex* mcVertex);
  LHCb::MCVertex* getStoredMCV(const LHCb::MCVertex* mcv);
  LHCb::MCVertices* getClonedMCVs();

  LHCb::MCHit* cloneMCHit(const LHCb::MCHit* mchit);
  LHCb::MCHit* getStoredMCHit(const LHCb::MCHit* mchit);
  LHCb::MCHits* getClonedMCHits();

  LHCb::MCCaloHit* cloneMCCaloHit(const LHCb::MCCaloHit* mchit);
  LHCb::MCCaloHit* getStoredMCCaloHit(const LHCb::MCCaloHit* mchit);
  LHCb::MCCaloHits* getClonedMCCaloHits();

  void clear();
  MCCloner* DeepClone();

  private:
  LHCb::MCParticle* cloneKeyedMCP(const LHCb::MCParticle* mcp);
  LHCb::MCVertex* cloneKeyedMCV(const LHCb::MCVertex* mcv);
  LHCb::MCHit* cloneKeyedMCHit(const LHCb::MCHit* mchit);
  LHCb::MCCaloHit* cloneKeyedMCCaloHit(const LHCb::MCCaloHit* mccalohit);
  LHCb::MCParticle* doCloneMCP(const LHCb::MCParticle* mcp);
  LHCb::MCVertex* doCloneMCV(const LHCb::MCVertex* mcVertex);
  LHCb::MCHit* doCloneMCHit(const LHCb::MCHit* mchit);
  LHCb::MCCaloHit* doCloneMCCaloHit(const LHCb::MCCaloHit* mccalohit);

  inline bool cloneOriginVertex(const LHCb::MCVertex* vertex) {
    return (vertex != NULL);
    // return vertex && (vertex->isDecay() || vertex->isPrimary() );
  }

  void cloneDecayVertices(const SmartRefVector<LHCb::MCVertex>& endVertices, LHCb::MCParticle* clonedParticle);
  void cloneDecayProducts(const SmartRefVector<LHCb::MCParticle>& products, LHCb::MCVertex* clonedVertex);

  std::map<const LHCb::MCParticle*, LHCb::MCParticle*> m_mcps;
  std::map<const LHCb::MCVertex*, LHCb::MCVertex*> m_mcvs;
  std::map<const LHCb::MCHit*, LHCb::MCHit*> m_mchit;
  std::map<const LHCb::MCCaloHit*, LHCb::MCCaloHit*> m_mccalohit;

};

#endif
