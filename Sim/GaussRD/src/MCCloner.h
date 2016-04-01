#ifndef MCCLONER_H
#define MCCLONER_H 1

#include <map>
#include <unordered_map>
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

#include "Event/GenCollision.h"

class GaussRDMergeAndClean;

class MCCloner {
  public:
  friend class GaussRDMergeAndClean;
  MCCloner();
  virtual ~MCCloner();

  LHCb::MCParticle* cloneMCP(const LHCb::MCParticle* mcp);
  LHCb::MCParticle* getStoredMCP(const LHCb::MCParticle* mcp);
  LHCb::MCParticles* getClonedMCPs();

  LHCb::MCVertex* cloneMCV(const LHCb::MCVertex* mcVertex);
  LHCb::MCVertex* getStoredMCV(const LHCb::MCVertex* mcv);
  LHCb::MCVertices* getClonedMCVs();

  LHCb::MCHit* cloneMCHit(const LHCb::MCHit* mchit, const std::string& vol);
  LHCb::MCHit* getStoredMCHit(const LHCb::MCHit* mchit, const std::string& vol);
  LHCb::MCHits* getClonedMCHits(const std::string& vol);

  LHCb::MCCaloHit* cloneMCCaloHit(const LHCb::MCCaloHit* mchit,
                                  const std::string& vol);
  LHCb::MCCaloHit* getStoredMCCaloHit(const LHCb::MCCaloHit* mchit,
                                      const std::string& vol);
  LHCb::MCCaloHits* getClonedMCCaloHits(const std::string& vol);

  LHCb::MCRichHit* cloneMCRichHit(const LHCb::MCRichHit* mchit);
  LHCb::MCRichHit* getStoredMCRichHit(const LHCb::MCRichHit* mchit);
  LHCb::MCRichHits* getClonedMCRichHits();

  LHCb::MCRichOpticalPhoton* cloneMCRichOpticalPhoton(
      const LHCb::MCRichOpticalPhoton* mchit);
  LHCb::MCRichOpticalPhoton* getStoredMCRichOpticalPhoton(
      const LHCb::MCRichOpticalPhoton* mchit);
  LHCb::MCRichOpticalPhotons* getClonedMCRichOpticalPhotons();

  LHCb::MCRichSegment* cloneMCRichSegment(const LHCb::MCRichSegment* mchit);
  LHCb::MCRichSegment* getStoredMCRichSegment(const LHCb::MCRichSegment* mchit);
  LHCb::MCRichSegments* getClonedMCRichSegments();

  LHCb::MCRichTrack* cloneMCRichTrack(const LHCb::MCRichTrack* mchit);
  LHCb::MCRichTrack* getStoredMCRichTrack(const LHCb::MCRichTrack* mchit);
  LHCb::MCRichTracks* getClonedMCRichTracks();

  LHCb::GenCollision* cloneGenCollision(const LHCb::GenCollision* mchit);
  LHCb::GenCollision* getStoredGenCollision(const LHCb::GenCollision* mchit);
  LHCb::GenCollisions* getClonedGenCollisions();

  // This deletes the internal ObjectVector<> responsible for storing the hits.
  // This will
  // delete the stored objects as well so only use it when necessary.
  void clear();
  // This clears the internal maps (does not delete the objects) and sets the
  // pointers
  // of the ObjectVectors to zero. Be sure to have them somehwere (e.g. in the
  // TES)
  // before proceding.
  void clear_no_deletion();
  MCCloner* DeepClone();

  private:
  // When merge-cloning later, we must be able to turn off carry-over of the
  // key.
  // Make function private to restrict access to the merge algorithm.
  void setCloneKey(bool c) { m_clone_key = c; };
  LHCb::MCParticle* cloneKeyedMCP(const LHCb::MCParticle* mcp);
  LHCb::MCParticle* doCloneMCP(const LHCb::MCParticle* mcp);

  LHCb::MCVertex* cloneKeyedMCV(const LHCb::MCVertex* mcv);
  LHCb::MCVertex* doCloneMCV(const LHCb::MCVertex* mcVertex);

