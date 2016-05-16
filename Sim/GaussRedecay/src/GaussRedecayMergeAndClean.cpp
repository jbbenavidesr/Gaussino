// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "Event/GenCollision.h"
#include "Event/MCCaloHit.h"
#include "Event/MCHeader.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/MCVertex.h"
#include "Event/Particle.h"
#include "GaussRedecayMergeAndClean.h"

#include "LHCbMath/LHCbMath.h"
#include "MCCloner.h"

#include "GaussRedecay/IGaussRedecayStr.h"
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
        m_richOpticalPhotonsLocation =
            LHCb::MCRichOpticalPhotonLocation::Default,
        "Location in TES where to put resulting MCRichOpticalPhotons");
    declareProperty(
        "MCRichSegmentsLocation",
        m_richSegmentsLocation = LHCb::MCRichSegmentLocation::Default,
        "Location in TES where to put resulting MCRichSegments");
    declareProperty("MCRichTracksLocation",
                    m_richTracksLocation = LHCb::MCRichTrackLocation::Default,
                    "Location in TES where to put resulting MCRichTracks");
    declareProperty("SignalTESROOT", m_signal_tes_prefix = "Signal/",
                    "Root in TES of the Signal objects.");
    declareProperty("HepMCEventLocation",
                    m_hepMCEventLocation = LHCb::HepMCEventLocation::Default);
    declareProperty(
        "GenCollisionLocation",
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

    m_temp_cloner = new MCCloner();
    // As the default keys have been used when the signal containers were
    // filled,
    // there are key conflicts if the cloned signal objects use the same key so
    // turn
    // off the cloning of the keys.
    m_temp_cloner->setCloneKey(false);
    m_mcparticles = get_and_print<LHCb::MCParticles>(m_particlesLocation);
    m_mcvertices = get_and_print<LHCb::MCVertices>(m_verticesLocation);
    m_temp_cloner->m_list_mcps = m_mcparticles.first;
    m_temp_cloner->m_list_mcvs = m_mcvertices.first;


    auto sig_info = m_gaussRDStrSvc->getRegisteredForRedecay();
    for (auto& info : *sig_info) {
        fix_connections(info.first, info.second.pdg_id);
    }

    // Now clone all the other particles and vertices.
    // all the tagging particles have been removed as well as the primary
    // vertex. During the fix_connections call, some of those have already been
    // cloned, however the cloner does not clone those again.
    for (auto& part : *m_mcparticles.second) {
        m_temp_cloner->cloneMCP(part);
    }
    for (auto& vtx : *m_mcvertices.second) {
        m_temp_cloner->cloneMCV(vtx);
    }

    // MCHits
    for (auto& s : m_hitsLocations) {
        auto mchits = get_and_print<LHCb::MCHits>(s);
        m_temp_cloner->m_list_mchits[s] = mchits.first;
        for (auto& hit : *mchits.second) {
            m_temp_cloner->cloneMCHit(hit, s);
        }
    }

    // MCCaloHits
    for (auto& s : m_calohitsLocations) {
        auto mchits = get_and_print<LHCb::MCCaloHits>(s);
        m_temp_cloner->m_list_mccalohit[s] = mchits.first;
        for (auto& hit : *mchits.second) {
            m_temp_cloner->cloneMCCaloHit(hit, s);
        }
    }

    // MCRichHits
    auto mcrichhits = get_and_print<LHCb::MCRichHits>(m_richHitsLocation);
    m_temp_cloner->m_list_mcrichhits = mcrichhits.first;
    for (auto& a : *mcrichhits.second) {
        m_temp_cloner->cloneMCRichHit(a);
    }

    // MCRichOpticalPhotons
    auto mcrichops =
        get_and_print<LHCb::MCRichOpticalPhotons>(m_richOpticalPhotonsLocation);
    m_temp_cloner->m_list_mcrichops = mcrichops.first;
    for (auto& a : *mcrichops.second) {
        m_temp_cloner->cloneMCRichOpticalPhoton(a);
    }

    // MCRichSegments
    auto mcrichsegs =
        get_and_print<LHCb::MCRichSegments>(m_richSegmentsLocation);
    m_temp_cloner->m_list_mcrichsegs = mcrichsegs.first;
    for (auto& a : *mcrichsegs.second) {
        m_temp_cloner->cloneMCRichSegment(a);
    }

    // MCRichTracks
    auto mcrichtracks = get_and_print<LHCb::MCRichTracks>(m_richTracksLocation);
    m_temp_cloner->m_list_mcrichtracks = mcrichtracks.first;
    for (auto& a : *mcrichtracks.second) {
        m_temp_cloner->cloneMCRichTrack(a);
    }

    // The cloner created new memory to store the copied objects but the
    // internal lists are actually managed by the TES, so do not delete
    // the stored content.
    m_temp_cloner->clear_no_deletion();
    delete m_temp_cloner;
    m_temp_cloner = nullptr;

    return StatusCode::SUCCESS;
}

