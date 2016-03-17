#include "MCCloner.h"

LHCb::MCCaloHit* MCCloner::getStoredMCCaloHit(const LHCb::MCCaloHit* mccalohit) {
  auto result = m_mccalohit.find(mccalohit);
  if (result == m_mccalohit.end()) {
    return nullptr;
  } else {
    return result->second;
  }
}

LHCb::MCCaloHit* MCCloner::cloneKeyedMCCaloHit(const LHCb::MCCaloHit* mccalohit) {
  auto clone = getStoredMCCaloHit(mccalohit);
  if (!clone) {
    // Copy all the properties. Matching mc particle will be selected
    // later.
    clone = new LHCb::MCCaloHit();
    clone->setTime(mccalohit->time());
    clone->setCellID(mccalohit->cellID());
    clone->setActiveE(mccalohit->activeE());
    clone->setSensDetID(mccalohit->sensDetID());
    m_mccalohit.insert(std::pair<const LHCb::MCCaloHit*, LHCb::MCCaloHit*>(mccalohit, clone));
  }

  return clone;
}

LHCb::MCCaloHit* MCCloner::cloneMCCaloHit(const LHCb::MCCaloHit* mccalohit) {
  if (!mccalohit) return NULL;
  LHCb::MCCaloHit* clone = getStoredMCCaloHit(mccalohit);
  return (clone ? clone : this->doCloneMCCaloHit(mccalohit));
}

LHCb::MCCaloHit* MCCloner::doCloneMCCaloHit(const LHCb::MCCaloHit* mccalohit) {
  if (!mccalohit) return NULL;

  // Clone the MCCaloHits
  LHCb::MCCaloHit* clone = cloneKeyedMCCaloHit(mccalohit);
  // Fix the MCParticle relation of the clone
  auto org_part = mccalohit->particle();

  // Assume those have been transformed before
  if (org_part) {
    auto cp_part = getStoredMCP(org_part);
    if (cp_part) {
      clone->setParticle(cp_part);
    } else {
      // TODO: Produce an error
    }
  }

  return clone;
}

LHCb::MCCaloHits* MCCloner::getClonedMCCaloHits() {
  auto temp = new LHCb::MCCaloHits();
  for (auto& h : m_mccalohit) {
    temp->add(h.second);
  }
  return temp;
}
