// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// from GaussRedecay
#include "GaussRedecay/IGaussRedecayCtr.h"

// local
#include "GaussRedecayCtrFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRedecayCtrFilter
//
//
// 2016-03-15 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(GaussRedecayCtrFilter)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRedecayCtrFilter::GaussRedecayCtrFilter(const std::string& Name, ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc),
      m_gaussRDSvcName("GaussRedecay"),
      m_gaussRDSvc(0),
      m_registerNewEvent(false),
      m_isPhase(-1),
      m_isPhaseNot(-1),
      m_setPhase(-1) {
  declareProperty("GaussRedecay", m_gaussRDSvcName);
  declareProperty("RegisterNewEvent", m_registerNewEvent = false);
  declareProperty("IsPhaseEqual", m_isPhase = -1);
  declareProperty("IsPhaseNotEqual", m_isPhaseNot = -1);
  declareProperty("SetPhase", m_setPhase = -1);
}

//=============================================================================
// Destructor
//=============================================================================
GaussRedecayCtrFilter::~GaussRedecayCtrFilter() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRedecayCtrFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  m_gaussRDSvc = svc<IGaussRedecayCtr>(m_gaussRDSvcName, true);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRedecayCtrFilter::execute() {
  if (nullptr == gaussRDSvc()) {
    m_gaussRDSvc = svc<IGaussRedecayCtr>(m_gaussRDSvcName, true);
  }

  if (nullptr == gaussRDSvc()) {
    return Error(" execute(): IGaussRedecayCtr* points to NULL");
  }

  // If this algorithm requests a new event, we call that function and ignore
  // the rest.
  if (m_registerNewEvent) {
    if (gaussRDSvc()->registerNewEvent()) {
      if (msgLevel(MSG::DEBUG)) {
        debug() << m_gaussRDSvcName << " phase = " << gaussRDSvc()->getPhase() << endmsg;
        debug() << "Setting setFilterPassed(true)" << endmsg;
      }
      setFilterPassed(true);
    } else {
      if (msgLevel(MSG::DEBUG)) {
        debug() << m_gaussRDSvcName << " phase = " << gaussRDSvc()->getPhase() << endmsg;
        debug() << "Setting setFilterPassed(false)" << endmsg;
      }
      setFilterPassed(false);
    }
    return StatusCode::SUCCESS;
  }

  // Next check that the flag is equal to the requested value:
  if (m_isPhase > -1) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Checking if " << m_gaussRDSvcName << " phase is " << m_isPhase << endmsg;
    }
    if (gaussRDSvc()->getPhase() == m_isPhase) {
      if (msgLevel(MSG::DEBUG)) {
        debug() << m_gaussRDSvcName << " phase = " << gaussRDSvc()->getPhase() << endmsg;
        debug() << "Setting setFilterPassed(true)" << endmsg;
      }
      setFilterPassed(true);
    } else {
      if (msgLevel(MSG::DEBUG)) {
        debug() << m_gaussRDSvcName << " phase = " << gaussRDSvc()->getPhase() << endmsg;
        debug() << "Setting setFilterPassed(false)" << endmsg;
      }
      setFilterPassed(false);
    }
  }

  // Next check that the flag is not equal to the requested value:
  if (m_isPhaseNot > -1) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Checking if " << m_gaussRDSvcName << " phase is not " << m_isPhaseNot << endmsg;
    }
    if (gaussRDSvc()->getPhase() != m_isPhaseNot) {
      if (msgLevel(MSG::DEBUG)) {
        debug() << m_gaussRDSvcName << " phase = " << gaussRDSvc()->getPhase() << endmsg;
        debug() << "Setting setFilterPassed(true)" << endmsg;
      }
      setFilterPassed(true);
    } else {
      if (msgLevel(MSG::DEBUG)) {
        debug() << m_gaussRDSvcName << " phase = " << gaussRDSvc()->getPhase() << endmsg;
        debug() << "Setting setFilterPassed(false)" << endmsg;
      }
      setFilterPassed(false);
    }
  }

  // Lastly, set the phase to a new value if required!

  if (m_setPhase > -1 && gaussRDSvc()->getPhase() != 0) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Setting GaussRedecay phase to " << m_setPhase << endmsg;
    }
    gaussRDSvc()->setPhase(m_setPhase);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
