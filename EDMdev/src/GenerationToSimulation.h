#pragma once

// Gaudi.
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Vector4DTypes.h"

// HepMC.
#include "GenEvent/HepMCUtils.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenRanges.h"

// Event.
#include "Event/HepMCEvent.h"
#include "Event/MCHeader.h"

// Event.
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// HepMC.
#include "HepMC/SimpleVector.h"

/** @class GenerationToSimulation GenerationToSimulation.h
 *  Algorithm to transmit particles from the generation phase
 *  to the simulation phase.
 *
 *  @author Gloria CORTI, Patrick ROBBE
 *  @date   2008-09-24
 *
 *  Slimmed down to allow testing for a new EDM
 *  @author Dominik Muller
 *  @date 2018-02-08
 */
class GenerationToSimulation : public GaudiAlgorithm {
  public:
  /// Standard constructor.
  GenerationToSimulation(const std::string& name, ISvcLocator* pSvcLocator):
    GaudiAlgorithm(name, pSvcLocator) {};
  virtual ~GenerationToSimulation() = default;  ///< Destructor.
  virtual StatusCode execute() override;

  private:
  /// Determine the primary vertex of the interaction.
  Gaudi::LorentzVector primaryVertex(const HepMC::GenEvent* genEvent) const;

  /// Decide if a particle has to be kept or not.
  static bool keep(const HepMC::GenParticle * particle);

  /// Convert a GenParticle either into a MCParticle or G4PrimaryParticle.
  void convert(HepMC::GenParticle*& particle, LHCb::MCVertex* originVertex,
               LHCb::MCParticle* mothermcp);

  /// Decide if the particle should be transfered to Geant4 or only MCParticle.
  unsigned char transferToSimulation(const HepMC::GenParticle* p) const;

  /// Create an MCParticle from a HepMC GenParticle.
  LHCb::MCParticle* makeMCParticle(HepMC::GenParticle*& particle,
                                   LHCb::MCVertex*& endVertex) const;

  /// Compute the lifetime of a particle.
  double lifetime(const HepMC::FourVector mom, const HepMC::GenVertex* P,
                  const HepMC::GenVertex* E) const;

  /// Check if a particle has oscillated.
  const HepMC::GenParticle* hasOscillated(const HepMC::GenParticle* P) const;

  Gaudi::Property<double> m_travelLimit{
      this, "TravelLimit", 1e-10 * Gaudi::Units::m};
  Gaudi::Property<std::string> m_generationLocation{
      this, "HepMCEventLocation", LHCb::HepMCEventLocation::Default};
  Gaudi::Property<std::string> m_particlesLocation{
      this, "Particles", LHCb::MCParticleLocation::Default};
  Gaudi::Property<std::string> m_verticesLocation{
      this, "Vertices", LHCb::MCVertexLocation::Default};
  Gaudi::Property<std::string> m_mcHeader{
      this, "MCHeader", LHCb::MCHeaderLocation::Default};

  LHCb::MCParticles* m_particleContainer = nullptr;
  LHCb::MCVertices* m_vertexContainer = nullptr;

  std::map<int, bool> m_mcParticleMap;
};
