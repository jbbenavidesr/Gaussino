// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "Event/MCHeader.h"
#include "Event/MCParticle.h"
#include "GaussRedecayPrintMCParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRedecayPrintMCParticles
//
//
// 2016-03-30 : Dominik Muller
//-----------------------------------------------------------------------------

int GaussRedecayPrintMCParticles::printMCParticlesTree(LHCb::MCVertex* vtx,
                                                       int level, int counter) {
  m_vertices.insert(vtx);
  std::string spacer = "|---";
  std::string space = "";
  for (int i = 0; i < level; i++) {
    space += spacer;
  }
  auto prds = vtx->products();
  for (auto& part : prds) {
    m_particles.insert(part);
    if (msgLevel(MSG::DEBUG)) {
      debug() << counter << " " << space << " " << part->particleID().pid()
              << " (PT, Eta) = (" << part->pt() << ", "
              << part->pseudoRapidity() << ")" << endmsg;
    }
    counter++;
    auto evs = part->endVertices();
    for (auto& ev : evs) {
      counter = printMCParticlesTree(ev, level + 1, counter);
    }
  }
  return counter;
}

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(GaussRedecayPrintMCParticles)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRedecayPrintMCParticles::GaussRedecayPrintMCParticles(
    const std::string& Name, ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc) {
  declareProperty("Particles",
                  m_particlesLocation = LHCb::MCParticleLocation::Default,
                  "Location to place the MCParticles.");
  declareProperty("Vertices",
                  m_verticeLocation = LHCb::MCVertexLocation::Default,
                  "Location to place the MCVertices.");
  declareProperty("MCHeader",
                  m_mcHeaderLocation = LHCb::MCHeaderLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
GaussRedecayPrintMCParticles::~GaussRedecayPrintMCParticles() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRedecayPrintMCParticles::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRedecayPrintMCParticles::execute() {
  m_vertices.clear();
  m_particles.clear();
  auto m_particleContainer = get<LHCb::MCParticles>(m_particlesLocation);
  auto m_verticeContainer = get<LHCb::MCVertices>(m_verticeLocation);
  auto m_mcHeader = get<LHCb::MCHeader>(m_mcHeaderLocation);
  auto pvs = m_mcHeader->primaryVertices();
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Event has " << m_particleContainer->size()
            << " MCParticles. Starting from the PVs:" << endmsg;
  }
  for (auto& pv : pvs) {
    printMCParticlesTree(pv);
  }

  if (m_vertices.size() != m_verticeContainer->size()) {
    error() << "Could not reach all MCVertices from the PV "
            << m_vertices.size() << "/" << m_verticeContainer->size() << endmsg;
  } else {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "All MCVertices are reachable!" << endmsg;
    }
  }
  if (m_particles.size() != m_particleContainer->size()) {
    error() << "Could not reach all MCParticles from the PV "
            << m_particles.size() << "/" << m_particleContainer->size()
            << endmsg;
  } else {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "All MCParticles are reachable!" << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
