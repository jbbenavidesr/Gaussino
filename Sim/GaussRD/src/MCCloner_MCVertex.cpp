#include "MCCloner.h"

LHCb::MCVertex* MCCloner::getStoredMCV(const LHCb::MCVertex* mcv) {
  auto result = m_mcvs.find(mcv);
  if (result == m_mcvs.end()) {
    return nullptr;
  } else {
    return result->second;
  }
}

LHCb::MCVertex* MCCloner::cloneKeyedMCV(const LHCb::MCVertex* mcv) {
  auto clone = getStoredMCV(mcv);
  if (!clone) {
    clone = mcv->clone();
    m_mcvs.insert(std::pair<const LHCb::MCVertex*, LHCb::MCVertex*>(mcv, clone));
  }
  return clone;
}

LHCb::MCVertex* MCCloner::cloneMCV(const LHCb::MCVertex* vertex) {
  if (!vertex) return NULL;
  LHCb::MCVertex* clone = getStoredMCV(vertex);

  const size_t nProd = vertex->products().size();
  const size_t nCloneProd = (clone ? clone->products().size() : 0);

  return (clone && (nProd == nCloneProd) ? clone : this->doCloneMCV(vertex));
}

LHCb::MCVertex* MCCloner::doCloneMCV(const LHCb::MCVertex* vertex) {
  LHCb::MCVertex* clone = cloneKeyedMCV(vertex);

  clone->setMother(cloneMCP(vertex->mother()));

  clone->clearProducts();

  cloneDecayProducts(vertex->products(), clone);

  return clone;
}

LHCb::MCVertices* MCCloner::getClonedMCVs() {
  auto temp = new LHCb::MCVertices();
  for (auto& v : m_mcvs) {
    temp->insert(v.second);
  }
  return temp;
}
