#include "MCCloner.h"

void MCCloner::cloneDecayVertices(const SmartRefVector<LHCb::MCVertex>& endVertices, LHCb::MCParticle* clonedParticle) {
    for (SmartRefVector<LHCb::MCVertex>::const_iterator iEndVtx = endVertices.begin(); iEndVtx != endVertices.end(); ++iEndVtx) {
        if ((*iEndVtx)->isDecay() && !((*iEndVtx)->products().empty())) {
            LHCb::MCVertex* decayVertexClone = cloneMCV(*iEndVtx);
            clonedParticle->addToEndVertices(decayVertexClone);
        }
    }
}

void MCCloner::cloneDecayProducts(const SmartRefVector<LHCb::MCParticle>& products, LHCb::MCVertex* clonedVertex) {
    for (SmartRefVector<LHCb::MCParticle>::const_iterator iProd = products.begin(); iProd != products.end(); ++iProd) {
        LHCb::MCParticle* productClone = cloneMCP(*iProd);
        if (productClone) {
            productClone->setOriginVertex(clonedVertex);
            clonedVertex->addToProducts(productClone);
        }
    }
}

MCCloner* MCCloner::DeepClone() {
    auto new_cloner = new MCCloner();
    // Clone ALL the particles again. These clones are placed back
    // into the TES later on. For the linking to work correctly,
    // clone in the same order as before:
    //
    // GetTrackerHitsAlg/GetPuVetoHits
    // GetTrackerHitsAlg/GetVeloHits
    // GetTrackerHitsAlg/GetTTHits
    // GetTrackerHitsAlg/GetITHits
    // GetTrackerHitsAlg/GetOTHits
    // GaudiSequencer/RichHits
    //    GetMCRichHitsAlg/GetRichHits
    //    GetMCRichOpticalPhotonsAlg/GetRichPhotons
    //    GetMCRichSegmentsAlg/GetRichSegments
    //    GetMCRichTracksAlg/GetRichTracks
    //    Rich::MC::MCPartToMCRichTrackAlg/MCPartToMCRichTrack
    //    Rich::MC::MCRichHitToMCRichOpPhotAlg/MCRichHitToMCRichOpPhot
    // GetCaloHitsAlg/GetSpdHits
    // GetCaloHitsAlg/GetPrsHits
    // GetCaloHitsAlg/GetEcalHits
    // GetCaloHitsAlg/GetHcalHits
    // GetTrackerHitsAlg/GetMuonHits
    //
    for (auto& m : *getClonedMCPs()) {
        new_cloner->cloneMCP(m);
    }
    for (auto& m : *getClonedMCVs()) {
        new_cloner->cloneMCV(m);
    }
    for (auto& s : m_list_mchits) {
        for (auto& m : *getClonedMCHits(s.first)) {
            new_cloner->cloneMCHit(m, s.first);
        }
    }
    for (auto& m : *getClonedMCRichHits()) {
        new_cloner->cloneMCRichHit(m);
    }
    for (auto& m : *getClonedMCRichOpticalPhotons()) {
        new_cloner->cloneMCRichOpticalPhoton(m);
    }
    for (auto& m : *getClonedMCRichSegments()) {
        new_cloner->cloneMCRichSegment(m);
    }
    for (auto& m : *getClonedMCRichTracks()) {
        new_cloner->cloneMCRichTrack(m);
    }
    for (auto& s : m_list_mccalohit) {
        for (auto& m : *getClonedMCCaloHits(s.first)) {
            new_cloner->cloneMCCaloHit(m, s.first);
        }
    }

    return new_cloner;
}

MCCloner::MCCloner(){}

void MCCloner::clear_no_deletion() {
    m_mcps.clear();
    m_mcvs.clear();
    m_mchit.clear();
    m_mccalohit.clear();
    m_mcrichhit.clear();
    m_mcrichops.clear();
    m_mcrichsegs.clear();
    m_mcrichtracks.clear();

    m_list_mcps = nullptr;
    m_list_mcvs = nullptr;
    m_list_mcrichhits = nullptr;
    m_list_mcrichops = nullptr;
    m_list_mcrichsegs = nullptr;
    m_list_mcrichtracks = nullptr;
    m_list_mchits.clear();
    m_list_mccalohit.clear();
}

void MCCloner::clear() {
    // Remove all objects created by the cloner.
    // Deletion of the object being cloned should be done
    // independently.
    delete m_list_mcps;
    delete m_list_mcvs;
    delete m_list_mcrichhits;
    delete m_list_mcrichops;
    delete m_list_mcrichsegs;
    delete m_list_mcrichtracks;
    for (auto& a : m_list_mchits) {
        delete a.second;
    }
    for (auto& a : m_list_mccalohit) {
        delete a.second;
    }
    clear_no_deletion();
}

MCCloner::~MCCloner() {}
