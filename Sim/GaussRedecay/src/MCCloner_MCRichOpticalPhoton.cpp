#include "MCCloner.h"

LHCb::MCRichOpticalPhoton* MCCloner::getStoredMCRichOpticalPhoton(
    const LHCb::MCRichOpticalPhoton* mchit) {
  auto result = m_mcrichops.find(mchit);
  if (result == m_mcrichops.end()) {
    return nullptr;
  } else {
    return result->second;
  }
}

LHCb::MCRichOpticalPhoton* MCCloner::cloneKeyedMCRichOpticalPhoton(
    const LHCb::MCRichOpticalPhoton* mchit) {
  auto clone = getStoredMCRichOpticalPhoton(mchit);
  if (!clone) {
    // Copy all the properties. Matching mc particle will be selected
    // later.

    clone = new LHCb::MCRichOpticalPhoton();
    m_mcrichops.insert(
        std::pair<const LHCb::MCRichOpticalPhoton*, LHCb::MCRichOpticalPhoton*>(
            mchit, clone));
    // Fix the MCRichHit relation of the clone
    auto org_part = mchit->mcRichHit();

    // Assume those have been transformed before
    if (org_part) {
      auto cp_part = getStoredMCRichHit(org_part);
      if (cp_part) {
        getClonedMCRichOpticalPhotons()->insert(clone, cp_part->index());
        clone->setMcRichHit(cp_part);
      } else {
        getClonedMCRichOpticalPhotons()->insert(clone);
        // TODO: Produce an error as this should not happen if orignial
        // had a MCRichHit assigned (should have been cloned before)
      }
    }
    clone->setPdIncidencePoint(mchit->pdIncidencePoint());
    clone->setEnergyAtProduction(mchit->energyAtProduction());
    clone->setEmissionPoint(mchit->emissionPoint());
    clone->setParentMomentum(mchit->parentMomentum());
    clone->setCherenkovTheta(mchit->cherenkovTheta());
    clone->setCherenkovPhi(mchit->cherenkovPhi());
    clone->setSphericalMirrorReflectPoint(mchit->sphericalMirrorReflectPoint());
    clone->setFlatMirrorReflectPoint(mchit->flatMirrorReflectPoint());
    clone->setAerogelExitPoint(mchit->aerogelExitPoint());
    clone->setHpdQWIncidencePoint(mchit->hpdQWIncidencePoint());
  }

  return clone;
}

LHCb::MCRichOpticalPhoton* MCCloner::cloneMCRichOpticalPhoton(
    const LHCb::MCRichOpticalPhoton* mchit) {
  if (!mchit) return NULL;
  LHCb::MCRichOpticalPhoton* clone = getStoredMCRichOpticalPhoton(mchit);
  return (clone ? clone : this->doCloneMCRichOpticalPhoton(mchit));
}

LHCb::MCRichOpticalPhoton* MCCloner::doCloneMCRichOpticalPhoton(
    const LHCb::MCRichOpticalPhoton* mchit) {
  if (!mchit) return NULL;

  // Clone the MCRichOpticalPhoton
  LHCb::MCRichOpticalPhoton* clone = cloneKeyedMCRichOpticalPhoton(mchit);

  return clone;
}

LHCb::MCRichOpticalPhotons* MCCloner::getClonedMCRichOpticalPhotons() {
  if (!m_list_mcrichops) {
    m_list_mcrichops = new LHCb::MCRichOpticalPhotons();
  }
  return m_list_mcrichops;
}
