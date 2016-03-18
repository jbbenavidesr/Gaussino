#include "MCCloner.h"

LHCb::MCHit* MCCloner::getStoredMCHit(const LHCb::MCHit* mchit, const std::string& vol) {
  auto volmap = m_mchit.find(vol);
  if (volmap == m_mchit.end()) {
    auto temp = std::map<const LHCb::MCHit*, LHCb::MCHit*>();
    m_mchit.insert(std::pair<std::string, std::map<const LHCb::MCHit*, LHCb::MCHit*>>(vol, temp));
    return nullptr;
  } else {
    auto result = (*volmap).second.find(mchit);
    if (result == (*volmap).second.end()) {
      return nullptr;
    } else {
      return result->second;
    }
  }
}

LHCb::MCHit* MCCloner::cloneKeyedMCHit(const LHCb::MCHit* mchit, const std::string& vol) {
  auto clone = getStoredMCHit(mchit, vol);
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
    m_mchit[vol].insert(std::pair<const LHCb::MCHit*, LHCb::MCHit*>(mchit, clone));
  }

  return clone;
}

LHCb::MCHit* MCCloner::cloneMCHit(const LHCb::MCHit* mchit, const std::string& vol) {
  if (!mchit) return NULL;
  LHCb::MCHit* clone = getStoredMCHit(mchit, vol);
  return (clone ? clone : this->doCloneMCHit(mchit, vol));
}

LHCb::MCHit* MCCloner::doCloneMCHit(const LHCb::MCHit* mchit, const std::string& vol) {
  if (!mchit) return NULL;

  // Clone the MCHit
  LHCb::MCHit* clone = cloneKeyedMCHit(mchit, vol);
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

LHCb::MCHits* MCCloner::getClonedMCHits(const std::string& vol) {
  auto temp = new LHCb::MCHits();
  auto volmap = m_mchit.find(vol);

  if (volmap == m_mchit.end()) {
    return nullptr;
  } else {
    for (auto& h : (*volmap).second) {
      temp->add(h.second);
    }
    return temp;
  }
}