LHCb::MCVertex* GaussRedecayMergeAndClean::findVertex(int placeholder) {
    auto vtxs = m_mcvertices.first;
    auto parts = m_mcparticles.first;
    auto ph = findPlaceholder(parts, placeholder);
    if (ph) {
        auto vertex = const_cast<LHCb::MCVertex*>(ph->originVertex());
        vertex->removeFromProducts(ph);
        deleteParticle(ph, vtxs, parts);
        return vertex;
    }
    return nullptr;  // should not get here but it insists on a warning
                     // otherwise ..
}

LHCb::MCParticle* GaussRedecayMergeAndClean::findPlaceholder(
    const LHCb::MCParticles* parts, int placeholder) {
    LHCb::MCParticle* matched = nullptr;
    for (auto& o : *parts) {
        if (o->particleID().pid() != placeholder) {
            continue;
        }
        matched = o;
    }
    if (!matched) {
        error() << "Could not find placeholder " << placeholder << endmsg;
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
            LHCb::MCParticle* Particle =
                const_cast<LHCb::MCParticle*>(constParticle);
            deleteParticle(Particle, m_vertexContainer, m_particleContainer);
        }
        const LHCb::MCVertex* constV = *endV;
        LHCb::MCVertex* V = const_cast<LHCb::MCVertex*>(constV);
        m_vertexContainer->erase(V);
    }
    m_particleContainer->erase(P);
}

void GaussRedecayMergeAndClean::fix_connections(int placeholder,
                                                int original_id) {
    unsigned int abs_org_id = abs(original_id);
    // Get the vertex and remove the const to change the decay products.
    // Then delete the original particle from the record.
    auto sig_tag = findPlaceholder(m_mcparticles.second, placeholder);
    auto sig_tag_prod_vtx =
        const_cast<LHCb::MCVertex*>(sig_tag->originVertex());
    auto signal_vertex = *begin(sig_tag->endVertices());
    // This vertex should only have one 2 children, one of which is the tag, the
    // other should be the actual signal with same abs pid as stored.
    if (signal_vertex->products().size() != 1) {
        error() << "Tagged signal vertex has not exactly 1 child." << endmsg;
    }
    auto sig_signal = *begin(signal_vertex->products());
    if (sig_signal->particleID().abspid() != abs_org_id) {
        error() << "Signal from matched placeholder has wrong PDG ID!"
                << endmsg;
    }

    // This temp cloner does not know about all the other particles from the
    // main simulation so for
    // the cloning procedure we need to remove the origin vertex of the new
    // signal to not add an additional vertex to the event which is unconnected.
    signal_vertex->removeFromProducts(sig_signal);
    sig_signal->setOriginVertex(nullptr);
    // We plan to iterate over all MCParticles and MCVertices in the signal
    // event
    // and clone them later so get rid of the tag particles.
    const_cast<LHCb::MCVertex*>(sig_tag_prod_vtx)->removeFromProducts(sig_tag);
    deleteParticle(sig_tag, m_mcvertices.second, m_mcparticles.second);

    // Let's create the clone and attach it to the original signal vertex.
    // Cloning is also adding the objects to the containers in the TES.
    auto org_signal_vtx = findVertex(placeholder);
    sig_signal = m_temp_cloner->cloneMCP(sig_signal);
    org_signal_vtx->addToProducts(sig_signal);
    sig_signal->setOriginVertex(org_signal_vtx);
    if (msgLevel(MSG::DEBUG)) {
        debug() << "Treated " << original_id << " -> " << placeholder
                << " to new connection." << endmsg;
    }
    // Every treated signal particle should have removed it's tagging partner,
    // so after the last one, the tagging origin vertex should be empty. Remove
    // it from the list to not clone it.
    if (sig_tag_prod_vtx->products().size() == 0) {
        m_mcvertices.second->erase(sig_tag_prod_vtx);
        if (msgLevel(MSG::DEBUG)) {
            debug() << "That was the last one, removing fake primary vertex "
                       "from the signal part."
                    << endmsg;
        }
    }
}

//=============================================================================
