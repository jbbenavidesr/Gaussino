#include "MCCloner.h"

LHCb::MCCaloHit* MCCloner::getStoredMCCaloHit(const LHCb::MCCaloHit* mccalohit, const std::string& vol) {
    auto volmap = m_mccalohit.find(vol);
    if (volmap == m_mccalohit.end()) {
        auto temp = std::unordered_map<const LHCb::MCCaloHit*, LHCb::MCCaloHit*>();
        m_mccalohit.insert(std::pair<std::string, std::unordered_map<const LHCb::MCCaloHit*, LHCb::MCCaloHit*>>(vol, temp));
        return nullptr;
    } else {
        auto result = (*volmap).second.find(mccalohit);
        if (result == (*volmap).second.end()) {
            return nullptr;
        } else {
            return result->second;
        }
    }
}

LHCb::MCCaloHit* MCCloner::cloneKeyedMCCaloHit(const LHCb::MCCaloHit* mccalohit, const std::string& vol) {
    auto clone = getStoredMCCaloHit(mccalohit, vol);
    if (!clone) {
        // Copy all the properties. Matching mc particle will be selected
        // later.
        clone = new LHCb::MCCaloHit();
        clone->setTime(mccalohit->time());
        clone->setCellID(mccalohit->cellID());
        clone->setActiveE(mccalohit->activeE());
        clone->setSensDetID(mccalohit->sensDetID());
        m_mccalohit[vol].insert(std::pair<const LHCb::MCCaloHit*, LHCb::MCCaloHit*>(mccalohit, clone));
        getClonedMCCaloHits(vol)->add(clone);
    }

    return clone;
}

LHCb::MCCaloHit* MCCloner::cloneMCCaloHit(const LHCb::MCCaloHit* mccalohit, const std::string& vol) {
    if (!mccalohit) return NULL;
    LHCb::MCCaloHit* clone = getStoredMCCaloHit(mccalohit, vol);
    return (clone ? clone : this->doCloneMCCaloHit(mccalohit, vol));
}

LHCb::MCCaloHit* MCCloner::doCloneMCCaloHit(const LHCb::MCCaloHit* mccalohit, const std::string& vol) {
    if (!mccalohit) return NULL;

    // Clone the MCCaloHits
    LHCb::MCCaloHit* clone = cloneKeyedMCCaloHit(mccalohit, vol);
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

LHCb::MCCaloHits* MCCloner::getClonedMCCaloHits(const std::string& vol) {
    auto volmap = m_list_mccalohit.find(vol);

    if (volmap == m_list_mccalohit.end()) {
        volmap = m_list_mccalohit.insert(std::pair<std::string, LHCb::MCCaloHits*>(vol, new LHCb::MCCaloHits())).first;
    }
    return volmap->second;
}
