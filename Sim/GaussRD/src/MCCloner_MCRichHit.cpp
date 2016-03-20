#include "MCCloner.h"

LHCb::MCRichHit* MCCloner::getStoredMCRichHit(const LHCb::MCRichHit* mchit) {
    auto result = m_mcrichhit.find(mchit);
    if (result == m_mcrichhit.end()) {
        return nullptr;
    } else {
        return result->second;
    }
}

LHCb::MCRichHit* MCCloner::cloneKeyedMCRichHit(const LHCb::MCRichHit* mchit) {
    auto clone = getStoredMCRichHit(mchit);
    if (!clone) {
        // Copy all the properties. Matching mc particle will be selected
        // later.
        clone = new LHCb::MCRichHit();
        clone->setSensDetID(mchit->sensDetID());
        clone->setEntry(mchit->entry());
        clone->setEnergy(mchit->energy());
        clone->setTimeOfFlight(mchit->timeOfFlight());
        clone->setRich(mchit->rich());
        clone->setRadiator(mchit->radiator());
        clone->setAerogelTileID(mchit->aerogelTileID());
        clone->setAerogelSubTileID(mchit->AerogelSubTileID());
        clone->setChargedTrack(mchit->chargedTrack());
        clone->setScatteredPhoton(mchit->scatteredPhoton());
        clone->setHpdSiBackscatter(mchit->hpdSiBackscatter());
        clone->setRadScintillation(mchit->radScintillation());
        clone->setHpdReflQWPC(mchit->hpdReflQWPC());
        clone->setHpdReflChr(mchit->hpdReflChr());
        clone->setHpdReflAirQW(mchit->hpdReflAirQW());
        clone->setHpdReflAirPC(mchit->hpdReflAirPC());
        clone->setHpdReflSi(mchit->hpdReflSi());
        clone->setHpdReflKovar(mchit->hpdReflKovar());
        clone->setHpdReflKapton(mchit->hpdReflKapton());
        clone->setHpdReflPCQW(mchit->hpdReflPCQW());
        m_mcrichhit.insert(std::pair<const LHCb::MCRichHit*, LHCb::MCRichHit*>(mchit, clone));
        m_list_mcrichhits->push_back(clone);
    }

    return clone;
}

LHCb::MCRichHit* MCCloner::cloneMCRichHit(const LHCb::MCRichHit* mchit) {
    if (!mchit) return NULL;
    LHCb::MCRichHit* clone = getStoredMCRichHit(mchit);
    return (clone ? clone : this->doCloneMCRichHit(mchit));
}

LHCb::MCRichHit* MCCloner::doCloneMCRichHit(const LHCb::MCRichHit* mchit) {
    if (!mchit) return NULL;

    // Clone the MCRichHit
    LHCb::MCRichHit* clone = cloneKeyedMCRichHit(mchit);
    // Fix the MCParticle relation of the clone
    auto org_part = mchit->mcParticle();

    // Assume those have been transformed before
    if (org_part) {
        auto cp_part = getStoredMCP(org_part);
        if (cp_part) {
            clone->setMCParticle(cp_part);
            clone->setMCParticle(cloneMCP(mchit->mcParticle()));
        } else {
            // TODO: Produce an error
        }
    }

    return clone;
}

LHCb::MCRichHits* MCCloner::getClonedMCRichHits() {
    if(!m_list_mcrichhits){
        m_list_mcrichhits = new LHCb::MCRichHits();
    }
    return m_list_mcrichhits;
}
