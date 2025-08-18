/*****************************************************************************\
 * (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
 *                                                                             *
 * This software is distributed under the terms of the Apache License          *
 * version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
 *                                                                             *
 * In applying this licence, CERN does not waive the privileges and immunities *
 * granted to it by virtue of its status as an Intergovernmental Organization  *
 * or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

#include "Defaults/Locations.h"

// from LHCb
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

#include "Gaudi/Accumulators/StaticHistogram.h"
#include "GaudiAlg/Consumer.h"

namespace Acc = Gaudi::Accumulators;

/**
 *  Algorithm to fill reference historgam for MCParticles and MCVertices.
 *  A more detailed set is switched on with property "DetailedHistos = true".
 *
 *  @author Gloria CORTI
 *  @date   2005-08-12
 */
class MCTruthMonitor : public Gaudi::Functional::Consumer<void( const LHCb::MCParticles&, const LHCb::MCVertices& )> {
public:
  MCTruthMonitor( const std::string& name, ISvcLocator* pSvcLocator )
      : Consumer( name, pSvcLocator,
                  { { "Particles", Gaussino::MCParticleLocation::Default },
                    { "Vertices", Gaussino::MCVertexLocation::Default } } ){};

  void operator()( const LHCb::MCParticles&, const LHCb::MCVertices& ) const override;

private:
  Gaudi::Property<bool> m_detailedHistos{ this, "DetailedHistos", false };

  mutable Acc::StaticHistogram<1> m_hNPart{ this, "100", "Multiplicity MCparticles", { 100, 0., 1000. } };
  mutable Acc::StaticHistogram<1> m_hNVert{ this, "200", "Multiplicity MCVertices", { 100, 0., 200. } };
  mutable Acc::StaticHistogram<1> m_hPOrigT{ this, "101", "Vertex type of particles", { 20, 0., 20. } };
  mutable Acc::StaticHistogram<1> m_hPOrigZ{ this, "102", "Z origin of all particles (mm)", { 2100, -1000., 20000. } };
  mutable Acc::StaticHistogram<1> m_hPMom{ this, "103", "Momentum of all particles (GeV)", { 200, 0., 1. } };
  mutable Acc::StaticHistogram<1> m_hPPrimMom{ this, "104", "Momentum of primary particles (GeV)", { 50, 0., 100. } };
  mutable Acc::StaticHistogram<1> m_hPProtMom{ this, "105", "Momentum of protons particles (GeV)", { 50, 0., 100. } };
  mutable Acc::StaticHistogram<1> m_hVType{ this, "201", "Vertex type", { 20, 0., 20. } };
  mutable Acc::StaticHistogram<1> m_hVZpos{ this, "202", "Z position of all vertices (mm)", { 2100, -1000., 20000. } };
  mutable Acc::StaticHistogram<1> m_hVZpos2{ this, "203", "Z position of all vertices (mm)", { 200, -10000., 10000. } };
  mutable Acc::StaticHistogram<1> m_hVTime{ this, "204", "Time of all vertices (ns)", { 120, -20., 100. } };

  mutable Acc::StaticHistogram<1> m_hNProtons{ this, "111", "Number of protons", { 50, 0., 100. } };
  mutable Acc::StaticHistogram<1> m_hNNeutrons{ this, "112", "Number of neutrons", { 50, 0., 100. } };
  mutable Acc::StaticHistogram<1> m_hNChPions{ this, "113", "Number of charged pions", { 100, 0., 200. } };
  mutable Acc::StaticHistogram<1> m_hNPiZeros{ this, "114", "Number of pi zeros", { 100, 0., 200. } };
  mutable Acc::StaticHistogram<1> m_hNChKaons{ this, "115", "Number of charged kaons", { 50, 0., 50. } };
  mutable Acc::StaticHistogram<1> m_hNKs{ this, "116", "Number of Ks", { 50, 0., 50. } };
  mutable Acc::StaticHistogram<1> m_hNElectrons{ this, "117", "Number of e+/e-", { 100, 0., 200. } };
  mutable Acc::StaticHistogram<1> m_hNMuons{ this, "118", "Number of mu+mu-", { 100, 0., 100. } };
  mutable Acc::StaticHistogram<1> m_hNGammas{ this, "119", "Number of gammas", { 100, 0., 200. } };
  mutable Acc::StaticHistogram<1> m_hNBeauty{ this, "120", "Number of particles with b/b~ quark", { 10, 0., 10. } };
  mutable Acc::StaticHistogram<1> m_hNCharm{ this, "121", "Number of particles with c/c~ quark", { 20, 0., 20. } };
  mutable Acc::StaticHistogram<1> m_hNNuclei{ this, "122", "Number of nuclei", { 50, 0., 50. } };
};

