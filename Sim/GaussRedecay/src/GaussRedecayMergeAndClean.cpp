// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "GaussRedecayMergeAndClean.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/MCHeader.h"
#include "Event/GenCollision.h"

#include "MCCloner.h"
#include "GaussRedecay/IGaussRedecayStr.h"
#include "LHCbMath/LHCbMath.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRedecayMergeAndClean
//
//
// 2016-03-15 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(GaussRedecayMergeAndClean)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRedecayMergeAndClean::GaussRedecayMergeAndClean(const std::string& Name,
                                           ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc), m_gaussRDStrSvc(nullptr) {
  declareProperty("Particles",
                  m_particlesLocation = LHCb::MCParticleLocation::Default,
                  "Location to place the MCParticles.");
  declareProperty("GaussRedecay", m_gaussRDSvcName = "GaussRedecay");
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
GaussRedecayMergeAndClean::~GaussRedecayMergeAndClean() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRedecayMergeAndClean::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  m_gaussRDStrSvc = svc<IGaussRedecayStr>(m_gaussRDSvcName, true);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRedecayMergeAndClean::execute() {
  // Get all the MCParticles and MCVertices first and merge them in the main
  // container.
  // All hits are only associated to those and can be combined directly.
  // auto org_signal = m_gaussRDStrSvc->getSignal();
  auto m_mcHeader =
      get<LHCb::MCHeader>(m_signal_tes_prefix + m_mcHeaderLocation);

  auto temp_cloner = new MCCloner();
  // As the default keys have been used when the signal containers were
  // filled,
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
  auto tmp_pv = m_mcHeader->primaryVertices();
  // Perform some sanity checks
  if (tmp_pv.size() != 1) {
    return Error("Signal does not have exactly one PV.");
  }
  auto signal_vertex = *begin(tmp_pv);
  if (signal_vertex->products().size() != 1) {
    return Error("Signal PV does not have exactly one child.");
  }
  // Get all the main vertices that have the same position as the
  // signal sim PV

  auto org_signal_vtx = findVertex(mcvertices.first, mcparticles.first);

  // This temp cloner does not know about all the other particles from the
  // main
  // simulation so for
  // the cloning procedure we need to remove the origin vertex of the new
  // signal to not add an additional vertex to the event which is unconnected.
  auto sig_signal = *begin(signal_vertex->products());
  signal_vertex->removeFromProducts(sig_signal);
  sig_signal->setOriginVertex(nullptr);
  mcvertices.second->erase(signal_vertex);

  // Let's create the clone and attach it to the original signal vertex.
  // Cloning
  // is also adding the objects to the containers in the TES.
  sig_signal = temp_cloner->cloneMCP(sig_signal);
  org_signal_vtx->addToProducts(sig_signal);
  sig_signal->setOriginVertex(org_signal_vtx);

  // Now clone all the other particles and vertices.
  for (auto& part : *mcparticles.second) {
    temp_cloner->cloneMCP(part);
  }
  for (auto& vtx : *mcvertices.second) {
    temp_cloner->cloneMCV(vtx);
  }

  // MCHits
  for (auto& s : m_hitsLocations) {
    auto mchits = get_and_print<LHCb::MCHits>(s);
    temp_cloner->m_list_mchits[s] = mchits.first;
    for (auto& hit : *mchits.second) {
      temp_cloner->cloneMCHit(hit, s);
    }
  }

  // MCCaloHits
  for (auto& s : m_calohitsLocations) {
    auto mchits = get_and_print<LHCb::MCCaloHits>(s);
    temp_cloner->m_list_mccalohit[s] = mchits.first;
    for (auto& hit : *mchits.second) {
      temp_cloner->cloneMCCaloHit(hit, s);
    }
  }

  // MCRichHits
  auto mcrichhits = get_and_print<LHCb::MCRichHits>(m_richHitsLocation);
  temp_cloner->m_list_mcrichhits = mcrichhits.first;
  for (auto& a : *mcrichhits.second) {
    temp_cloner->cloneMCRichHit(a);
  }

  // MCRichOpticalPhotons
  auto mcrichops =
      get_and_print<LHCb::MCRichOpticalPhotons>(m_richOpticalPhotonsLocation);
  temp_cloner->m_list_mcrichops = mcrichops.first;
  for (auto& a : *mcrichops.second) {
    temp_cloner->cloneMCRichOpticalPhoton(a);
  }

  // MCRichSegments
  auto mcrichsegs = get_and_print<LHCb::MCRichSegments>(m_richSegmentsLocation);
  temp_cloner->m_list_mcrichsegs = mcrichsegs.first;
  for (auto& a : *mcrichsegs.second) {
    temp_cloner->cloneMCRichSegment(a);
  }

  // MCRichTracks
  auto mcrichtracks = get_and_print<LHCb::MCRichTracks>(m_richTracksLocation);
  temp_cloner->m_list_mcrichtracks = mcrichtracks.first;
  for (auto& a : *mcrichtracks.second) {
    temp_cloner->cloneMCRichTrack(a);
  }

  temp_cloner->clear_no_deletion();
  delete temp_cloner;

  return StatusCode::SUCCESS;
}

LHCb::MCVertex* GaussRedecayMergeAndClean::findVertex(LHCb::MCVertices* vtxs,
                                                 LHCb::MCParticles* parts) {
  std::vector<LHCb::MCVertex*> matched_vertices;
  /*auto signal_vertex_position = m_gaussRDStrSvc->getSignalOrigin();*/
  /*
   * Option 1: Find the placeholder.
   */
  auto ph = findPlaceholder(parts);
  if (ph) {
    auto vertex = const_cast<LHCb::MCVertex*>(ph->originVertex());
    vertex->removeFromProducts(ph);
    deleteParticle(ph, vtxs, parts);
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Returning matched via placeholder." << endmsg;
    }
    return vertex;
  }
  /*
   * Option 1-3: Position matched vertices
   */
  /*for (auto& vtx : *vtxs) {*/
    /*auto vtx_position = vtx->position();*/
    /*bool equ_d = true;*/
    /*equ_d = equ_d && LHCb::Math::knuth_equal_to_double(*/
                         /*signal_vertex_position.X(), vtx_position.X());*/
    /*equ_d = equ_d && LHCb::Math::knuth_equal_to_double(*/
                         /*signal_vertex_position.Y(), vtx_position.Y());*/
    /*equ_d = equ_d && LHCb::Math::knuth_equal_to_double(*/
                         /*signal_vertex_position.Z(), vtx_position.Z());*/
    /*if (!equ_d) {*/
      /*continue;*/
    /*}*/
    /*// Only consider real vertices, not Geant4 interaction stuff*/
    /*if (vtx->type() == 0 || vtx->type() >= 100) {*/
      /*continue;*/
    /*}*/

    /*matched_vertices.push_back(vtx);*/
  /*}*/

  /*
   * Option 2: Position matched vertex
   */
  /*if (matched_vertices.size() == 1) {*/
    /*warning() << "Returning position matched vertex." << endmsg;*/
    /*return *begin(matched_vertices);*/
  /*}*/

  /*
   * Option 3: First position matched vertex
   */
  /*if (matched_vertices.size() > 1) {*/
    /*if (msgLevel(MSG::DEBUG)) {*/
      /*warning() << "Multiple position matched vertices, returning first vertex."*/
                /*<< endmsg;*/
    /*}*/
    /*return *begin(matched_vertices);*/
  /*}*/

  /*
   * Option 4: Creating a new vertex
   */
  /*if (matched_vertices.size() == 0) {*/
    /*if (msgLevel(MSG::DEBUG)) {*/
      /*warning() << "Could not find vertex. That is not normal." << endmsg;*/
      /*warning() << "Creating a new one." << endmsg;*/
    /*}*/
    /*auto vertex = new LHCb::MCVertex();*/
    /*vertex->setTime(signal_vertex_position.T());*/
    /*vertex->setPosition(Gaudi::XYZPoint(signal_vertex_position.x(),*/
                                        /*signal_vertex_position.y(),*/
                                        /*signal_vertex_position.z()));*/
    /*vtxs->insert(vertex);*/
    /*return vertex;*/
  /*}*/
  return nullptr;  // should not get here but it insists on a warning otherwise ..
}

