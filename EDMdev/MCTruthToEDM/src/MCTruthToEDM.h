#pragma once

// Gaudi.
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/Vector4DTypes.h"

// Event.
#include "Event/MCHeader.h"
#include "Event/GenHeader.h"

// Event.
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "MCTruthToEDM/LinkedParticleMCParticleLink.h"

#include "GiGaMTCoreTruth/MCTruthConverter.h"

#include "Defaults/Locations.h"

/** @class MCTruthToEDM MCTruthToEDM.h
 *  Algorithm to move create the MCParticle and MCVertex structure
 *  from the filled MCTruth objects. Based loosely on the
 *  MCTruthToEDM algorithm in Gauss
 *
 *  Also returns an additional structure which maps LinkedParticle to the
 *  converted MCParticle. This will be useful for later assigning the MCParticle
 *  to the MCHit via the LinkedParticle stored in the G4VHit
 *
 *  @author Dominik Muller
 *  @date 2018-04-09
 */

class MCTruthToEDM : public Gaudi::Functional::MultiTransformer<
                         std::tuple<LHCb::MCParticles, LHCb::MCVertices, LHCb::MCHeader, LinkedParticleMCParticleLinks>(
                             const Gaussino::MCTruthPtrs&, const LHCb::GenHeader &  )>
{
public:
  /// Standard constructor.
  MCTruthToEDM( const std::string& name, ISvcLocator* pSvcLocator )
      : MultiTransformer(
            name, pSvcLocator, {{KeyValue{"MCTruthLocation", Gaussino::MCTruthsLocation::Default},
                KeyValue{"GenHeaderLocation", Gaussino::GenHeaderLocation::Default}}},
            {{
                KeyValue{"Particles", Gaussino::MCParticleLocation::Default},
                KeyValue{"Vertices", Gaussino::MCVertexLocation::Default},
                KeyValue{"MCHeader", LHCb::MCHeaderLocation::Default},
                KeyValue{"LinkedParticleMCParticleLinks", Gaussino::LinkedParticleMCParticleLinksLocation::Default},
            }} ){};
  virtual ~MCTruthToEDM() = default; ///< Destructor.
  virtual std::tuple<LHCb::MCParticles, LHCb::MCVertices, LHCb::MCHeader, LinkedParticleMCParticleLinks>
  operator()( const Gaussino::MCTruthPtrs&, const LHCb::GenHeader& ) const override;

private:
  typedef std::set<std::pair<LHCb::MCVertex*, const HepMC3::GenVertex*>> VertexSet;
  /// Determine the primary vertex of the interaction.
  LHCb::MCVertex* FindPV( LinkedParticle* lp, VertexSet& pvs ) const;

  /// Convert a GenParticle either into a MCParticle or G4PrimaryParticle.
  class Converter
  {
  public:
    Converter( LHCb::MCParticles& mcparticles, LHCb::MCVertices& mcvertices, LinkedParticleMCParticleLinks& lpmcp_links,
               MsgStream& stm )
        : msgStream( stm ), m_particles( mcparticles ), m_vertices( mcvertices ), m_links( lpmcp_links )
    {
    }
    void convert( LinkedParticle* particle, LHCb::MCVertex* originVertex );

    LHCb::MCParticle* makeMCParticle( LinkedParticle* particle );
    // Simple helper to create a vertex at the location and add
    // it to the containers
    LHCb::MCVertex* createVertex( const HepMC3::FourVector& fm );
    // Converts a LinkedVertex to MCVertex. Figures out a sensible
    // vertex type and sets it based on the MCTruth structure.
    LHCb::MCVertex* createVertex( LinkedVertex* lv );

  private:
    MsgStream& msgStream;
    LHCb::MCParticles& m_particles;
    LHCb::MCVertices& m_vertices;
    LinkedParticleMCParticleLinks& m_links;
    std::set<LinkedVertex*> already_converted{};
  };
};
