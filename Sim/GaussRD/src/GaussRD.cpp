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
    delete m_mc_cloner;
  }
}

//=============================================================================
// service initialization
//=============================================================================
StatusCode GaussRD::initialize() {
  // initialize the base class
  StatusCode sc = Service::initialize();
  m_mc_cloner = new MCCloner();
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
  if (m_rd_counter < m_max_rd_counter) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << " Redecay counter " << m_rd_counter << " smaller than " << m_max_rd_counter << ". Returning false." << endmsg;
    }
    m_rd_counter++;
    m_phase = 2;
    if (m_mc_cloner_copy) {
      delete m_mc_cloner_copy;
    }
    m_mc_cloner_copy = m_mc_cloner->DeepClone();
    return false;
  } else {
    if (msgLevel(MSG::DEBUG)) {
      debug() << " Redecay counter " << m_rd_counter << " equal to " << m_max_rd_counter << ". Returning true." << endmsg;
    }
    
    m_phase = 1;
    m_rd_counter = 0;
    // Won't need any of the copied objects anymore.
    // Need empty cloner for the next incoming event.
    m_mc_cloner->clear();
    if (m_mc_cloner_copy) {
      delete m_mc_cloner_copy;
    }
    m_mc_cloner_copy = nullptr;
    return true;
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
