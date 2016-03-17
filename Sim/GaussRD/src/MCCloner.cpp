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

MCCloner::MCCloner() : m_mcps(), m_mcvs(), m_mchit() {}

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
  for (auto& m : m_mchit) {
    delete m.second;
  }
  m_mcps.clear();
  m_mcvs.clear();
  m_mchit.clear();
}
MCCloner::~MCCloner() { clear(); }
