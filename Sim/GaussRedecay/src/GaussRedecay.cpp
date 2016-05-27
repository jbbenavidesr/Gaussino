// $Id: GaussRedecay.cpp,v 1.18 2009-12-17 11:00:12 marcocle Exp $
#define GAUSSRD_CPP 1

// Include files
// from STD & STL
#include <string>

// from Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"

// local
#include "GaussRedecay.h"
#include "MCCloner.h"

//-----------------------------------------------------------------------------
// Implementation of GaussRedecay
//-----------------------------------------------------------------------------

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_SERVICE_FACTORY(GaussRedecay)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRedecay::GaussRedecay(const std::string& name, ISvcLocator* svcloc)
    : Service(name, svcloc),
      m_mc_cloner(nullptr),
      m_mc_cloner_copy(nullptr),
      m_rd_counter(0),
      m_sig_map(),
      m_sig_mom(),
      m_sig_point(),
      m_sig_id(0) {
    declareProperty("nRedecay", m_max_rd_counter = 100);
    declareProperty("Phase", m_phase = 0);
    declareProperty("RedecayMode", m_rd_mode = 0,
                    "0: Signal, 1: Everything heavier.");
    declareProperty("G4Reserve", m_g4_reserve = 100,
                    "Number of tag particles registerred to Geant4.");
}

//=============================================================================
// Destructor
//=============================================================================
GaussRedecay::~GaussRedecay() {
    if (m_mc_cloner) {
        delete m_mc_cloner;
    }
    if (m_mc_cloner_copy) {
        delete m_mc_cloner_copy;
    }
}

//=============================================================================
// service initialization
//=============================================================================
StatusCode GaussRedecay::initialize() {
    // initialize the base class
    StatusCode sc = Service::initialize();
    m_mc_cloner = nullptr;
    m_mc_cloner_copy = nullptr;
    if (sc.isFailure()) {
        return sc;
    }

    return StatusCode::SUCCESS;
}

//=============================================================================
// service finalization
//=============================================================================
StatusCode GaussRedecay::finalize() {
    if (m_mc_cloner) m_mc_cloner->clear();
    if (m_mc_cloner_copy) m_mc_cloner_copy->clear_no_deletion();
    ///  finalize the base class
    return Service::finalize();
}

//=============================================================================
// query interface
//=============================================================================
StatusCode GaussRedecay::queryInterface(const InterfaceID& id, void** ppI) {
    if (0 == ppI) {
        return StatusCode::FAILURE;  //  RETURN !!!
    } else if (IGaussRedecayCtr::interfaceID() == id) {
        *ppI = static_cast<IGaussRedecayCtr*>(this);
    } else if (IGaussRedecayStr::interfaceID() == id) {
        *ppI = static_cast<IGaussRedecayStr*>(this);
    } else {
        return Service::queryInterface(id, ppI);  //  RETURN !!!
    }

    addRef();

    return StatusCode::SUCCESS;
}

//=============================================================================
// Check if the counter is at the max value and return true if a new event
// should be generated
//=============================================================================
bool GaussRedecay::registerNewEvent() {
    // In case phase is 0, the entire redecay part should be ignored.
    if (m_phase == 0) {
        return true;
    }
    // Have to handle two different cases, need to rerun the generation in case
    // of counter==0 or counter==max, otherwise just do some cleanup.
    m_first_access = true;
    if (m_rd_counter == 0 || m_rd_counter == m_max_rd_counter) {
        if (msgLevel(MSG::DEBUG)) {
            debug() << " Redecay counter " << m_rd_counter
                    << ". Need to generate a new event." << endmsg;
        }
        // close the loop and increment already for the next event.
        m_rd_counter = 1;
        m_phase = 1;
        /*trigger new event generation and clean up. Check if the MC cloner
         * already
         * exists (should be the case except for the very first event)*/
        if (m_mc_cloner) {
            // Won't need any of the copied objects anymore.
            // Need empty cloner for the next incoming event.
            // None of the objects in that MCCloner have ever been put into the
            // TES so take care of deleting it ourselves.
            m_mc_cloner->clear();
            delete m_mc_cloner;
        }
        m_mc_cloner = new MCCloner();
        // New event has new particles to redecay so empty the map storing them.
        m_sig_map.clear();
        m_n_particles = 0;
        return true;
    } else if (m_rd_counter > 0 && m_rd_counter < m_max_rd_counter) {
        if (msgLevel(MSG::DEBUG)) {
            debug() << " Redecay counter " << m_rd_counter << " smaller than "
                    << m_max_rd_counter
                    << ". Clean up the copy and return false." << endmsg;
        }
        m_phase = 2;
        m_rd_counter++;
        if (m_mc_cloner_copy) {
            /*The content (ObjectVectors and all the Objects) have been handed*/
            /*over to the TES DO NOT attempt to delete all of them again!*/
            m_mc_cloner_copy->clear_no_deletion();
            delete m_mc_cloner_copy;
        }
        /*If the counter is >0, previous event properly filled m_mc_cloner, so
         * make*/
        /*a deep copy of all the content to be moved to the TES.*/
        m_mc_cloner_copy = m_mc_cloner->DeepClone();
        return false;
    } else {
        // This should NEVER happen, but just in case ...
        error() << "Invalid redecay counter " << m_rd_counter
                << ". Something is REALLY wrong!" << endmsg;
        return false;
    }
}

