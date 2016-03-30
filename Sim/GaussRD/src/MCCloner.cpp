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
    
    new_cloner->m_mcps.reserve(m_mcps.size());
    new_cloner->getClonedMCPs()->reserve(m_mcps.size()+100);
    new_cloner->m_mcvs.reserve(m_mcvs.size());
    new_cloner->getClonedMCVs()->reserve(m_mcvs.size()+100);

    for (auto& m : *getClonedMCPs()) {
        new_cloner->cloneMCP(m);
    }
    for (auto& m : *getClonedMCVs()) {
        new_cloner->cloneMCV(m);
    }
  
    new_cloner->m_sig_part = new_cloner->cloneMCP(m_sig_part);
    for (auto& s : m_list_mchits) {
        auto temp = std::unordered_map<const LHCb::MCHit*, LHCb::MCHit*>();
        new_cloner->m_mchit.insert(std::pair<std::string, std::unordered_map<const LHCb::MCHit*, LHCb::MCHit*>>(s.first, temp));
        new_cloner->m_mchit[s.first].reserve(m_mchit[s.first].size()+100);
        for (auto& m : *getClonedMCHits(s.first)) {
            new_cloner->cloneMCHit(m, s.first);
        }
    }
    new_cloner->m_mcrichhit.reserve(m_mcrichhit.size());
    new_cloner->getClonedMCRichHits()->reserve(m_mcrichhit.size()+100);
    for (auto& m : *getClonedMCRichHits()) {
        new_cloner->cloneMCRichHit(m);
    }
    new_cloner->m_mcrichops.reserve(m_mcrichops.size());
    new_cloner->getClonedMCRichOpticalPhotons()->reserve(m_mcrichops.size()+100);
    for (auto& m : *getClonedMCRichOpticalPhotons()) {
        new_cloner->cloneMCRichOpticalPhoton(m);
    }
    new_cloner->m_mcrichsegs.reserve(m_mcrichsegs.size());
    new_cloner->getClonedMCRichSegments()->reserve(m_mcrichsegs.size()+100);
    for (auto& m : *getClonedMCRichSegments()) {
        new_cloner->cloneMCRichSegment(m);
    }
    new_cloner->m_mcrichtracks.reserve(m_mcrichtracks.size());
    new_cloner->getClonedMCRichTracks()->reserve(m_mcrichtracks.size()+100);
    for (auto& m : *getClonedMCRichTracks()) {
        new_cloner->cloneMCRichTrack(m);
    }
    for (auto& m : *getClonedGenCollisions()) {
        new_cloner->cloneGenCollision(m);
    }
    new_cloner->m_mcvs.reserve(m_mcvs.size());
    for (auto& s : m_list_mccalohit) {
        auto temp = std::unordered_map<const LHCb::MCCaloHit*, LHCb::MCCaloHit*>();
        new_cloner->m_mccalohit.insert(std::pair<std::string, std::unordered_map<const LHCb::MCCaloHit*, LHCb::MCCaloHit*>>(s.first, temp));
        new_cloner->m_mccalohit[s.first].reserve(m_mccalohit[s.first].size()+100);
        for (auto& m : *getClonedMCCaloHits(s.first)) {
            new_cloner->cloneMCCaloHit(m, s.first);
        }
    }

    return new_cloner;
}

MCCloner::MCCloner()
    : m_mcps(),
      m_mcvs(),
      m_mcrichhit(),
      m_mcrichops(),
      m_mcrichsegs(),
      m_mcrichtracks(),
      m_mchit(),
      m_mccalohit(),
      m_list_mchits(),
      m_list_mccalohit(),
      m_gencollisions(){}

void MCCloner::clear_no_deletion() {
    m_mcps.clear();
    m_mcvs.clear();
    m_mchit.clear();
    m_mccalohit.clear();
    m_mcrichhit.clear();
    m_mcrichops.clear();
    m_mcrichsegs.clear();
    m_mcrichtracks.clear();
    m_gencollisions.clear();

    m_list_mcps = nullptr;
    m_list_mcvs = nullptr;
    m_list_mcrichhits = nullptr;
    m_list_mcrichops = nullptr;
    m_list_mcrichsegs = nullptr;
    m_list_mcrichtracks = nullptr;
    m_list_gencollisions = nullptr;
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
    delete m_list_gencollisions;
    for (auto& a : m_list_mchits) {
        delete a.second;
    }
    for (auto& a : m_list_mccalohit) {
        delete a.second;
    }
    clear_no_deletion();
}

MCCloner::~MCCloner() {}
