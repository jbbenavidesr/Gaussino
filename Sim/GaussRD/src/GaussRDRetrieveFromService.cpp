// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// from GaussRD
#include "GaussRD/IGaussRDStr.h"
#include "GaussRD/IGaussRDCtr.h"

// local
#include "GaussRDRetrieveFromService.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRDRetrieveFromService
//
//
// 2016-03-15 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(GaussRDRetrieveFromService)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRDRetrieveFromService::GaussRDRetrieveFromService(const std::string& Name, ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc), m_gaussRDSvcName("GaussRD"), m_gaussRDStrSvc(0), m_gaussRDCtrSvc(0) {
    declareProperty("GaussRD", m_gaussRDSvcName = "GaussRD");
    declareProperty("Particles", m_particlesLocation = LHCb::MCParticleLocation::Default, "Location to place the MCParticles.");
    declareProperty("Vertices", m_verticesLocation = LHCb::MCVertexLocation::Default, "Location to place the MCVertices.");
    declareProperty("MCHitsLocation", m_hitsLocations, "Location in TES where to put resulting MCHits");
    declareProperty("MCCaloHitsLocation", m_calohitsLocations, "Location in TES where to put resulting MCCaloHits");
    declareProperty("MCRichHitsLocation", m_richHitsLocation = LHCb::MCRichHitLocation::Default, "Location in TES where to put resulting MCRichHits");
    declareProperty("MCRichOpticalPhotonsLocation", m_richOpticalPhotonsLocation = LHCb::MCRichOpticalPhotonLocation::Default,
                    "Location in TES where to put resulting MCRichOpticalPhotons");
    declareProperty("MCRichSegmentsLocation", m_richSegmentsLocation = LHCb::MCRichSegmentLocation::Default,
                    "Location in TES where to put resulting MCRichSegments");
    declareProperty("MCRichTracksLocation", m_richTracksLocation = LHCb::MCRichTrackLocation::Default,
                    "Location in TES where to put resulting MCRichTracks");
}

//=============================================================================
// Destructor
//=============================================================================
GaussRDRetrieveFromService::~GaussRDRetrieveFromService() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRDRetrieveFromService::initialize() {
    StatusCode sc = GaudiAlgorithm::initialize();
    if (sc.isFailure()) {
        return sc;
    }

    m_gaussRDStrSvc = svc<IGaussRDStr>(m_gaussRDSvcName, true);
    m_gaussRDCtrSvc = svc<IGaussRDCtr>(m_gaussRDSvcName, true);

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRDRetrieveFromService::execute() {
    if (nullptr == gaussRDStrSvc()) {
        m_gaussRDStrSvc = svc<IGaussRDStr>(m_gaussRDSvcName, true);
    }

    if (nullptr == gaussRDStrSvc()) {
        return Error(" execute(): IGaussRDCtr* points to NULL");
    }

    if (nullptr == gaussRDCtrSvc()) {
        m_gaussRDCtrSvc = svc<IGaussRDCtr>(m_gaussRDSvcName, true);
    }

    if (nullptr == gaussRDCtrSvc()) {
        return Error(" execute(): IGaussRDCtr* points to NULL");
    }

    if(gaussRDCtrSvc()->whatShouldIDo()!=2){
        if (msgLevel(MSG::DEBUG)) {
            debug() << "GaussRD phase not 2, skipping retrieval." << endmsg;
        }
        return StatusCode::SUCCESS;
    }

    auto m_particleContainer = gaussRDStrSvc()->getClonedMCPs();
    if (msgLevel(MSG::DEBUG)) {
        debug() << "Copying " << m_particleContainer->size() << " MCParticles to " << m_particlesLocation << endmsg;
    }
      put(m_particleContainer, m_particlesLocation);

    auto m_vertexContainer = gaussRDStrSvc()->getClonedMCVs();
    if (msgLevel(MSG::DEBUG)) {
        debug() << "Copying " << m_vertexContainer->size() << " MCVertices to " << m_verticesLocation << endmsg;
    }
    put(m_vertexContainer, m_verticesLocation);

    for (auto& s : m_hitsLocations) {
        auto m_hitsContainer = gaussRDStrSvc()->getClonedMCHits(s);
        if (msgLevel(MSG::DEBUG)) {
            debug() << "Copying " << m_hitsContainer->size() << " MCHits to " << s << endmsg;
        }
        put(m_hitsContainer, s);
    }

    for (auto& s : m_calohitsLocations) {
        auto m_calohitsContainer = gaussRDStrSvc()->getClonedMCCaloHits(s);
        if (msgLevel(MSG::DEBUG)) {
            debug() << "Copying " << m_calohitsContainer->size() << " MCCaloHits to " << s << endmsg;
        }
          put(m_calohitsContainer, s);
    }

    auto m_richHitsContainer = gaussRDStrSvc()->getClonedMCRichHits();
    if (msgLevel(MSG::DEBUG)) {
        debug() << "Copying " << m_richHitsContainer->size() << " MCRichHits to " << m_richHitsLocation<< endmsg;
    }
    put(m_richHitsContainer, m_richHitsLocation);

    auto m_richOpticalPhotonsContainer = gaussRDStrSvc()->getClonedMCRichOpticalPhotons();
    if (msgLevel(MSG::DEBUG)) {
        debug() << "Copying " << m_richOpticalPhotonsContainer->size() << " MCRichOpticalPhotons to " << m_richOpticalPhotonsLocation<< endmsg;
    }
    put(m_richOpticalPhotonsContainer, m_richOpticalPhotonsLocation);

    auto m_richSegmentsContainer = gaussRDStrSvc()->getClonedMCRichSegments();
    if (msgLevel(MSG::DEBUG)) {
        debug() << "Copying " << m_richSegmentsContainer->size() << " MCRichSegments to " << m_richSegmentsLocation<< endmsg;
    }
    put(m_richSegmentsContainer, m_richSegmentsLocation);

    auto m_richTracksContainer = gaussRDStrSvc()->getClonedMCRichTracks();
    if (msgLevel(MSG::DEBUG)) {
        debug() << "Copying " << m_richTracksContainer->size() << " MCRichTracks to " << m_richTracksLocation<< endmsg;
    }
    put(m_richTracksContainer, m_richTracksLocation);

    return StatusCode::SUCCESS;
}

//=============================================================================
