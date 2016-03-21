// $Id: GiGa.cpp,v 1.18 2009-12-17 11:00:12 marcocle Exp $
#define GAUSSRD_CPP 1

// Include files
// from STD & STL
#include <string>
//#include <list>
//#include <vector>
//#include <algorithm>

// from Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
//#include    "GaudiKernel/IChronoStatSvc.h"
//#include    "GaudiKernel/IToolSvc.h"
//#include    "GaudiKernel/SvcFactory.h"
//#include    "GaudiKernel/MsgStream.h"
//#include    "GaudiKernel/Bootstrap.h"
//#include    "GaudiKernel/MsgStream.h"
//#include    "GaudiKernel/Stat.h"
//#include    "GaudiKernel/PropertyMgr.h"
//#include    "GaudiKernel/IRndmGenSvc.h"

// local
#include "GaussRD.h"
#include "MCCloner.h"

//-----------------------------------------------------------------------------
// Implementation of general non-inline methods from class GiGaSvc
//
// YYYY-MM-DD : I.Belyaev
//
// Last modified 2006-07-21 : G.Corti
//-----------------------------------------------------------------------------

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_SERVICE_FACTORY(GaussRD)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRD::GaussRD(const std::string& name, ISvcLocator* svcloc)
    : Service(name, svcloc), m_mc_cloner(nullptr), m_mc_cloner_copy(nullptr), m_rd_counter(0) {
  /// name of runmanager
  declareProperty("nRedecay", m_max_rd_counter = 100);
}

//=============================================================================
// Destructor
//=============================================================================
GaussRD::~GaussRD() {
  if (m_mc_cloner) {
    m_mc_cloner->clear();
    delete m_mc_cloner;
  }
}

//=============================================================================
// service initialization
//=============================================================================
StatusCode GaussRD::initialize() {
  // initialize the base class
  StatusCode sc = Service::initialize();
  m_mc_cloner = nullptr;
  m_mc_cloner_copy = nullptr;
  if (sc.isFailure()) {
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// service finalization
//=============================================================================
StatusCode GaussRD::finalize() {
  m_mc_cloner->clear();
  m_mc_cloner_copy->clear_no_deletion();
  ///  finalize the base class
  return Service::finalize();
}

//=============================================================================
// Check if the counter is at the max value and return true if a new event
// should be generated
//=============================================================================
bool GaussRD::registerNewEvent() {
  // In case phase is 0, the entire redecay part should be ignored.
  if (m_phase == 0) {
    return true;
  }
  // Have to handle two different cases, need to rerun the generation in case
  // of counter==0 or counter==max, otherwise just do some cleanup.
  if (m_rd_counter == 0 || m_rd_counter == m_max_rd_counter) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << " Redecay counter " << m_rd_counter << ". Need to generate a new event." << endmsg;
    }
    // close the loop and increment already for the next event.
    m_rd_counter = 1;
    m_phase = 1;
    // trigger new event generation and clean up
    // Check if the MC cloner already exists (should be the case except for the
    // very first event)
    if (m_mc_cloner) {
      // Won't need any of the copied objects anymore.
      // Need empty cloner for the next incoming event.
      // None of the objects in that MCCloner have ever been put into the
      // TES so take care of deleting it ourselves.
      m_mc_cloner->clear();
      delete m_mc_cloner;
    }
    m_mc_cloner = new MCCloner();
    return true;
  } else if (m_rd_counter > 0 && m_rd_counter < m_max_rd_counter) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << " Redecay counter " << m_rd_counter << " smaller than " << m_max_rd_counter << ". Clean up the copy and return false." << endmsg;
    }
    m_phase = 2;
    m_rd_counter++;
    if (m_mc_cloner_copy) {
      // The content (ObjectVectors and all the Objects) have been handed over to the TES.
      // DO NOT attempt to delete all of them again!
      m_mc_cloner_copy->clear_no_deletion();
      delete m_mc_cloner_copy;
    }
    // If the counter is >0, previous event properly filled m_mc_cloner, so make a deep copy
    // of all the content to be moved to the TES.
    m_mc_cloner_copy = m_mc_cloner->DeepClone();
    return false;
  } else {
    // This should NEVER happen, but just in case ...
    error() << "Invalid redecay counter " << m_rd_counter << ". Something is REALLY wrong!" << endmsg;
    return false;
  }
}

int GaussRD::whatShouldIDo() const { return m_phase; }

LHCb::MCParticle* GaussRD::cloneMCP(const LHCb::MCParticle* mcp) { return m_mc_cloner->cloneMCP(mcp); }
LHCb::MCParticles* GaussRD::getClonedMCPs() { return m_mc_cloner_copy->getClonedMCPs(); };

LHCb::MCVertex* GaussRD::cloneMCV(const LHCb::MCVertex* mcVertex) { return m_mc_cloner->cloneMCV(mcVertex); };
LHCb::MCVertices* GaussRD::getClonedMCVs() { return m_mc_cloner_copy->getClonedMCVs(); };

LHCb::MCHit* GaussRD::cloneMCHit(const LHCb::MCHit* mchit, const std::string& vol) { return m_mc_cloner->cloneMCHit(mchit, vol); };
LHCb::MCHits* GaussRD::getClonedMCHits(const std::string& vol) { return m_mc_cloner_copy->getClonedMCHits(vol); };

LHCb::MCCaloHit* GaussRD::cloneMCCaloHit(const LHCb::MCCaloHit* mchit, const std::string& vol) { return m_mc_cloner->cloneMCCaloHit(mchit, vol); };
LHCb::MCCaloHits* GaussRD::getClonedMCCaloHits(const std::string& vol) { return m_mc_cloner_copy->getClonedMCCaloHits(vol); }

LHCb::MCRichHit* GaussRD::cloneMCRichHit(const LHCb::MCRichHit* mchit) { return m_mc_cloner->cloneMCRichHit(mchit); };
LHCb::MCRichHits* GaussRD::getClonedMCRichHits() { return m_mc_cloner_copy->getClonedMCRichHits(); };

LHCb::MCRichOpticalPhoton* GaussRD::cloneMCRichOpticalPhoton(const LHCb::MCRichOpticalPhoton* mchit) {
  return m_mc_cloner->cloneMCRichOpticalPhoton(mchit);
};
LHCb::MCRichOpticalPhotons* GaussRD::getClonedMCRichOpticalPhotons() { return m_mc_cloner_copy->getClonedMCRichOpticalPhotons(); };

LHCb::MCRichSegment* GaussRD::cloneMCRichSegment(const LHCb::MCRichSegment* mchit) { return m_mc_cloner->cloneMCRichSegment(mchit); };
LHCb::MCRichSegments* GaussRD::getClonedMCRichSegments() { return m_mc_cloner_copy->getClonedMCRichSegments(); };

LHCb::MCRichTrack* GaussRD::cloneMCRichTrack(const LHCb::MCRichTrack* mchit) { return m_mc_cloner->cloneMCRichTrack(mchit); }
LHCb::MCRichTracks* GaussRD::getClonedMCRichTracks() { return m_mc_cloner_copy->getClonedMCRichTracks(); };
