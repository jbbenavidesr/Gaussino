// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// from GaussRD
#include "GaussRD/IGaussRDStr.h"
#include "GaussRD/IGaussRDCtr.h"

// local
#include "GaussRDCopyToService.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/GenCollision.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRDCopyToService
//
//
// 2016-03-15 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(GaussRDCopyToService)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRDCopyToService::GaussRDCopyToService(const std::string& Name,
                                           ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc),
      m_gaussRDSvcName("GaussRD"),
      m_gaussRDCtrSvc(0),
      m_gaussRDStrSvc(0) {
  declareProperty("GaussRD", m_gaussRDSvcName = "GaussRD");
  declareProperty("Particles",
                  m_particlesLocation = LHCb::MCParticleLocation::Default,
                  "Location to place the MCParticles.");
  declareProperty("Vertices",
                  m_verticesLocation = LHCb::MCVertexLocation::Default,
                  "Location to place the MCVertices.");
  declareProperty("MCHitsLocation", m_hitsLocations,
                  "Location in TES where to put resulting MCHits");
  declareProperty("MCCaloHitsLocation", m_calohitsLocations,
                  "Location in TES where to put resulting MCCaloHits");
  declareProperty("MCRichHitsLocation",
                  m_richHitsLocation = LHCb::MCRichHitLocation::Default,
                  "Location in TES where to put resulting MCRichHits");
  declareProperty(
      "MCRichOpticalPhotonsLocation",
      m_richOpticalPhotonsLocation = LHCb::MCRichOpticalPhotonLocation::Default,
      "Location in TES where to put resulting MCRichOpticalPhotons");
  declareProperty("MCRichSegmentsLocation",
                  m_richSegmentsLocation = LHCb::MCRichSegmentLocation::Default,
                  "Location in TES where to put resulting MCRichSegments");
  declareProperty("MCRichTracksLocation",
                  m_richTracksLocation = LHCb::MCRichTrackLocation::Default,
                  "Location in TES where to put resulting MCRichTracks");
  declareProperty(
      "GenCollisionLocation",
      m_GenCollisionsLocation = LHCb::GenCollisionLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
GaussRDCopyToService::~GaussRDCopyToService() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRDCopyToService::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  m_gaussRDCtrSvc = svc<IGaussRDCtr>(m_gaussRDSvcName, true);
  m_gaussRDStrSvc = svc<IGaussRDStr>(m_gaussRDSvcName, true);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRDCopyToService::execute() {
  if (nullptr == m_gaussRDCtrSvc) {
    m_gaussRDCtrSvc = svc<IGaussRDCtr>(m_gaussRDSvcName, true);
  }

  if (nullptr == m_gaussRDCtrSvc) {
    return Error(" execute(): IGaussRDCtr* points to NULL");
  }
  if (nullptr == m_gaussRDStrSvc) {
    m_gaussRDStrSvc = svc<IGaussRDStr>(m_gaussRDSvcName, true);
  }

  if (nullptr == m_gaussRDStrSvc) {
    return Error(" execute(): IGaussRDStr* points to NULL");
  }
  if (m_gaussRDCtrSvc->whatShouldIDo() != 1) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "GaussRD phase: " << m_gaussRDCtrSvc->whatShouldIDo()
              << ". Skipping..." << endmsg;
    }
  } else {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Requested copy of MC locations to service." << endmsg;
    }
  }

  auto m_genCollisionsContainer =
      get<LHCb::GenCollisions>(m_GenCollisionsLocation);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Copying " << m_genCollisionsContainer->size()
            << " GenCollisions from " << m_GenCollisionsLocation << endmsg;
  }
  for (auto& gc : *m_genCollisionsContainer) {
    m_gaussRDStrSvc->cloneGenCollision(gc);
  }

  auto m_particleContainer = get<LHCb::MCParticles>(m_particlesLocation);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Copying " << m_particleContainer->size() << " MCParticles from "
            << m_particlesLocation << endmsg;
  }
  for (auto& part : *m_particleContainer) {
    m_gaussRDStrSvc->cloneMCP(part);
  }

  auto m_vertexContainer = get<LHCb::MCVertices>(m_verticesLocation);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Copying " << m_vertexContainer->size() << " MCVertices from "
            << m_verticesLocation << endmsg;
  }
  for (auto& vrt : *m_vertexContainer) {
    m_gaussRDStrSvc->cloneMCV(vrt);
  }

  for (auto& s : m_hitsLocations) {
    auto m_hitsContainer = get<LHCb::MCHits>(s);
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Copying " << m_hitsContainer->size() << " MCHits from " << s
              << endmsg;
    }
    for (auto& hit : *m_hitsContainer) {
      m_gaussRDStrSvc->cloneMCHit(hit, s);
    }
  }

  for (auto& s : m_calohitsLocations) {
    auto m_calohitsContainer = get<LHCb::MCCaloHits>(s);
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Copying " << m_calohitsContainer->size()
              << " MCCaloHits from " << s << endmsg;
    }
    for (auto& hit : *m_calohitsContainer) {
      m_gaussRDStrSvc->cloneMCCaloHit(hit, s);
    }
  }

  auto m_richHitsContainer = get<LHCb::MCRichHits>(m_richHitsLocation);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Copying " << m_richHitsContainer->size() << " MCRichHits from "
            << m_richHitsLocation << endmsg;
  }
  for (auto& a : *m_richHitsContainer) {
    m_gaussRDStrSvc->cloneMCRichHit(a);
  }

  auto m_richOpticalPhotonsContainer =
      get<LHCb::MCRichOpticalPhotons>(m_richOpticalPhotonsLocation);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Copying " << m_richOpticalPhotonsContainer->size()
            << " MCRichOpticalPhotons from " << m_richOpticalPhotonsLocation
            << endmsg;
  }
  for (auto& a : *m_richOpticalPhotonsContainer) {
    m_gaussRDStrSvc->cloneMCRichOpticalPhoton(a);
  }

  auto m_richSegmentsContainer =
      get<LHCb::MCRichSegments>(m_richSegmentsLocation);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Copying " << m_richSegmentsContainer->size()
            << " MCRichSegments from " << m_richSegmentsLocation << endmsg;
  }
  for (auto& a : *m_richSegmentsContainer) {
    m_gaussRDStrSvc->cloneMCRichSegment(a);
  }

  auto m_richTracksContainer = get<LHCb::MCRichTracks>(m_richTracksLocation);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Copying " << m_richTracksContainer->size()
            << " MCRichTracks from " << m_richTracksLocation << endmsg;
  }
  for (auto& a : *m_richTracksContainer) {
    m_gaussRDStrSvc->cloneMCRichTrack(a);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
