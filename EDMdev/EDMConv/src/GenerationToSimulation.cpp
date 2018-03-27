// Local.
#include "GenerationToSimulation.h"
#include "MCInterfaces/IFlagSignalChain.h"

// Gaudi.
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Transform4DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Kernel.
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "range/v3/all.hpp"

#include "HepMC/Status.h"
#include "HepMC/VertexAttribute.h"
#include "Defaults/HepMCAttributes.h"

DECLARE_COMPONENT(GenerationToSimulation)

std::tuple<MCPARTICLES, MCVERTICES, LHCb::MCHeader> GenerationToSimulation::
operator()(const std::vector<HepMC::GenEvent>& generationEvents) const {
  // Create containers in TES for MCParticles and MCVertices.
  MCPARTICLES m_particleContainer;
  MCVERTICES m_vertexContainer;
  auto n_hepmc_particles = ranges::accumulate(
      generationEvents | ranges::view::transform([](auto& ev) {
        return ev.particles_size();
      }),
      0);
  auto n_hepmc_vertices = ranges::accumulate(
      generationEvents | ranges::view::transform([](auto& ev) {
        return ev.vertices_size();
      }),
      0);

  if (msgLevel(MSG::DEBUG)) {
    debug() << "Reserving space for " << n_hepmc_particles << "("
            << n_hepmc_vertices << ") MCParticles(MCVertices)" << endmsg;
  }

  m_particleContainer.reserve(n_hepmc_particles);
  m_particleContainer.reserve(n_hepmc_vertices);

  // Create some MCHeader.
  LHCb::MCHeader mcHeader;
  std::vector<unsigned int> pv_indices;

  // Loop over the events (one for each pile-up interaction).
  for (auto& ev: generationEvents) {

    // Determine the position of the primary vertex.
    auto thePV = primaryVertex(ev);

    // Create and add the primary MCVertex.
    m_vertexContainer.emplace_back();
    auto pv_idx = m_vertexContainer.size() - 1;
    auto& primaryVertex = m_vertexContainer.back();
    primaryVertex.setPosition(Gaudi::XYZPoint(thePV.Vect()));
    primaryVertex.setTime(thePV.T());
    primaryVertex.setType(LHCb::MCVertex::ppCollision);
    // FIXME: mcheader should properly store the reference instead

    // Those vertices are now constant (and the id has different meaning)
    // So make an external list to find ROOT vertices
    // Set ID of all vertices to 0.
    std::vector<bool> not_root_vertex(ev.vertices_size());

    auto set_prod_id_zero = [&not_root_vertex](auto part) {
      auto endVertex = part->end_vertex();
      if (endVertex) not_root_vertex.at(endVertex->id()) = true;
      return part;
    };

    auto is_prod_vtx_id_zero = [&not_root_vertex](auto part) {
      return !not_root_vertex.at(part->production_vertex()->id());
    };

    auto convert_part = [&](auto hepmc_part) {
      this->convert(hepmc_part, m_vertexContainer.at(pv_idx),
                    m_particleContainer, m_vertexContainer);
      return hepmc_part;
    };

    ranges::for_each(ev.particles(), set_prod_id_zero);

    ranges::for_each(ev.particles() | ranges::view::filter(keep) |
                         ranges::view::filter(is_prod_vtx_id_zero),
                     convert_part);
    pv_indices.push_back(pv_idx);
  }
  for (auto i : pv_indices) {
    mcHeader.addToPrimaryVertices(&m_vertexContainer.at(i));
  }

  return std::make_tuple(std::move(m_particleContainer),
                         std::move(m_vertexContainer), mcHeader);
}

