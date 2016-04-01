// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "GaussRDMergeAndClean.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/MCHeader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRDMergeAndClean
//
//
// 2016-03-15 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(GaussRDMergeAndClean)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRDMergeAndClean::GaussRDMergeAndClean(const std::string& Name,
                                           ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc), m_gaussRDStrSvc(nullptr) {
  declareProperty("Particles",
                  m_particlesLocation = LHCb::MCParticleLocation::Default,
                  "Location to place the MCParticles.");
  declareProperty("GaussRD", m_gaussRDSvcName = "GaussRD");
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
  declareProperty("MCHeader",
                  m_mcHeaderLocation = LHCb::MCHeaderLocation::Default,
                  "Location of the mc header");
  declareProperty("SignalTESROOT", m_signal_tes_prefix = "Signal/",
                  "Root in TES of the Signal objects.");
}

//=============================================================================
// Destructor
//=============================================================================
GaussRDMergeAndClean::~GaussRDMergeAndClean() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRDMergeAndClean::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  m_gaussRDStrSvc = svc<IGaussRDStr>(m_gaussRDSvcName, true);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRDMergeAndClean::execute() {
  // Get all the MCParticles and MCVertices first and merge them in the main
  // container.
  // All hits are only associated to those and can be combined directly.
  auto mcparticles = get_and_print<LHCb::MCParticles>(m_particlesLocation);
  for(auto & sp: *mcparticles.second){
    mcparticles.first->insert(sp);
    sp->pt();
  }
  mcparticles.first->erase(*mcparticles.first->begin());


  return StatusCode::SUCCESS;
}

//=============================================================================
