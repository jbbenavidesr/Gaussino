#pragma once

// Gaudi.
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/Transformer.h"
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

typedef std::vector<LHCb::MCParticle> MCPARTICLES;
typedef std::vector<LHCb::MCVertex> MCVERTICES;

class GenerationToSimulation
    : public Gaudi::Functional::MultiTransformer<std::tuple<
          MCPARTICLES, MCVERTICES, LHCb::MCHeader>(const LHCb::HepMCEvents&)> {
  public:
  /// Standard constructor.
  GenerationToSimulation(const std::string& name, ISvcLocator* pSvcLocator)
      : MultiTransformer(
            name, pSvcLocator,
            {KeyValue{"HepMCEventLocation", LHCb::HepMCEventLocation::Default}},
            {{KeyValue{"Particles", LHCb::MCParticleLocation::Default},
             KeyValue{"Vertices", LHCb::MCVertexLocation::Default},
             KeyValue{"MCHeader", LHCb::MCHeaderLocation::Default}}}){};
  virtual ~GenerationToSimulation() = default;  ///< Destructor.
  std::tuple<MCPARTICLES, MCVERTICES, LHCb::MCHeader> operator()(const LHCb::HepMCEvents & ) const override;

  private:
  /// Determine the primary vertex of the interaction.
  Gaudi::LorentzVector primaryVertex(const HepMC::GenEvent* genEvent) const;

  /// Decide if a particle has to be kept or not.
  static bool keep(const HepMC::GenParticle* particle);

  /// Convert a GenParticle either into a MCParticle or G4PrimaryParticle.
  void convert(HepMC::GenParticle* & particle, LHCb::MCVertex & originVertex,
               MCPARTICLES & mcparticles, MCVERTICES & mcvertices ) const;

  /// Decide if the particle should be transfered to Geant4 or only MCParticle.
  unsigned char transferToSimulation(const HepMC::GenParticle* p) const;

  /// Create an MCParticle from a HepMC GenParticle, add it to the container and return
  LHCb::MCParticle& makeMCParticle(HepMC::GenParticle* & particle, LHCb::MCVertex & originVertex,
                                   MCPARTICLES & mcparticles, MCVERTICES & mcvertices) const;

  /// Compute the lifetime of a particle.
  double lifetime(const HepMC::FourVector mom, const HepMC::GenVertex* P,
                  const HepMC::GenVertex* E) const;

  /// Check if a particle has oscillated.
  const HepMC::GenParticle* hasOscillated(const HepMC::GenParticle* P) const;

  Gaudi::Property<double> m_travelLimit{this, "TravelLimit",
                                        1e-10 * Gaudi::Units::m};
};