int GaussRedecay::getPhase() const { return m_phase; }

int GaussRedecay::registerForRedecay(Particle part, int pileup_id) {
    int unique_id = m_n_particles + 1 + PlaceholderPDGID;
    if (unique_id >= PlaceholderPDGID + m_g4_reserve) {
        return -1;
    }
    if(m_sig_map.find(pileup_id) == end(m_sig_map)){
      m_sig_map[pileup_id] = std::map<int, Particle>();
    }
    m_sig_map[pileup_id][unique_id] = part;
    m_n_particles++;
    return unique_id;
}

std::map<int, GaussRedecay::Particle> *GaussRedecay::getRegisteredForRedecay(){
  if(m_first_access){
    m_first_access = false;
    m_pileup_it = begin(m_sig_map);
  } else {
    m_pileup_it++;
    if(m_pileup_it==end(m_sig_map)){
      m_pileup_it = begin(m_sig_map);
    }
  }

  return &(m_pileup_it->second);
}
// Following are all the boring redirections for the MCCloner class.

LHCb::MCParticle* GaussRedecay::cloneMCP(const LHCb::MCParticle* mcp) {
    return m_mc_cloner->cloneMCP(mcp);
}
LHCb::MCParticles* GaussRedecay::getClonedMCPs() {
    return m_mc_cloner_copy->getClonedMCPs();
}

LHCb::MCVertex* GaussRedecay::cloneMCV(const LHCb::MCVertex* mcVertex) {
    return m_mc_cloner->cloneMCV(mcVertex);
}
LHCb::MCVertices* GaussRedecay::getClonedMCVs() {
    return m_mc_cloner_copy->getClonedMCVs();
}

LHCb::MCHit* GaussRedecay::cloneMCHit(const LHCb::MCHit* mchit,
                                      const std::string& vol) {
    return m_mc_cloner->cloneMCHit(mchit, vol);
}
LHCb::MCHits* GaussRedecay::getClonedMCHits(const std::string& vol) {
    return m_mc_cloner_copy->getClonedMCHits(vol);
}

LHCb::MCCaloHit* GaussRedecay::cloneMCCaloHit(const LHCb::MCCaloHit* mchit,
                                              const std::string& vol) {
    return m_mc_cloner->cloneMCCaloHit(mchit, vol);
}
LHCb::MCCaloHits* GaussRedecay::getClonedMCCaloHits(const std::string& vol) {
    return m_mc_cloner_copy->getClonedMCCaloHits(vol);
}

LHCb::MCRichHit* GaussRedecay::cloneMCRichHit(const LHCb::MCRichHit* mchit) {
    return m_mc_cloner->cloneMCRichHit(mchit);
}
LHCb::MCRichHits* GaussRedecay::getClonedMCRichHits() {
    return m_mc_cloner_copy->getClonedMCRichHits();
}

LHCb::MCRichOpticalPhoton* GaussRedecay::cloneMCRichOpticalPhoton(
    const LHCb::MCRichOpticalPhoton* mchit) {
    return m_mc_cloner->cloneMCRichOpticalPhoton(mchit);
}
LHCb::MCRichOpticalPhotons* GaussRedecay::getClonedMCRichOpticalPhotons() {
    return m_mc_cloner_copy->getClonedMCRichOpticalPhotons();
}

LHCb::MCRichSegment* GaussRedecay::cloneMCRichSegment(
    const LHCb::MCRichSegment* mchit) {
    return m_mc_cloner->cloneMCRichSegment(mchit);
}
LHCb::MCRichSegments* GaussRedecay::getClonedMCRichSegments() {
    return m_mc_cloner_copy->getClonedMCRichSegments();
}

LHCb::MCRichTrack* GaussRedecay::cloneMCRichTrack(
    const LHCb::MCRichTrack* mchit) {
    return m_mc_cloner->cloneMCRichTrack(mchit);
}
LHCb::MCRichTracks* GaussRedecay::getClonedMCRichTracks() {
    return m_mc_cloner_copy->getClonedMCRichTracks();
}

LHCb::GenCollision* GaussRedecay::cloneGenCollision(
    const LHCb::GenCollision* mchit) {
    return m_mc_cloner->cloneGenCollision(mchit);
}
LHCb::GenCollisions* GaussRedecay::getClonedGenCollisions() {
    return m_mc_cloner_copy->getClonedGenCollisions();
}
