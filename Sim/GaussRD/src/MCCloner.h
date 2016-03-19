#ifndef MCCLONER_H
#define MCCLONER_H 1

#include <map>
#include <vector>
#include <string>
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"

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

  LHCb::MCHit* cloneMCHit(const LHCb::MCHit* mchit, const std::string & vol);
  LHCb::MCHit* getStoredMCHit(const LHCb::MCHit* mchit, const std::string & vol);
  LHCb::MCHits* getClonedMCHits(const std::string & vol);

  LHCb::MCCaloHit* cloneMCCaloHit(const LHCb::MCCaloHit* mchit, const std::string & vol);
  LHCb::MCCaloHit* getStoredMCCaloHit(const LHCb::MCCaloHit* mchit, const std::string & vol);
  LHCb::MCCaloHits* getClonedMCCaloHits(const std::string & vol);

  LHCb::MCRichHit* cloneMCRichHit(const LHCb::MCRichHit* mchit);
  LHCb::MCRichHit* getStoredMCRichHit(const LHCb::MCRichHit* mchit);
  LHCb::MCRichHits* getClonedMCRichHits();

  LHCb::MCRichOpticalPhoton* cloneMCRichOpticalPhoton(const LHCb::MCRichOpticalPhoton* mchit);
  LHCb::MCRichOpticalPhoton* getStoredMCRichOpticalPhoton(const LHCb::MCRichOpticalPhoton* mchit);
  LHCb::MCRichOpticalPhotons* getClonedMCRichOpticalPhotons();

  LHCb::MCRichSegment* cloneMCRichSegment(const LHCb::MCRichSegment* mchit);
  LHCb::MCRichSegment* getStoredMCRichSegment(const LHCb::MCRichSegment* mchit);
  LHCb::MCRichSegments* getClonedMCRichSegments();

  LHCb::MCRichTrack* cloneMCRichTrack(const LHCb::MCRichTrack* mchit);
  LHCb::MCRichTrack* getStoredMCRichTrack(const LHCb::MCRichTrack* mchit);
  LHCb::MCRichTracks* getClonedMCRichTracks();

  void clear();
  void clear_no_deletion();
  MCCloner* DeepClone();

  private:
  LHCb::MCParticle* cloneKeyedMCP(const LHCb::MCParticle* mcp);
  LHCb::MCParticle* doCloneMCP(const LHCb::MCParticle* mcp);

  LHCb::MCVertex* cloneKeyedMCV(const LHCb::MCVertex* mcv);
  LHCb::MCVertex* doCloneMCV(const LHCb::MCVertex* mcVertex);

  LHCb::MCHit* cloneKeyedMCHit(const LHCb::MCHit* mchit, const std::string & vol);
  LHCb::MCHit* doCloneMCHit(const LHCb::MCHit* mchit, const std::string & vol);

  LHCb::MCCaloHit* cloneKeyedMCCaloHit(const LHCb::MCCaloHit* mccalohit, const std::string & vol);
  LHCb::MCCaloHit* doCloneMCCaloHit(const LHCb::MCCaloHit* mccalohit, const std::string & vol);

  LHCb::MCRichHit* cloneKeyedMCRichHit(const LHCb::MCRichHit* mchit);
  LHCb::MCRichHit* doCloneMCRichHit(const LHCb::MCRichHit* mchit);

  LHCb::MCRichOpticalPhoton* cloneKeyedMCRichOpticalPhoton(const LHCb::MCRichOpticalPhoton* mchit);
  LHCb::MCRichOpticalPhoton* doCloneMCRichOpticalPhoton(const LHCb::MCRichOpticalPhoton* mchit);

  LHCb::MCRichSegment* cloneKeyedMCRichSegment(const LHCb::MCRichSegment* mchit);
  LHCb::MCRichSegment* doCloneMCRichSegment(const LHCb::MCRichSegment* mchit);

  LHCb::MCRichTrack* cloneKeyedMCRichTrack(const LHCb::MCRichTrack* mchit);
  LHCb::MCRichTrack* doCloneMCRichTrack(const LHCb::MCRichTrack* mchit);

  inline bool cloneOriginVertex(const LHCb::MCVertex* vertex) {
    return (vertex != NULL);
    // return vertex && (vertex->isDecay() || vertex->isPrimary() );
  }

  void cloneDecayVertices(const SmartRefVector<LHCb::MCVertex>& endVertices, LHCb::MCParticle* clonedParticle);
  void cloneDecayProducts(const SmartRefVector<LHCb::MCParticle>& products, LHCb::MCVertex* clonedVertex);

  std::map<const LHCb::MCParticle*, LHCb::MCParticle*> m_mcps;
  std::map<const LHCb::MCVertex*, LHCb::MCVertex*> m_mcvs;
  std::map<const LHCb::MCRichHit*, LHCb::MCRichHit*> m_mcrichhit;
  std::map<const LHCb::MCRichOpticalPhoton*, LHCb::MCRichOpticalPhoton*> m_mcrichops;
  std::map<const LHCb::MCRichSegment*, LHCb::MCRichSegment*> m_mcrichsegs;
  std::map<const LHCb::MCRichTrack*, LHCb::MCRichTrack*> m_mcrichtracks;
  // Turns out we have to save the MCHits split into different locations, hence, more maps ...

  std::map<std::string, std::map<const LHCb::MCHit*, LHCb::MCHit*>> m_mchit;
  std::map<std::string, std::map<const LHCb::MCCaloHit*, LHCb::MCCaloHit*>> m_mccalohit;
};

#endif
