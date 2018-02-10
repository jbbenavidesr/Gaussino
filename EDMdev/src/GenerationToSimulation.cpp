// Local.
#include "GenerationToSimulation.h"
#include "MCInterfaces/IFlagSignalChain.h"

// Gaudi.
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Transform4DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Kernel.
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

#include "range/v3/all.hpp"

DECLARE_ALGORITHM_FACTORY(GenerationToSimulation)

StatusCode GenerationToSimulation::execute() {
  // Retrieve the HepMCEvents.
  debug() << "==> Execute" << endmsg;
  LHCb::HepMCEvents* generationEvents =
      get<LHCb::HepMCEvents>(m_generationLocation);

  // Create containers in TES for MCParticles and MCVertices.
  m_particleContainer = new LHCb::MCParticles();
  put(m_particleContainer, m_particlesLocation);

  m_vertexContainer = new LHCb::MCVertices();
  put(m_vertexContainer, m_verticesLocation);

  // Create some MCHeader.
  auto mcHeader = new LHCb::MCHeader();
  put(mcHeader, m_mcHeader);

  // Loop over the events (one for each pile-up interaction).
  for (auto& genEvent : *generationEvents) {
    auto ev = genEvent->pGenEvt();

    // Empty the maps of converted particles.
    m_mcParticleMap.clear();

    // Determine the position of the primary vertex.
    auto thePV = primaryVertex(ev);

    // Create and add the primary MCVertex.
    auto primaryVertex = new LHCb::MCVertex();
    m_vertexContainer->insert(primaryVertex);
    primaryVertex->setPosition(Gaudi::XYZPoint(thePV.Vect()));
    primaryVertex->setTime(thePV.T());
    primaryVertex->setType(LHCb::MCVertex::ppCollision);
    mcHeader->addToPrimaryVertices(primaryVertex);

    // Set ID of all vertices to 0.
    for (auto hepmc_vtx : ev->vertex_range()) hepmc_vtx->set_id(0);

    //FIXME: 
    std::vector<HepMC::GenParticle*> _all_particles;
    _all_particles.reserve(ev->particles_size());
    for (auto _part : ev->particle_range()) {
      _all_particles.push_back(_part);
    }

    // Extract the particles to store in MCParticles based on keep
    auto seed_particles = _all_particles | ranges::view::filter(keep) |
                          ranges::action::transform([](auto part) {
                            auto endVertex = part->end_vertex();
                            if (endVertex) endVertex->set_id(1);
                            return part;
                          }) |
                          ranges::view::filter([](auto part) {
                            return part->production_vertex()->id() == 0;
                          });

    for (auto hepmc_part : seed_particles) {
      convert(hepmc_part, primaryVertex, nullptr);
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Decides if a particle should be kept in MCParticles.
//=============================================================================
/*static*/ bool GenerationToSimulation::keep(
    const HepMC::GenParticle* particle) {
  LHCb::ParticleID pid(particle->pdg_id());
  switch (particle->status()) {
    case LHCb::HepMCEvent::StableInProdGen:
      return true;
    case LHCb::HepMCEvent::DecayedByDecayGen:
      return true;
    case LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen:
      return true;
    case LHCb::HepMCEvent::SignalInLabFrame:
      return true;
    case LHCb::HepMCEvent::StableInDecayGen:
      return true;

    // For some processes the resonance has status 3.
    case LHCb::HepMCEvent::DocumentationParticle:
      if (24 == particle->parent_event()->signal_process_id()) {
        if (23 == pid.abspid())
          return true;
        else if (25 == pid.abspid())
          return true;
      } else if (26 == particle->parent_event()->signal_process_id()) {
        if (24 == pid.abspid())
          return true;
        else if (25 == pid.abspid())
          return true;
      } else if (102 == particle->parent_event()->signal_process_id()) {
        if (25 == pid.abspid()) return true;
      } else if (6 == pid.abspid())
        return true;
      return false;
    case LHCb::HepMCEvent::Unknown:
      return false;
    case LHCb::HepMCEvent::DecayedByProdGen:
      if (pid.isHadron()) return true;
      if (pid.isLepton()) return true;
      if (pid.isNucleus()) return true;
      if (pid.isDiQuark()) return false;

      // Store particles of interest.
      switch (pid.abspid()) {
        case LHCb::ParticleID::down:
          return false;
        case LHCb::ParticleID::up:
          return false;
        case LHCb::ParticleID::strange:
          return false;
        case LHCb::ParticleID::charm:
          return true;
        case LHCb::ParticleID::bottom:
          return true;
        case LHCb::ParticleID::top:
          return false;
        case 21:
          return false;  // Gluon.
        case 22:
          return true;  // Photon.
        case 23:        // Z0.
          if (24 == particle->parent_event()->signal_process_id())
            return false;
          else
            return true;
        case 24:  // W.
          if (26 == particle->parent_event()->signal_process_id())
            return false;
          else
            return true;
        case 25:  // SM Higgs.
          if (24 == particle->parent_event()->signal_process_id() ||
              26 == particle->parent_event()->signal_process_id() ||
              102 == particle->parent_event()->signal_process_id())
            return false;
          else
            return true;
        case 32:
          return true;  // Z'.
        case 33:
          return true;  // Z''.
        case 34:
          return true;  // W'.
        case 35:
          return true;  // CP-even heavy Higgs (H0/H2).
        case 36:
          return true;  // CP-odd Higgs (A0/H3).
        case 37:
          return true;  // Charged Higgs (H+).
        // See Table 6 of the Pythia 6 manual (arxiv.org/abs/hep-ph/0603175).
        case 90:
          return false;  // System particle.
        case 91:
          return false;  // Parton system from cluster fragmentation.
        case 92:
          return false;  // Parton system from string fragmentation.
        case 93:
          return false;  // Parton system from independent fragmentation.
        case 94:
          return false;  // Time-like showering system.
        default:
          return true;
      }
      return true;
    default:
      return false;
  }
  return false;
}

//=============================================================================
// Convert a decay tree into MCParticle or to G4PrimaryParticle.
//=============================================================================
void GenerationToSimulation::convert(HepMC::GenParticle*& particle,
                                     LHCb::MCVertex* originVertex,
                                     LHCb::MCParticle* mothermcp) {
  // Decision to convert the particle.
  unsigned char conversionCode = transferToSimulation(particle);
  switch (conversionCode) {
    case 2: {  // Convert to MCParticle.

      // Check if already converted.
      const int pBarcode = particle->barcode();
      std::map<int, bool>::const_iterator result =
          m_mcParticleMap.find(pBarcode);
      if (result != m_mcParticleMap.end()) return;

      // Convert the particle.
      LHCb::MCVertex* endVertex = 0;
      LHCb::MCParticle* mcP = makeMCParticle(particle, endVertex);
      if (originVertex) {
        mcP->setOriginVertex(originVertex);
        originVertex->addToProducts(mcP);
      }
      m_mcParticleMap.insert(std::make_pair(pBarcode, true));
      mothermcp = mcP;
      originVertex = endVertex;
      break;
    }
    case 3:  // Skip the particle.
    default:
      break;
  }

  // Convert all daughters of the HepMC particle (recurse).
  auto ev = particle->end_vertex();
  if (ev) {
    // Create the list.
    std::vector<HepMC::GenParticle*> dList;

    // Sort by barcode and convert.
    for (auto P : ev->particles(HepMC::IteratorRange::children)) {
      convert(P, originVertex, mothermcp);
    }
  }
}

//=============================================================================
// Decide how to treat the particle.
// 1: convert to Geant4 for tracking in detector.
// 2: convert to MCParticle directly, the particle is not sent to Geant4.
// 3: skip the particle completely.
//=============================================================================
unsigned char GenerationToSimulation::transferToSimulation(
    const HepMC::GenParticle* p) const {
  if (!keep(p)) return 3;
  return 2;
}

//=============================================================================
// Create an MCParticle from a HepMC GenParticle.
//=============================================================================
LHCb::MCParticle* GenerationToSimulation::makeMCParticle(
    HepMC::GenParticle*& particle, LHCb::MCVertex*& endVertex) const {
  // Create and insert into TES.
  LHCb::MCParticle* mcp = new LHCb::MCParticle();
  m_particleContainer->insert(mcp);

  // Set properties.
  Gaudi::LorentzVector mom(particle->momentum());
  LHCb::ParticleID pid(particle->pdg_id());
  mcp->setMomentum(mom);
  mcp->setParticleID(pid);

  // Set the vertex.
  HepMC::GenVertex* V = particle->end_vertex();
  if (V) {
    endVertex = new LHCb::MCVertex();
    m_vertexContainer->insert(endVertex);
    endVertex->setPosition(Gaudi::XYZPoint(V->point3d()));
    endVertex->setTime(V->position().t());
    endVertex->setMother(mcp);

    // Check if the particle has oscillated.
    const HepMC::GenParticle* B = hasOscillated(particle);
    if (B) {
      endVertex->setType(LHCb::MCVertex::OscillatedAndDecay);
      particle = const_cast<HepMC::GenParticle*>(B);
    } else if ((4 == pid.abspid()) || (5 == pid.abspid()))
      endVertex->setType(LHCb::MCVertex::StringFragmentation);
    else
      endVertex->setType(LHCb::MCVertex::DecayVertex);
    mcp->addToEndVertices(endVertex);
  }

  //  Set the fromSignal flag
  if (LHCb::HepMCEvent::SignalInLabFrame == (particle->status())) {
    mcp->setFromSignal(true);
  }

  return mcp;
}

//=============================================================================
// Determine the primary vertex for the event.
//=============================================================================
Gaudi::LorentzVector GenerationToSimulation::primaryVertex(
    const HepMC::GenEvent* genEvent) const {
  Gaudi::LorentzVector result(0, 0, 0, 0);

  // First method, get the beam particle and use the decay vertex if it
  // exists.
  if (genEvent->valid_beam_particles()) {
    HepMC::GenParticle* P = genEvent->beam_particles().first;
    HepMC::GenVertex* V = P->end_vertex();
    if (V)
      result = V->position();
    else
      error() << "The beam particles have no end vertex!" << endmsg;
    // Second method, use the singal vertex stored in HepMC.
  } else if (0 != genEvent->signal_process_vertex()) {
    HepMC::GenVertex* V = genEvent->signal_process_vertex();
    result = V->position();
    // Third method, take production/end vertex of the particle with
    // barcode 1.
  } else {
    HepMC::GenParticle* P = genEvent->barcode_to_particle(1);
    HepMC::GenVertex* V = 0;
    if (P) {
      V = P->production_vertex();
      if (V)
        result = V->position();
      else {
        V = P->end_vertex();
        if (V)
          result = V->position();
        else
          error() << "The first particle has no production vertex and "
                  << "no end vertex !" << endmsg;
      }
    } else
      error() << "No particle with barcode equal to 1!" << endmsg;
  }
  return result;
}

//=============================================================================
// Compute the lifetime of a particle.
//=============================================================================
double GenerationToSimulation::lifetime(const HepMC::FourVector mom,
                                        const HepMC::GenVertex* P,
                                        const HepMC::GenVertex* E) const {
  if (!E) return 0;
  Gaudi::LorentzVector A(P->position()), B(E->position());
  Gaudi::LorentzVector AB = B - A;

  // Switch to mm for time.
  AB.SetE(AB.T() * Gaudi::Units::c_light);

  // Boost displacement 4-vector to rest frame of particle.
  Gaudi::LorentzVector M(mom);
  ROOT::Math::Boost theBoost(M.BoostToCM());
  Gaudi::LorentzVector ABStar = theBoost(AB);

  // Switch back to time.
  return ABStar.T() / Gaudi::Units::c_light;
}

//=============================================================================
// Check if a particle has oscillated.
//=============================================================================
const HepMC::GenParticle* GenerationToSimulation::hasOscillated(
    const HepMC::GenParticle* P) const {
  const HepMC::GenVertex* ev = P->end_vertex();
  if (!ev) return 0;
  if (1 != ev->particles_out_size()) return 0;
  const HepMC::GenParticle* D = *(ev->particles_out_const_begin());
  if (!D) return 0;
  if (-P->pdg_id() != D->pdg_id()) return 0;
  return D;
}
