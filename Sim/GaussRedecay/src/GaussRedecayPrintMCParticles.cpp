// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "GaussRedecayPrintMCParticles.h"
#include "Event/MCParticle.h"
#include "Event/MCHeader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRedecayPrintMCParticles
//
//
// 2016-03-30 : Dominik Muller
//-----------------------------------------------------------------------------

int GaussRedecayPrintMCParticles::printMCParticlesTree(LHCb::MCVertex* vtx,
                                                  int level, int counter) {
  std::string spacer = "|---";
  std::string space = "";
  for (int i = 0; i < level; i++) {
    space += spacer;
  }
  auto prds = vtx->products();
  for (auto& part : prds) {
    debug() << counter << " " << space << " " << part->particleID().pid()
            << " (PT, Eta) = (" << part->pt() << ", " << part->pseudoRapidity()
            << ")" << endmsg;
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
GaussRedecayPrintMCParticles::GaussRedecayPrintMCParticles(const std::string& Name,
                                                 ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc) {
  declareProperty("Particles",
                  m_particlesLocation = LHCb::MCParticleLocation::Default,
                  "Location to place the MCParticles.");
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
  if (!msgLevel(MSG::DEBUG)) {
    return StatusCode::SUCCESS;
  }

  auto m_particleContainer = get<LHCb::MCParticles>(m_particlesLocation);
  auto m_mcHeader = get<LHCb::MCHeader>(m_mcHeaderLocation);
  auto pvs = m_mcHeader->primaryVertices();
  debug() << "Event has " << m_particleContainer->size()
          << " MCParticles. Starting from the PV:" << endmsg;
  for (auto& pv : pvs) {
    printMCParticlesTree(pv);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
