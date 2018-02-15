// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "CheckMCStructure.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
//-----------------------------------------------------------------------------
// Implementation file for class : CheckMCStructure
//
//
// 2016-03-30 : Dominik Muller
//-----------------------------------------------------------------------------

int CheckMCStructure::printMCParticlesTree(LHCb::MCVertex* vtx,
                                           unsigned int& n_particles,
                                           unsigned int& n_vertices, int level,
                                           int counter) const {
  n_vertices++;;
  std::string spacer = "|---";
  std::string space = "";
  for (int i = 0; i < level; i++) {
    space += spacer;
  }
  auto prds = vtx->products();
  for (auto& part : prds) {
    n_particles++;
    if (msgLevel(MSG::DEBUG)) {
      auto name = m_ppSvc->find(part->particleID())->name();

      debug() << counter << " " << space << " " << name << " (PT, Eta) = ("
              << part->pt() << ", " << part->pseudoRapidity() << ")" << endmsg;
    }
    counter++;
    auto evs = part->endVertices();
    for (auto& ev : evs) {
      counter = printMCParticlesTree(ev, n_particles, n_vertices, level + 1, counter);
    }
  }
  return counter;
}

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(CheckMCStructure)

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheckMCStructure::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc");
  if (sc.isFailure() || !m_ppSvc) {
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
void CheckMCStructure::operator()(const MCPARTICLES& mcparticles,
                                  const MCVERTICES& mcvertices,
                                  const LHCb::MCHeader& mcheader) const {
  unsigned int n_particles = 0;
  unsigned int n_vertices = 0;
  auto pvs = mcheader.primaryVertices();
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Event has " << pvs.size() << " primary vertices" << endmsg;
    debug() << "Event has " << mcparticles.size()
            << " MCParticles. Will print tarting from the PVs." << endmsg;
  }
  int iPV = 0;
  for (auto& pv : pvs) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Starting from PV #" << iPV++ << endmsg;
    }
    printMCParticlesTree(pv, n_particles, n_vertices);
  }

  if (n_vertices != mcvertices.size()) {
    error() << "Could not reach all MCVertices from the PV " << n_vertices
            << "/" << mcvertices.size() << endmsg;
  } else {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "All MCVertices are reachable!" << endmsg;
    }
  }
  if (n_particles != mcparticles.size()) {
    error() << "Could not reach all MCParticles from the PV " << n_particles
            << "/" << mcparticles.size() << endmsg;
  } else {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "All MCParticles are reachable!" << endmsg;
    }
  }
}

//=============================================================================