//=============================================================================
// Decides if a particle should be kept in MCParticles.
//=============================================================================
/*static*/ bool GenerationToSimulation::keep(
    const HepMC::GenParticlePtr & particle) {
  LHCb::ParticleID pid(particle->pdg_id());
  // Get the signal process ID as we will need this multiple times.
  // If the IntAttribute for the process ID was not added to the event,
  // a default constructed shared pointer to IntAttribute is returned which
  // by default initialises the process ID to 0 which is identical to the
  // behaviour in HepMC2 where the default value was 0 as well.
  auto sig_proc_id =
      particle->parent_event()->attribute<HepMC::IntAttribute>( Gaussino::HepMC::Attributes::SignalProcessID )->value();
  switch (particle->status()) {
    case HepMC::Status::StableInProdGen:
      return true;
    case HepMC::Status::DecayedByDecayGen:
      return true;
    case HepMC::Status::DecayedByDecayGenAndProducedByProdGen:
      return true;
    case HepMC::Status::SignalInLabFrame:
      return true;
    case HepMC::Status::StableInDecayGen:
      return true;

    // For some processes the resonance has status 3.
    case HepMC::Status::DocumentationParticle:
      if (24 == sig_proc_id) {
        if (23 == pid.abspid())
          return true;
        else if (25 == pid.abspid())
          return true;
      } else if (26 == sig_proc_id) {
        if (24 == pid.abspid())
          return true;
        else if (25 == pid.abspid())
          return true;
      } else if (102 == sig_proc_id) {
        if (25 == pid.abspid()) return true;
      } else if (6 == pid.abspid())
        return true;
      return false;
    case HepMC::Status::Unknown:
      return false;
    case HepMC::Status::DecayedByProdGen:
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
          if (24 == sig_proc_id)
            return false;
          else
            return true;
        case 24:  // W.
          if (26 == sig_proc_id)
            return false;
          else
            return true;
        case 25:  // SM Higgs.
          if (24 == sig_proc_id ||
              26 == sig_proc_id ||
              102 == sig_proc_id)
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
void GenerationToSimulation::convert(HepMC::GenParticlePtr particle,
                                     LHCb::MCVertex& originVertex,
                                     MCPARTICLES& mcparticles,
                                     MCVERTICES& mcvertices) const {
  unsigned char conversionCode = transferToSimulation(particle);
  switch (conversionCode) {
    case 1: {  // Convert to MCParticle.

      // Convert the particle.
      makeMCParticle(particle, originVertex, mcparticles, mcvertices);
      break;
    }
    case 2: {  // Convert to MCParticle.

      // Convert the particle.
      makeMCParticle(particle, originVertex, mcparticles, mcvertices);
      break;
    }
    case 3:  // Skip the particle.
    default:
      break;
  }

  // Convert all daughters of the HepMC particle (recurse).
  auto ev = particle->end_vertex();
  if (ev) {
    // This is going to be the MCVertex just created in makeMCParticle()
    // Getting a number here because the reference will change in the loop
    // below. FIXME: this is gross
    auto end_vtx_position = mcvertices.size() - 1;
    for (auto P : ev->particles(HepMC::IteratorRange::children)) {
      auto& endvertex = mcvertices.at(end_vtx_position);
      convert(P, endvertex, mcparticles, mcvertices);
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
    const HepMC::GenParticlePtr & p) const {
  if (!keep(p)) return 3;

  // Determine the travel distance.
  auto ev = p->end_vertex();
  auto pv = p->production_vertex();
  auto  E = ev->position();
  auto  P = pv->position();
  double dist = (E - P).p3mod();

  // Skip passing to Geant4 if under travel limit.
  if (dist < m_travelLimit) return 2;
  return 1;
}

//=============================================================================
// Create an MCParticle from a HepMC GenParticle.
//=============================================================================
LHCb::MCParticle& GenerationToSimulation::makeMCParticle(
    HepMC::GenParticlePtr& particle, LHCb::MCVertex& originVertex,
    MCPARTICLES& mcparticles, MCVERTICES& mcvertices) const {
  // Create and insert into TES.
  // LHCb::MCParticle* mcp = new LHCb::MCParticle();
  mcparticles.emplace_back();
  auto& mcp = mcparticles.back();

  mcp.setOriginVertex(&originVertex);
  originVertex.addToProducts(&mcp);

  // Set properties.
  Gaudi::LorentzVector mom(particle->momentum());
  LHCb::ParticleID pid(particle->pdg_id());
  mcp.setMomentum(mom);
  mcp.setParticleID(pid);

  // Set the vertex.
  auto hepmc_endvertex = particle->end_vertex();
  if (hepmc_endvertex) {
    mcvertices.emplace_back();
    auto& endVertex = mcvertices.back();
    // position() gives a four vector instead of a three vector but implements
    // x(), y() and z() which is required for the ROOT 3D vector for which 
    // Gaudi::XYZPoint is a simple typedef
    endVertex.setPosition(Gaudi::XYZPoint(hepmc_endvertex->position()));
    endVertex.setTime(hepmc_endvertex->position().t());
    endVertex.setMother(&mcp);

    // Check if the particle has oscillated.
    auto B = hasOscillated(particle);
    if (B) {
      endVertex.setType(LHCb::MCVertex::OscillatedAndDecay);
      particle = B;
    } else if ((4 == pid.abspid()) || (5 == pid.abspid()))
      endVertex.setType(LHCb::MCVertex::StringFragmentation);
    else
      endVertex.setType(LHCb::MCVertex::DecayVertex);
    mcp.addToEndVertices(&endVertex);
  }

  //  Set the fromSignal flag
  if (HepMC::Status::SignalInLabFrame == (particle->status())) {
    mcp.setFromSignal(true);
  }

  return mcp;
}

//=============================================================================
// Determine the primary vertex for the event.
//=============================================================================
Gaudi::LorentzVector GenerationToSimulation::primaryVertex(
    const HepMC::GenEvent& genEvent) const {
  Gaudi::LorentzVector result(0, 0, 0, 0);

  // First method, get the beam particle and use the decay vertex if it
  // exists.
  auto sig_proc_vtx = genEvent.attribute<HepMC::VertexAttribute>(Gaussino::HepMC::Attributes::SignalProcessVertex)->value();
  if (genEvent.valid_beam_particles()) {
    auto P = genEvent.beam_particles().first;
    auto V = P->end_vertex();
    if (V)
      result = V->position();
    else
      error() << "The beam particles have no end vertex!" << endmsg;
    // Second method, use the singal vertex stored in HepMC.
  } else if ( sig_proc_vtx ) {
    result = sig_proc_vtx->position();
    // Third method, take production/end vertex of the particle with
    // barcode 1.
  } else {
    auto & P = *std::begin(genEvent.particles());
    HepMC::GenVertexPtr V{nullptr};
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
                                        const HepMC::GenVertexPtr & P,
                                        const HepMC::GenVertexPtr & E) const {
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
const HepMC::GenParticlePtr GenerationToSimulation::hasOscillated(
    const HepMC::GenParticlePtr & P) const {
  auto & ev = P->end_vertex();
  if (!ev) return 0;
  if (1 != ev->particles_out_size()) return 0;
  auto D = *(ev->particles_out_const_begin());
  if (!D) return 0;
  if (-P->pdg_id() != D->pdg_id()) return 0;
  return D;
}
