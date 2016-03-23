// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// from GaussRD
#include "GaussRD/IGaussRDCtr.h"

// local
#include "GaussRDNotDoFullFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRDNotDoFullFilter
//
//
// 2016-03-15 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(GaussRDNotDoFullFilter)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRDNotDoFullFilter::GaussRDNotDoFullFilter(const std::string& Name, ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc), m_gaussRDSvcName("GaussRD"), m_gaussRDSvc(0) {
    declareProperty("GaussRD", m_gaussRDSvcName);
}

//=============================================================================
// Destructor
//=============================================================================
GaussRDNotDoFullFilter::~GaussRDNotDoFullFilter() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRDNotDoFullFilter::initialize() {
    StatusCode sc = GaudiAlgorithm::initialize();
    if (sc.isFailure()) {
        return sc;
    }

    m_gaussRDSvc = svc<IGaussRDCtr>(m_gaussRDSvcName, true);

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRDNotDoFullFilter::execute() {
    if (nullptr == gaussRDSvc()) {
        m_gaussRDSvc = svc<IGaussRDCtr>(m_gaussRDSvcName, true);
    }

    if (nullptr == gaussRDSvc()) {
        return Error(" execute(): IGaussRDCtr* points to NULL");
    }

    if (msgLevel(MSG::DEBUG)) {
        debug() << "GaussRD phase=" << gaussRDSvc()->whatShouldIDo() << endmsg;
    }

    if (gaussRDSvc()->whatShouldIDo() <= 1) {
        if (msgLevel(MSG::DEBUG)) {
            debug() << "Setting setFilterPassed(false)" << endmsg;
        }
        setFilterPassed(false);
    } else {
        if (msgLevel(MSG::DEBUG)) {
            debug() << "Setting setFilterPassed(true)" << endmsg;
        }
        setFilterPassed(true);
    }

    return StatusCode::SUCCESS;
}

//=============================================================================