LHCb::MCParticle* GaussRedecayMergeAndClean::findPlaceholder(
    const LHCb::MCParticles* parts) {
  LHCb::MCParticle* matched = nullptr;
  for (auto& o : *parts) {
    if (o->particleID().pid() != 424242) {
      continue;
    }
    matched = o;
  }
  return matched;
}

//=============================================================================
// Delete a particle and all decay tree
//=============================================================================
void GaussRedecayMergeAndClean::deleteParticle(
    LHCb::MCParticle* P, LHCb::MCVertices* m_vertexContainer,
    LHCb::MCParticles* m_particleContainer) {
  for (SmartRefVector<LHCb::MCVertex>::const_iterator endV =
           P->endVertices().begin();
       P->endVertices().end() != endV; ++endV) {
    for (SmartRefVector<LHCb::MCParticle>::const_iterator prod =
             (*endV)->products().begin();
         prod != (*endV)->products().end(); ++prod) {
      const LHCb::MCParticle* constParticle = *prod;
      LHCb::MCParticle* Particle = const_cast<LHCb::MCParticle*>(constParticle);
      deleteParticle(Particle, m_vertexContainer, m_particleContainer);
    }
    const LHCb::MCVertex* constV = *endV;
    LHCb::MCVertex* V = const_cast<LHCb::MCVertex*>(constV);
    m_vertexContainer->erase(V);
  }
  m_particleContainer->erase(P);
}

//=============================================================================
