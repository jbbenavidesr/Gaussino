#include "GiGaMTTruth/GaussinoTrackInformation.h"

// ============================================================================
/** @file
 *  Implementation file for class : GaussTrackInformation
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/02/2001
 */
// ============================================================================

GaussTrackInformation::GaussTrackInformation(const GaussTrackInformation& right)
    : G4VUserTrackInformation(right),
      m_appendStep(right.m_appendStep),
      m_toBeStored(right.m_toBeStored),
      m_createdHit(right.m_createdHit),
      m_directParent(right.m_directParent),
      m_storeHepMC(right.m_storeHepMC),
      m_hits(right.m_hits),
      m_detInfo(right.m_detInfo) {
#ifdef GIGA_DEBUG
  GaussTrackInformationLocal::s_Counter.increment();
#endif
}

GaussTrackInformation* GaussTrackInformation::clone() const {
  return new GaussTrackInformation(*this);
}