// Declaration of the Algorithm Factory
DECLARE_COMPONENT( MCTruthMonitor )

void MCTruthMonitor::operator()( const LHCb::MCParticles& particles, const LHCb::MCVertices& vertices ) const {
  unsigned int nProtons = 0, nNeutrons = 0, nChPions = 0, nPiZeros = 0;
  unsigned int nChKaons = 0, nKs = 0, nElectrons = 0, nMuons = 0, nGammas = 0;
  unsigned int nBeauty = 0, nCharm = 0, nNuclei = 0;

  ++m_hNPart[particles.size()];
  LHCb::MCParticles::const_iterator ip;
  for ( ip = particles.begin(); particles.end() != ip; ++ip ) {
    ++m_hPOrigZ[( *ip )->originVertex()->position().z() / Gaudi::Units::mm];
    int vtype = ( *ip )->originVertex()->type();
    if ( vtype >= 100 ) { vtype = vtype - 100 + 10; }
    ++m_hPOrigT[vtype];
    ++m_hPMom[( *ip )->p() / Gaudi::Units::GeV];
    if ( ( *ip )->originVertex()->type() == LHCb::MCVertex::ppCollision ) {
      ++m_hPPrimMom[( *ip )->p() / Gaudi::Units::GeV];
    }
    LHCb::ParticleID id = ( *ip )->particleID();
    if ( id.pid() == 2212 ) {
      if ( ( *ip )->p() >= 5.0 * Gaudi::Units::TeV ) {
        ++m_hPProtMom[10 * ( ( *ip )->p() ) / ( Gaudi::Units::TeV / Gaudi::Units::GeV )];
      } else {
        ++m_hPProtMom[( *ip )->p() / Gaudi::Units::GeV];
      }
    }
    if ( m_detailedHistos.value() ) {
      // Find number of different particles types
      if ( id.abspid() == 2212 ) nProtons++;
      if ( id.abspid() == 2112 ) nNeutrons++;
      if ( id.abspid() == 211 ) nChPions++;
      if ( id.abspid() == 111 ) nPiZeros++;
      if ( id.abspid() == 321 ) nChKaons++;
      if ( id.abspid() == 310 ) nKs++;
      if ( id.abspid() == 11 ) nElectrons++;
      if ( id.abspid() == 13 ) nMuons++;
      if ( id.abspid() == 22 ) nGammas++;
      if ( id.hasBottom() ) nBeauty++;
      if ( id.hasCharm() ) nCharm++;
      if ( id.isNucleus() ) nNuclei++;
    }
  }
  if ( m_detailedHistos.value() ) {
    ++m_hNProtons[nProtons];
    ++m_hNNeutrons[nNeutrons];
    ++m_hNChPions[nChPions];
    ++m_hNPiZeros[nPiZeros];
    ++m_hNChKaons[nChKaons];
    ++m_hNKs[nKs];
    ++m_hNElectrons[nElectrons];
    ++m_hNMuons[nMuons];
    ++m_hNGammas[nGammas];
    ++m_hNBeauty[nBeauty];
    ++m_hNCharm[nCharm];
    ++m_hNNuclei[nNuclei];
  }

  ++m_hNVert[vertices.size()];
  LHCb::MCVertices::const_iterator iv;
  for ( iv = vertices.begin(); vertices.end() != iv; ++iv ) {
    int vtype = ( *iv )->type();
    if ( vtype >= 100 ) { vtype = vtype - 100 + 10; }
    ++m_hVType[vtype];
    ++m_hVZpos[( *iv )->position().z() / Gaudi::Units::mm];
    ++m_hVZpos2[( *iv )->position().z() / Gaudi::Units::mm];
    ++m_hVTime[( *iv )->time() / Gaudi::Units::ns];
  }
}
