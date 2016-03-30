#include "MCCloner.h"

LHCb::MCParticle* MCCloner::getStoredMCP(const LHCb::MCParticle* mcp) {
  auto result = m_mcps.find(mcp);
  if (result == m_mcps.end()) {
    return nullptr;
  } else {
    return result->second;
  }
}

LHCb::MCParticle* MCCloner::cloneKeyedMCP(const LHCb::MCParticle* mcp) {
  auto clone = getStoredMCP(mcp);
  if (!clone) {
    clone = mcp->clone();
    m_mcps.insert(std::pair<const LHCb::MCParticle*, LHCb::MCParticle*>(mcp, clone));
    getClonedMCPs()->insert(clone, mcp->key());
  }
  return clone;
}

LHCb::MCParticle* MCCloner::cloneMCP(const LHCb::MCParticle* mcp) {
  if (!mcp) return NULL;
  LHCb::MCParticle* clone = getStoredMCP(mcp);
  return (clone ? clone : this->doCloneMCP(mcp));
}

LHCb::MCParticle* MCCloner::doCloneMCP(const LHCb::MCParticle* mcp) {
  if (!mcp) return NULL;

  // Clone the MCParticle
  LHCb::MCParticle* clone = cloneKeyedMCP(mcp);

  // Original origin vertex
  const LHCb::MCVertex* originVertex = mcp->originVertex();

  // Should we clone the origin vertex ?
  if (cloneOriginVertex(originVertex)) {
    // Has it already been cloned
    LHCb::MCVertex* originVertexClone = getStoredMCV(originVertex);
    if (!originVertexClone) {
      originVertexClone = cloneKeyedMCV(originVertex);
      originVertexClone->clearProducts();

      // Clone the origin vertex mother
      const LHCb::MCParticle* mother = originVertex->mother();
      LHCb::MCParticle* motherClone = (mother ? cloneMCP(mother) : NULL);
      originVertexClone->setMother(motherClone);
    }

    // Add the cloned origin vertex to the cloned MCP
    clone->setOriginVertex(originVertexClone);

    // Add the cloned MCP to the cloned origin vertex, if not already there
    bool found = false;
    for (SmartRefVector<LHCb::MCParticle>::const_iterator i = originVertexClone->products().begin(); i != originVertexClone->products().end(); ++i) {
      const LHCb::MCParticle* c = *i;
      if (c == clone) {
        found = true;
        break;
      }
    }
    if (!found) {
      originVertexClone->addToProducts(clone);
    }

  } else {
    clone->setOriginVertex(NULL);
  }

  // Clone the end vertices
  clone->clearEndVertices();
  cloneDecayVertices(mcp->endVertices(), clone);

  return clone;
}

LHCb::MCParticles* MCCloner::getClonedMCPs() {
  if(!m_list_mcps){
      m_list_mcps = new LHCb::MCParticles();
  }
  return m_list_mcps;
}
