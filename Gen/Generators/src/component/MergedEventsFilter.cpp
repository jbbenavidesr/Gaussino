// $Id: MergedEventsFilter.cpp,v 1.1 2008-05-06 08:27:55 gcorti Exp $
// Include files

// from LHCb
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"

// from Kernel
#include "GenInterfaces/IFullGenEventCutTool.h"

// local
#include "MergedEventsFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MergedEventsFilter
//
// 2008-04-30 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_COMPONENT(MergedEventsFilter)

//=============================================================================
// Main execution
//=============================================================================
void MergedEventsFilter::operator()(
    const HepMC3::GenEventPtrs& theEvents,
    const LHCb::GenCollisions& theCollisions) const {
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  // Apply generator level cut on full event
  bool goodEvent = true;
  if (m_fullGenEventCutTool) {
    goodEvent = m_fullGenEventCutTool->studyFullEvent(theEvents, theCollisions);
  }
  setFilterPassed(goodEvent);
}
