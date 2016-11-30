// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// from GaussRedecay
#include "GaussRedecay/IGaussRedecayStr.h"
#include "GaussRedecay/IGaussRedecayCtr.h"

// local
#include "GaussRedecayRetrieveFromService.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCHeader.h"
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRedecayRetrieveFromService
//
//
// 2016-03-15 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(GaussRedecayRetrieveFromService)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRedecayRetrieveFromService::GaussRedecayRetrieveFromService(const std::string& Name,
                                                       ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc),
      m_gaussRDSvcName("GaussRedecay"),
      m_gaussRDStrSvc(0),
      m_gaussRDCtrSvc(0) {
  declareProperty("GaussRedecay", m_gaussRDSvcName = "GaussRedecay");
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
  declareProperty("MCHeader", m_mcHeader = LHCb::MCHeaderLocation::Default,
		  "Location to retrieve the MCHeader.");
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRedecayRetrieveFromService::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  m_gaussRDStrSvc = svc<IGaussRedecayStr>(m_gaussRDSvcName, true);
  m_gaussRDCtrSvc = svc<IGaussRedecayCtr>(m_gaussRDSvcName, true);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRedecayRetrieveFromService::execute() {
  if (nullptr == gaussRDStrSvc()) {
    m_gaussRDStrSvc = svc<IGaussRedecayStr>(m_gaussRDSvcName, true);
  }

  if (nullptr == gaussRDStrSvc()) {
    return Error(" execute(): IGaussRedecayCtr* points to NULL");
  }

  if (nullptr == gaussRDCtrSvc()) {
    m_gaussRDCtrSvc = svc<IGaussRedecayCtr>(m_gaussRDSvcName, true);
  }

  if (nullptr == gaussRDCtrSvc()) {
    return Error(" execute(): IGaussRedecayCtr* points to NULL");
  }

  if (gaussRDCtrSvc()->getPhase() != 2) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "GaussRedecay phase=" << gaussRDCtrSvc()->getPhase()
              << " not 2, skipping retrieval." << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  auto m_genCollisionsContainer = gaussRDStrSvc()->getClonedGenCollisions();
  if (test_print_put(m_genCollisionsContainer, m_GenCollisionsLocation)
          .isFailure()) {
    return StatusCode::FAILURE;
  };

  auto m_particleContainer = gaussRDStrSvc()->getClonedMCPs();
  if (test_print_put(m_particleContainer, m_particlesLocation).isFailure()) {
    return StatusCode::FAILURE;
  };

  auto m_vertexContainer = gaussRDStrSvc()->getClonedMCVs();
  if (test_print_put(m_vertexContainer, m_verticesLocation).isFailure()) {
    return StatusCode::FAILURE;
  };

  // Add the primary vertex to the header file.
  LHCb::MCHeader* mcHeader = get<LHCb::MCHeader>(m_mcHeader);
  for(auto&vtx:*m_vertexContainer){
      if(vtx->type() == LHCb::MCVertex::ppCollision){
          mcHeader->addToPrimaryVertices(vtx);
      }
  }

  for (auto& s : m_hitsLocations) {
    auto m_hitsContainer = gaussRDStrSvc()->getClonedMCHits(s);
    if (test_print_put(m_hitsContainer, s).isFailure()) {
      return StatusCode::FAILURE;
    };
  }

  for (auto& s : m_calohitsLocations) {
    auto m_calohitsContainer = gaussRDStrSvc()->getClonedMCCaloHits(s);
    if (test_print_put(m_calohitsContainer, s).isFailure()) {
      return StatusCode::FAILURE;
    };
  }

  auto m_richHitsContainer = gaussRDStrSvc()->getClonedMCRichHits();
  if (test_print_put(m_richHitsContainer, m_richHitsLocation).isFailure()) {
    return StatusCode::FAILURE;
  };

  auto m_richOpticalPhotonsContainer =
      gaussRDStrSvc()->getClonedMCRichOpticalPhotons();
  if (test_print_put(m_richOpticalPhotonsContainer,
                     m_richOpticalPhotonsLocation)
          .isFailure()) {
    return StatusCode::FAILURE;
  };

  auto m_richSegmentsContainer = gaussRDStrSvc()->getClonedMCRichSegments();
  if (test_print_put(m_richSegmentsContainer, m_richSegmentsLocation)
          .isFailure()) {
    return StatusCode::FAILURE;
  };

  auto m_richTracksContainer = gaussRDStrSvc()->getClonedMCRichTracks();
  if (test_print_put(m_richTracksContainer, m_richTracksLocation).isFailure()) {
    return StatusCode::FAILURE;
  };

  return StatusCode::SUCCESS;
}

//=============================================================================
