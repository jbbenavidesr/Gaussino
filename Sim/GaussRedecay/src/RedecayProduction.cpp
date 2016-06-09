// Gaudi.
#include "Kernel/IParticlePropertySvc.h"

// Event.
#include "Event/GenCollision.h"

// HepMC.

// local
#include "GaussRedecay/IGaussRedecayStr.h"
#include "RedecayProduction.h"

//-----------------------------------------------------------------------------
// Implementation file for class: RedecayProduction
//
// 2016-04-28: Dominik Muller
//-----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(RedecayProduction)

//=============================================================================
// Default constructor.
//=============================================================================
RedecayProduction::RedecayProduction(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : GaudiTool(type, name, parent), m_gaussRDStrSvc(nullptr) {
  declareInterface<IProductionTool>(this);
  declareProperty("GaussRedecay", m_gaussRDSvcName = "GaussRedecay");
}

RedecayProduction::~RedecayProduction() {}

StatusCode RedecayProduction::initialize() {
  // Print the initialization banner.
  always() << "============================================================="
           << "=====" << endmsg;
  always() << "Using as production engine " << this->type() << endmsg;
  always() << "============================================================="
           << "=====" << endmsg;

  // Initialize the Gaudi tool.
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) Exception("Failed to initialize the Gaudi tool.");

  m_gaussRDStrSvc = svc<IGaussRedecayStr>(m_gaussRDSvcName, true);
  if (nullptr == m_gaussRDStrSvc) {
    return Error(" initializeGenerator(): IGaussRedecayStr* points to NULL");
  }

  return sc;
}

StatusCode RedecayProduction::finalize() {
  if (m_gaussRDStrSvc != nullptr) release(m_gaussRDStrSvc);
  return GaudiTool::finalize();
}

StatusCode RedecayProduction::generateEvent(
    HepMC::GenEvent* theEvent, LHCb::GenCollision* /*theCollision*/) {
  // Let's construct a fake event
  auto sig_info = m_gaussRDStrSvc->getRegisteredForRedecay();
  for (auto& part : *sig_info) {
    auto mom = part.second.momentum;
    auto origin = part.second.point;
    auto thePdgId = part.second.pdg_id;
    HepMC::GenVertex* dummy_vertex = nullptr;
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Making a particle for PDG ID " << thePdgId
              << " with placeholder ID" << part.first << endmsg;
      debug() << "#### Momentum (PT, Eta, Phi, E) = (" << mom.pt() << ", "
              << mom.eta() << ", " << mom.phi() << ", " << mom.E() << ")"
              << endmsg;
    }
    // create HepMC Vertex
    auto v = new HepMC::GenVertex(
        HepMC::FourVector(origin.X(), origin.Y(), origin.Z(), origin.T()));
    if (dummy_vertex == nullptr) {
      dummy_vertex = new HepMC::GenVertex(
          HepMC::FourVector(origin.X(), origin.Y(), origin.Z(), origin.T()));
      auto beam =
          new HepMC::GenParticle(HepMC::FourVector(0, 0, 1, 1), 0,
                                 LHCb::HepMCEvent::DocumentationParticle);
      dummy_vertex->add_particle_in(beam);
      theEvent->add_vertex(dummy_vertex);
      if (!theEvent->valid_beam_particles()) {
        theEvent->set_beam_particles(beam, beam);
      }
    }
    // create HepMC particle
    auto p = new HepMC::GenParticle(
        HepMC::FourVector(mom.Px(), mom.Py(), mom.Pz(), mom.E()), thePdgId,
        LHCb::HepMCEvent::StableInProdGen);
    // Create tagging HepMC particle and attach to the same vertex
    auto tag = new HepMC::GenParticle(
        HepMC::FourVector(mom.Px(), mom.Py(), mom.Pz(), mom.P()), part.first,
        LHCb::HepMCEvent::StableInDecayGen);
    v->add_particle_in(tag);
    dummy_vertex->add_particle_out(tag);

    v->add_particle_out(p);
    theEvent->add_vertex(v);
  }

  return StatusCode::SUCCESS;
}
