// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "CheckMCStructure.h"
#include "Event/MCHeader.h"
#include "Event/MCParticle.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
//-----------------------------------------------------------------------------
// Implementation file for class : CheckMCStructure
//
//
// 2016-03-30 : Dominik Muller
//-----------------------------------------------------------------------------


int CheckMCStructure::printMCParticlesTree(LHCb::MCVertex* vtx, int level,
                                           int counter) {
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
      auto name = m_ppSvc->find(part->particleID())->name();

      debug() << counter << " " << space << " " << name 
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
DECLARE_ALGORITHM_FACTORY(CheckMCStructure)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckMCStructure::CheckMCStructure(const std::string& Name, ISvcLocator* SvcLoc)
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
// Initialization
//=============================================================================
StatusCode CheckMCStructure::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  m_ppSvc = 
    svc< LHCb::IParticlePropertySvc >( "LHCb::ParticlePropertySvc" ) ;
  if (sc.isFailure() || !m_ppSvc) {
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckMCStructure::execute() {
  m_vertices.clear();
  m_particles.clear();
  auto m_particleContainer = get<LHCb::MCParticles>(m_particlesLocation);
  auto m_verticeContainer = get<LHCb::MCVertices>(m_verticeLocation);
  auto m_mcHeader = get<LHCb::MCHeader>(m_mcHeaderLocation);
  auto pvs = m_mcHeader->primaryVertices();
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Event has " << pvs.size() << " primary vertices" << endmsg;
    debug() << "Event has " << m_particleContainer->size()
            << " MCParticles. Will print tarting from the PVs." << endmsg;
  }
  int iPV = 0;
  for (auto& pv : pvs) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Starting from PV #" << iPV++ << endmsg;
    }
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
