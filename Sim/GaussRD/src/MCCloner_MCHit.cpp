#include "MCCloner.h"

LHCb::MCHit* MCCloner::cloneKeyedMCHit(const LHCb::MCHit* mchit) {
  auto clone = getStoredMCHit(mchit);
  if (!clone) {
    // Copy all the properties. Matching mc particle will be selected
    // later.
    clone = new LHCb::MCHit();
    clone->setEntry(mchit->entry());
    clone->setDisplacement(mchit->displacement());
    clone->setEnergy(mchit->energy());
    clone->setTime(mchit->time());
    clone->setP(mchit->p());
    clone->setSensDetID(mchit->sensDetID());
    m_mchit.insert(std::pair<const LHCb::MCHit*, LHCb::MCHit*>(mchit, clone));
  }

  return clone;
}

LHCb::MCHit* MCCloner::cloneMCHit(const LHCb::MCHit* mchit) {
  if (!mchit) return NULL;
  LHCb::MCHit* clone = getStoredMCHit(mchit);
  return (clone ? clone : this->doCloneMCHit(mchit));
}

LHCb::MCHit* MCCloner::doCloneMCHit(const LHCb::MCHit* mchit) {
  if (!mchit) return NULL;

  // Clone the MCHit
  LHCb::MCHit* clone = cloneKeyedMCHit(mchit);
  // Fix the MCParticle relation of the clone
  auto org_part = mchit->mcParticle();

  // Assume those have been transformed before
  if (org_part) {
    auto cp_part = getStoredMCP(org_part);
    if (cp_part) {
      clone->setMCParticle(cp_part);
    } else {
      // TODO: Produce an error
    }
  }

  return clone;
}
