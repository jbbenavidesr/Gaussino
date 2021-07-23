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
#pragma once

// Include files
// from Gaudi
#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/GaudiHistoAlg.h"
// from LHCb
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// from AIDA
#include "AIDA/IHistogram1D.h"

/** @class MCTruthMonitor MCTruthMonitor.h
 *
 *  Algorithm to fill reference historgam for MCParticles and MCVertices.
 *  A more detailed set is switched on with property "DetailedHistos = true".
 *
 *  @author Gloria CORTI
 *  @date   2005-08-12
 */
class MCTruthMonitor : public Gaudi::Functional::Consumer<void( const LHCb::MCParticles&, const LHCb::MCVertices& ),
                                                          Gaudi::Functional::Traits::BaseClass_t<GaudiHistoAlg>>
{
public:
  /// Standard constructor
  MCTruthMonitor( const std::string& name, ISvcLocator* pSvcLocator )
      : Consumer( name, pSvcLocator,
                  {{KeyValue{"Particles", Gaussino::MCParticleLocation::Default},
                    KeyValue{"Vertices", Gaussino::MCVertexLocation::Default}}} )
  {
    setProduceHistos( false );
  };

  virtual ~MCTruthMonitor(); ///< Destructor

  StatusCode initialize() override; ///< Algorithm initialization
  void operator()( const LHCb::MCParticles&, const LHCb::MCVertices& ) const override;
  StatusCode finalize() override; ///< Algorithm finalization

protected:
  bool detailedHistos() const { return m_detailedHistos.value(); }

private:
  Gaudi::Property<bool> m_detailedHistos{this, "DetailedHistos", false};
  mutable std::mutex m_histo_lock;
  std::string m_mcParticles;         ///< Location of the MCParticles
  std::string m_mcVertices;          ///< Location of the MCVertices
  AIDA::IHistogram1D* m_hNPart;      ///< Histo of MCParticle multiplicity
  AIDA::IHistogram1D* m_hNVert;      ///< Histo of MCVertices multiplicity
  AIDA::IHistogram1D* m_hPOrigZ;     ///< Z Origin of all particles
  AIDA::IHistogram1D* m_hPOrigT;     ///< Origin vertex type for all particles
  AIDA::IHistogram1D* m_hPMom;       ///< |P| of all particles
  AIDA::IHistogram1D* m_hPPrimMom;   ///< |P| of particles from primary vertex
  AIDA::IHistogram1D* m_hPProtMom;   ///< |P| of protons
  AIDA::IHistogram1D* m_hNProtons;   ///< p/p_bar multiplicity
  AIDA::IHistogram1D* m_hNNeutrons;  ///< n/n_bar multiplicity
  AIDA::IHistogram1D* m_hNChPions;   ///< pi+/- multiplicity
  AIDA::IHistogram1D* m_hNPiZeros;   ///< pi0 multiplicity
  AIDA::IHistogram1D* m_hNChKaons;   ///< K+/- multiplicity
  AIDA::IHistogram1D* m_hNKs;        ///< Kshort multiplicity
  AIDA::IHistogram1D* m_hNElectrons; ///< e+/- multiplicity
  AIDA::IHistogram1D* m_hNMuons;     ///< mu+/- multiplicity
  AIDA::IHistogram1D* m_hNGammas;    ///< gammas multiplicity
  AIDA::IHistogram1D* m_hNBeauty;    ///< Particles with b/b_bar quark multip.
  AIDA::IHistogram1D* m_hNCharm;     ///< Particles with c/c_bar quark multip.
  AIDA::IHistogram1D* m_hNNuclei;    ///< nuclei multiplicity
  AIDA::IHistogram1D* m_hVType;      ///< Type of all vertices
  AIDA::IHistogram1D* m_hVZpos;      ///< Zpos of all vertices (in detector)
  AIDA::IHistogram1D* m_hVZpos2;     ///< Zpos of all vertices (symm. in IP)
  AIDA::IHistogram1D* m_hVTime;      ///< Time of all vertices
};
