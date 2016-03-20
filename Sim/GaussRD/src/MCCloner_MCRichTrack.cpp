#include "MCCloner.h"

LHCb::MCRichTrack* MCCloner::getStoredMCRichTrack(const LHCb::MCRichTrack* mchit) {
    auto result = m_mcrichtracks.find(mchit);
    if (result == m_mcrichtracks.end()) {
        return nullptr;
    } else {
        return result->second;
    }
}

LHCb::MCRichTrack* MCCloner::cloneKeyedMCRichTrack(const LHCb::MCRichTrack* mchit) {
    auto clone = getStoredMCRichTrack(mchit);
    if (!clone) {
        // Copy all the properties.
        clone = new LHCb::MCRichTrack();
        m_mcrichtracks.insert(std::pair<const LHCb::MCRichTrack*, LHCb::MCRichTrack*>(mchit, clone));

        auto org_part = mchit->mcParticle();

        // Assume those have been transformed before
        if (org_part) {
            auto cp_part = getStoredMCP(org_part);
            if (cp_part) {
                clone->setMcParticle(cp_part);
                m_list_mcrichtracks->insert(clone, cp_part->key());
            } else {
                // TODO: Produce an error as this should not happen if orignial
                // had a MCParticle assigned (should have been cloned before)
            }
        }
        for (auto& seg : mchit->mcSegments()) {
            if (seg) {
                auto cp_seg = getStoredMCRichSegment(seg);
                if (cp_seg) {
                    clone->addToMcSegments(cp_seg);
                    cp_seg->setMCRichTrack(clone);
                } else {
                    // TODO: Produce an error as this should not happen if orignial
                    // had a MCRichSegment assigned (should have been cloned before)
                }
            }
        }
    }

    return clone;
}

LHCb::MCRichTrack* MCCloner::cloneMCRichTrack(const LHCb::MCRichTrack* mchit) {
    if (!mchit) return NULL;
    LHCb::MCRichTrack* clone = getStoredMCRichTrack(mchit);
    return (clone ? clone : this->doCloneMCRichTrack(mchit));
}

LHCb::MCRichTrack* MCCloner::doCloneMCRichTrack(const LHCb::MCRichTrack* mchit) {
    if (!mchit) return NULL;

    // Clone the MCRichTrack
    LHCb::MCRichTrack* clone = cloneKeyedMCRichTrack(mchit);

    return clone;
}

LHCb::MCRichTracks* MCCloner::getClonedMCRichTracks() {
    if (!m_list_mcrichtracks) {
        m_list_mcrichtracks = new LHCb::MCRichTracks();
    }
    return m_list_mcrichtracks;
}
