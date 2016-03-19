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

MCCloner* MCCloner::DeepClone(){
  auto new_cloner = new MCCloner();
  for (auto& m : m_mcps) {
    new_cloner->cloneMCP(m.second);
  }
  for (auto& m : m_mcvs) {
    new_cloner->cloneMCV(m.second);
  }
  for (auto& m : m_mcrichhit) {
    new_cloner->cloneMCRichHit(m.second);
  }
  for (auto& m : m_mcrichops) {
    new_cloner->cloneMCRichOpticalPhoton(m.second);
  }
  for (auto& m : m_mcrichsegs) {
    new_cloner->cloneMCRichSegment(m.second);
  }
  for (auto& m : m_mcrichtracks) {
    new_cloner->cloneMCRichTrack(m.second);
  }
  for (auto& s : m_mchit) {
    for (auto& m : s.second) {
        new_cloner->cloneMCHit(m.second, s.first);
    }
  }
  for (auto& s : m_mccalohit) {
    for (auto& m : s.second) {
        new_cloner->cloneMCCaloHit(m.second, s.first);
    }
  }

  return new_cloner;
}

MCCloner::MCCloner() : m_mcps(), m_mcvs(), m_mchit() {}

void MCCloner::clear_no_deletion() {
  m_mcps.clear();
  m_mcvs.clear();
  m_mchit.clear();
  m_mccalohit.clear();
  m_mcrichhit.clear();
  m_mcrichops.clear();
  m_mcrichsegs.clear();
  m_mcrichtracks.clear();
}

void MCCloner::clear() {
  // Remove all objects created by the cloner.
  // Deletion of the object being cloned should be done
  // independently.
  for (auto& m : m_mcps) {
    delete m.second;
  }
  for (auto& m : m_mcvs) {
    delete m.second;
  }
  for (auto& m : m_mcrichhit) {
    delete m.second;
  }
  for (auto& m : m_mcrichops) {
    delete m.second;
  }
  for (auto& m : m_mcrichsegs) {
    delete m.second;
  }
  for (auto& m : m_mcrichtracks) {
    delete m.second;
  }
  for (auto& s : m_mchit) {
    for (auto& m : s.second) {
      delete m.second;
    }
  }
  for (auto& s : m_mccalohit) {
    for (auto& m : s.second) {
      delete m.second;
    }
  }
  clear_no_deletion();
}

MCCloner::~MCCloner() {}