  LHCb::MCHit* cloneKeyedMCHit(const LHCb::MCHit* mchit,
                               const std::string& vol);
  LHCb::MCHit* doCloneMCHit(const LHCb::MCHit* mchit, const std::string& vol);

  LHCb::MCCaloHit* cloneKeyedMCCaloHit(const LHCb::MCCaloHit* mccalohit,
                                       const std::string& vol);
  LHCb::MCCaloHit* doCloneMCCaloHit(const LHCb::MCCaloHit* mccalohit,
                                    const std::string& vol);

  LHCb::MCRichHit* cloneKeyedMCRichHit(const LHCb::MCRichHit* mchit);
  LHCb::MCRichHit* doCloneMCRichHit(const LHCb::MCRichHit* mchit);

  LHCb::MCRichOpticalPhoton* cloneKeyedMCRichOpticalPhoton(
      const LHCb::MCRichOpticalPhoton* mchit);
  LHCb::MCRichOpticalPhoton* doCloneMCRichOpticalPhoton(
      const LHCb::MCRichOpticalPhoton* mchit);

  LHCb::MCRichSegment* cloneKeyedMCRichSegment(
      const LHCb::MCRichSegment* mchit);
  LHCb::MCRichSegment* doCloneMCRichSegment(const LHCb::MCRichSegment* mchit);

  LHCb::MCRichTrack* cloneKeyedMCRichTrack(const LHCb::MCRichTrack* mchit);
  LHCb::MCRichTrack* doCloneMCRichTrack(const LHCb::MCRichTrack* mchit);

  LHCb::GenCollision* cloneKeyedGenCollision(const LHCb::GenCollision* mchit);
  LHCb::GenCollision* doCloneGenCollision(const LHCb::GenCollision* mchit);

  inline bool cloneOriginVertex(const LHCb::MCVertex* vertex) {
    return (vertex != NULL);
    // return vertex && (vertex->isDecay() || vertex->isPrimary() );
  }

  void cloneDecayVertices(const SmartRefVector<LHCb::MCVertex>& endVertices,
                          LHCb::MCParticle* clonedParticle);
  void cloneDecayProducts(const SmartRefVector<LHCb::MCParticle>& products,
                          LHCb::MCVertex* clonedVertex);

  std::unordered_map<const LHCb::MCParticle*, LHCb::MCParticle*> m_mcps;
  std::unordered_map<const LHCb::MCVertex*, LHCb::MCVertex*> m_mcvs;
  std::unordered_map<const LHCb::MCRichHit*, LHCb::MCRichHit*> m_mcrichhit;
  std::unordered_map<const LHCb::MCRichOpticalPhoton*,
                     LHCb::MCRichOpticalPhoton*> m_mcrichops;
  std::unordered_map<const LHCb::MCRichSegment*, LHCb::MCRichSegment*>
      m_mcrichsegs;
  std::unordered_map<const LHCb::MCRichTrack*, LHCb::MCRichTrack*>
      m_mcrichtracks;
  // Turns out we have to save the MCHits split into different locations, hence,
  // more maps ...

  std::map<std::string, std::unordered_map<const LHCb::MCHit*, LHCb::MCHit*>>
      m_mchit;
  std::map<std::string, std::unordered_map<const LHCb::MCCaloHit*,
                                           LHCb::MCCaloHit*>> m_mccalohit;

  // Storage for the containers
  LHCb::MCParticles* m_list_mcps = nullptr;
  LHCb::MCVertices* m_list_mcvs = nullptr;
  std::map<std::string, LHCb::MCHits*> m_list_mchits;
  std::map<std::string, LHCb::MCCaloHits*> m_list_mccalohit;
  LHCb::MCRichHits* m_list_mcrichhits = nullptr;
  LHCb::MCRichOpticalPhotons* m_list_mcrichops = nullptr;
  LHCb::MCRichSegments* m_list_mcrichsegs = nullptr;
  LHCb::MCRichTracks* m_list_mcrichtracks = nullptr;

  LHCb::GenCollisions* m_list_gencollisions = nullptr;
  std::map<const LHCb::GenCollision*, LHCb::GenCollision*> m_gencollisions;

  LHCb::MCParticle* m_sig_part = nullptr;
  bool m_clone_key = true;
};

#endif
