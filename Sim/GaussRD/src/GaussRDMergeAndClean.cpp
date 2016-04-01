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
#include "Event/GenCollision.h"

#include "MCCloner.h"
#include "GaussRD/IGaussRDStr.h"

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
  declareProperty("HepMCEventLocation",
                  m_hepMCEventLocation = LHCb::HepMCEventLocation::Default);
  declareProperty("GenCollisionLocation",
                  m_genCollisionLocation = LHCb::GenCollisionLocation::Default);
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
  auto org_signal = m_gaussRDStrSvc->getSignal();
  auto m_mcHeader =
      get<LHCb::MCHeader>(m_signal_tes_prefix + m_mcHeaderLocation);

  // Later check if we did it correctly
  auto org_pv = org_signal->primaryVertex();

  auto temp_cloner = new MCCloner();
  // As the default keys have been used when the signal containers were filled,
  // there are key conflicts if the cloned signal objects use the same key so
  // turn
  // off the cloning of the keys.
  temp_cloner->setCloneKey(false);
  auto mcparticles = get_and_print<LHCb::MCParticles>(m_particlesLocation);
  auto mcvertices = get_and_print<LHCb::MCVertices>(m_verticesLocation);
  temp_cloner->m_list_mcps = mcparticles.first;
  temp_cloner->m_list_mcvs = mcvertices.first;

  // Get the vertex and remove the const to change the decay products.
  // Then delete the original particle from the record.
  auto org_signal_vtx = const_cast<LHCb::MCVertex*>(org_signal->originVertex());
  org_signal_vtx->removeFromProducts(org_signal);
  mcparticles.first->erase(org_signal);

  // This temp cloner does not know about all the other particles from the main
  // simulation so for
  // the cloning procedure we need to remove the origin vertex of the new
  // signal to not add an additional vertex to the event which is unconnected.
  auto tmp_pv = m_mcHeader->primaryVertices();
  // Perform some sanity checks
  if (tmp_pv.size() != 1) {
    return Error("Signal does not have exactly one PV.");
  }
  auto sig_pv = *begin(tmp_pv);
  if (sig_pv->products().size() != 1) {
    return Error("Signal PV does not have exactly one child.");
  }
  auto sig_signal = *begin(sig_pv->products());
  sig_pv->removeFromProducts(sig_signal);
  sig_signal->setOriginVertex(nullptr);
  mcvertices.second->erase(sig_pv);

  // Let's create the clone and attach it to the original signal vertex. Cloning
  // is also adding the objects to the containers in the TES.
  sig_signal = temp_cloner->cloneMCP(sig_signal);
  org_signal_vtx->addToProducts(sig_signal);
  sig_signal->setOriginVertex(org_signal_vtx);
  if (org_pv != sig_signal->primaryVertex()) {
    return Error("PVs of original and replaced signal do not match!");
  }

  // Now clone all the other particles and vertices.
  for (auto& part : *mcparticles.second) {
    temp_cloner->cloneMCP(part);
  }
  for (auto& vtx : *mcvertices.second) {
    temp_cloner->cloneMCV(vtx);
  }

  //evtSvc()->unregisterObject(
      //fullTESLocation(m_signal_tes_prefix + m_mcHeaderLocation, true));
  //delete m_mcHeader;
  //evtSvc()->unregisterObject(
      //fullTESLocation(m_signal_tes_prefix + m_particlesLocation, true));
  //delete mcparticles.second;
  //evtSvc()->unregisterObject(
      //fullTESLocation(m_signal_tes_prefix + m_verticesLocation, true));
  //delete mcvertices.second;

  // The rest is straight forward as the cloner will assign the correct clones
  // for the particles which caused the hits.

  // MCHits
  for (auto& s : m_hitsLocations) {
    auto mchits = get_and_print<LHCb::MCHits>(s);
    temp_cloner->m_list_mchits[s] = mchits.first;
    for (auto& hit : *mchits.second) {
      temp_cloner->cloneMCHit(hit, s);
    }
    //evtSvc()->unregisterObject(fullTESLocation(m_signal_tes_prefix + s, true));
    //delete mchits.second;
  }

  // MCCaloHits
  for (auto& s : m_calohitsLocations) {
    auto mchits = get_and_print<LHCb::MCCaloHits>(s);
    temp_cloner->m_list_mccalohit[s] = mchits.first;
    for (auto& hit : *mchits.second) {
      temp_cloner->cloneMCCaloHit(hit, s);
    }
    //evtSvc()->unregisterObject(fullTESLocation(m_signal_tes_prefix + s, true));
    //delete mchits.second;
  }

  // MCRichHits
  auto mcrichhits = get_and_print<LHCb::MCRichHits>(m_richHitsLocation);
  temp_cloner->m_list_mcrichhits = mcrichhits.first;
  for (auto& a : *mcrichhits.second) {
    temp_cloner->cloneMCRichHit(a);
  }
  //evtSvc()->unregisterObject(
      //fullTESLocation(m_signal_tes_prefix + m_richHitsLocation, true));
  //delete mcrichhits.second;

  // MCRichOpticalPhotons
  auto mcrichops =
      get_and_print<LHCb::MCRichOpticalPhotons>(m_richOpticalPhotonsLocation);
  temp_cloner->m_list_mcrichops = mcrichops.first;
  for (auto& a : *mcrichops.second) {
    temp_cloner->cloneMCRichOpticalPhoton(a);
  }
  //evtSvc()->unregisterObject(fullTESLocation(
      //m_signal_tes_prefix + m_richOpticalPhotonsLocation, true));
  //delete mcrichops.second;

  // MCRichSegments
  auto mcrichsegs = get_and_print<LHCb::MCRichSegments>(m_richSegmentsLocation);
  temp_cloner->m_list_mcrichsegs = mcrichsegs.first;
  for (auto& a : *mcrichsegs.second) {
    temp_cloner->cloneMCRichSegment(a);
  }
  //evtSvc()->unregisterObject(
      //fullTESLocation(m_signal_tes_prefix + m_richSegmentsLocation, true));
  //delete mcrichsegs.second;

  // MCRichTracks
  auto mcrichtracks = get_and_print<LHCb::MCRichTracks>(m_richTracksLocation);
  temp_cloner->m_list_mcrichtracks = mcrichtracks.first;
  for (auto& a : *mcrichtracks.second) {
    temp_cloner->cloneMCRichTrack(a);
  }
  //evtSvc()->unregisterObject(
      //fullTESLocation(m_signal_tes_prefix + m_richTracksLocation, true));
  //delete mcrichtracks.second;

  // Now, cleanup the dummy files created: GenCollisions and the HepMCEvent
  auto colls =
      get<LHCb::GenCollisions>(m_signal_tes_prefix + m_genCollisionLocation);
  //evtSvc()->unregisterObject(
      //fullTESLocation(m_signal_tes_prefix + m_genCollisionLocation, true));
  //delete colls;
  auto hepmc =
      get<LHCb::HepMCEvents>(m_signal_tes_prefix + m_hepMCEventLocation);
  //evtSvc()->unregisterObject(
      //fullTESLocation(m_signal_tes_prefix + m_hepMCEventLocation, true));
  //delete hepmc;

  // And finally get rid of the main signal branch in the TES.
  // Should now be empty anyway.
  
  //evtSvc()->unregisterObject(fullTESLocation(m_signal_tes_prefix, true));

  temp_cloner->clear_no_deletion();
  delete temp_cloner;

  return StatusCode::SUCCESS;
}

//=============================================================================
