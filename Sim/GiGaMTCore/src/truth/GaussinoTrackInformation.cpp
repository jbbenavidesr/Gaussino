#include "GiGaMTCoreTruth/GaussinoTrackInformation.h"

// ============================================================================
/** @file
 *  Implementation file for class : GaussTrackInformation
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/02/2001
 */
// ============================================================================

G4ThreadLocal G4Allocator<GaussinoTrackInformation> *aGaussinoTrackInformationAllocator = nullptr;

GaussinoTrackInformation::GaussinoTrackInformation(const GaussinoTrackInformation& right)
    : G4VUserTrackInformation(right),
      m_appendStep(right.m_appendStep),
      m_storeTruth(right.m_storeTruth),
      m_createdHit(right.m_createdHit),
      m_directParent(right.m_directParent),
      m_hits(right.m_hits),
      m_detInfo(right.m_detInfo) {
}
