#include "MCCloner.h"

LHCb::MCRichSegment* MCCloner::getStoredMCRichSegment(const LHCb::MCRichSegment* mchit) {
    auto result = m_mcrichsegs.find(mchit);
    if (result == m_mcrichsegs.end()) {
        return nullptr;
    } else {
        return result->second;
    }
}

LHCb::MCRichSegment* MCCloner::cloneKeyedMCRichSegment(const LHCb::MCRichSegment* mchit) {
    auto clone = getStoredMCRichSegment(mchit);
    if (!clone) {
        // Copy all the properties.
        clone = new LHCb::MCRichSegment();
        m_mcrichsegs.insert(std::pair<const LHCb::MCRichSegment*, LHCb::MCRichSegment*>(mchit, clone));
        getClonedMCRichSegments()->insert(clone);

        clone->setRadiator(mchit->radiator());
        clone->setRich(mchit->rich());

        auto org_part = mchit->mcParticle();

        // Assume those have been transformed before
        if (org_part) {
            auto cp_part = getStoredMCP(org_part);
            if (cp_part) {
                clone->setMcParticle(cp_part);
            } else {
                // TODO: Produce an error as this should not happen if orignial
                // had a MCRichSegment assigned (should have been cloned before)
            }
        }
        for (auto& photon : mchit->mcRichOpticalPhotons()) {
            auto cp_photon = getStoredMCRichOpticalPhoton(photon);
            if (cp_photon) {
                clone->addToMCRichOpticalPhotons(cp_photon);
            } else {
                // TODO: Produce an error as this should not happen if orignial
                // had a MCRichSegment assigned (should have been cloned before)
            }
        }
        for (auto& mom : mchit->trajectoryMomenta()) {
            clone->addToTrajectoryMomenta(mom);
        }
        for (auto& mom : mchit->trajectoryPoints()) {
            clone->addToTrajectoryPoints(mom);
        }
    }

    return clone;
}

LHCb::MCRichSegment* MCCloner::cloneMCRichSegment(const LHCb::MCRichSegment* mchit) {
    if (!mchit) return NULL;
    LHCb::MCRichSegment* clone = getStoredMCRichSegment(mchit);
    return (clone ? clone : this->doCloneMCRichSegment(mchit));
}

LHCb::MCRichSegment* MCCloner::doCloneMCRichSegment(const LHCb::MCRichSegment* mchit) {
    if (!mchit) return NULL;

    // Clone the MCRichSegment
    LHCb::MCRichSegment* clone = cloneKeyedMCRichSegment(mchit);

    return clone;
}

LHCb::MCRichSegments* MCCloner::getClonedMCRichSegments() {
    if (!m_list_mcrichsegs) {
        m_list_mcrichsegs = new LHCb::MCRichSegments();
    }
    return m_list_mcrichsegs;
}
