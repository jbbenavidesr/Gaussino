// $Id: GiGa.cpp,v 1.18 2009-12-17 11:00:12 marcocle Exp $
#define GIGA_GIGASVC_CPP 1

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
GaussRD::GaussRD(const std::string& name, ISvcLocator* svcloc) : Service(name, svcloc), m_rd_counter(0) {
  /// name of runmanager
  declareProperty("nRedecay", m_max_rd_counter = 100);
}

//=============================================================================
// Destructor
//=============================================================================
GaussRD::~GaussRD() {}

//=============================================================================
// service initialization
//=============================================================================
StatusCode GaussRD::initialize() {
  // initialize the base class
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// service finalization
//=============================================================================
StatusCode GaussRD::finalize() {
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
    return false;
  } else {
    if (msgLevel(MSG::DEBUG)) {
      debug() << " Redecay counter " << m_rd_counter << " equal to " << m_max_rd_counter << ". Returning true." << endmsg;
    }
    m_rd_counter = 0;
    return true;
  }
}
