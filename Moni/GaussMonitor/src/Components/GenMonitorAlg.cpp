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

#include "GaussGenUtil.h"

// From LHCb
#include "Kernel/ParticleID.h"

#include "Defaults/HepMCAttributes.h"
#include "Defaults/Locations.h"

#include "Gaudi/Accumulators/StaticHistogram.h"
#include "GaudiAlg/Consumer.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMCUser/typedefs.h"

#include <atomic>
#include <mutex>
#include <string>

namespace Acc = Gaudi::Accumulators;

/**
 *  Monitoring algorithms for the generator sequences
 *
 *  @author Patrick Robbe (modified G.Corti)
 *  @date   2005-04-11
 *
 *  Modifications for function framework and HepMC3
 *
 *  @author Dominik Muller
 *  @date   2018-03-08
 */
class GenMonitorAlg : public Gaudi::Functional::Consumer<void( const HepMC3::GenEventPtrs& )> {
public:
  GenMonitorAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : Consumer( name, pSvcLocator, { KeyValue{ "Input", Gaussino::HepMCEventLocation::Default } } ) {}

  void               operator()( const HepMC3::GenEventPtrs& ) const override;
  virtual StatusCode finalize() override;

private:
  Gaudi::Property<bool> m_produceHistos{ this, "HistoProduce", false, "Switch on/off the production of histograms" };

  mutable std::atomic<int> m_counter{ 0 };
  mutable std::atomic<int> m_counterstable{ 0 };
  mutable std::atomic<int> m_counterCharged{ 0 };
  mutable std::atomic<int> m_counterChInEta{ 0 };
  mutable std::atomic<int> m_nEvents{ 0 };

  mutable Acc::StaticHistogram<1> m_hNPart{ this, "1", "Multiplicity all particles", { 300, 0., 2999. } };
  mutable Acc::StaticHistogram<1> m_hNStable{ this, "2", "Multiplicity protostable particles", { 300, 0., 2999. } };
  mutable Acc::StaticHistogram<1> m_hNSCharg{
      this, "3", "Multiplicity stable charged particles", { 300, -0.5, 299.5 } };
  mutable Acc::StaticHistogram<1> m_hNSChEta{
      this, "4", "Multiplicity stable charged particles in LHCb eta", { 300, -0.5, 299.5 } };
  mutable Acc::StaticHistogram<1> m_hProcess{ this, "5", "Process type", { 5101, -0.5, 5100.5 } };
  mutable Acc::StaticHistogram<1> m_hNPileUp{
      this, "10", "Num. of primary interaction per bunch", { 101, -0.5, 100.5 } };
  mutable Acc::StaticHistogram<1> m_hPrimX{ this, "11", "PrimaryVertex x (mm)", { 200, -1.0, 1.0 } };
  mutable Acc::StaticHistogram<1> m_hPrimY{ this, "12", "PrimaryVertex y (mm)", { 200, -1.0, 1.0 } };
  mutable Acc::StaticHistogram<1> m_hPrimZ{ this, "13", "PrimaryVertex z (mm)", { 100, -200., 200. } };
  mutable Acc::StaticHistogram<1> m_hPrimZV{ this, "14", "PrimaryVertex z, all Velo (mm)", { 100, -1000., 1000. } };
  mutable Acc::StaticHistogram<1> m_hPrimZE{
      this, "15", "PrimaryVertex z, all Exp Area (mm)", { 105, -1000., 20000. } };
  mutable Acc::StaticHistogram<1> m_hPrimT{ this, "16", "PrimaryVertex t (ns)", { 151, -75.5, 75.5 } };
  mutable Acc::StaticHistogram<1> m_hPartP{ this, "21", "Momentum of all particles (GeV)", { 100, 0., 100. } };
  mutable Acc::StaticHistogram<1> m_hPartPDG{ this, "22", "PDGid of all particles", { 10000, -4999., 5000. } };
  mutable Acc::StaticHistogram<1> m_hProtoP{ this, "31", "Momentum of protostable particles (GeV)", { 100, 0., 100. } };
  mutable Acc::StaticHistogram<1> m_hProtoPDG{ this, "32", "PDGid of protostable particles", { 10000, -4999., 5000. } };
  mutable Acc::StaticHistogram<1> m_hProtoLTime{ this, "33", "Lifetime protostable particles (mm)", { 105, -1., 20. } };
  mutable Acc::StaticHistogram<1> m_hStableEta{
      this, "44", "Pseudorapidity stable charged particles", { 150, -15., 15. } };
  mutable Acc::StaticHistogram<1> m_hStablePt{ this, "45", "Pt stable charged particles", { 100, 0., 20. } };
  mutable Acc::StaticHistogram<2> m_hPrimXvsZ{
      this, "17", "PrimaryVertex x vs z (mm)", { 100, -500., 500. }, { 100, -1., 1. } };
  mutable Acc::StaticHistogram<2> m_hPrimYvsZ{
      this, "18", "PrimaryVertex y vs z (mm)", { 100, -500., 500. }, { 100, -1., 1. } };

  Gaudi::Property<double>      m_minEta{ this, "MinEta", 2.0 };
  Gaudi::Property<double>      m_maxEta{ this, "MaxEta", 4.9 };
  Gaudi::Property<std::string> m_generatorName{ this, "ApplyTo", "" };
};

// Declaration of the Algorithm Factory
DECLARE_COMPONENT( GenMonitorAlg )

void GenMonitorAlg::operator()( const HepMC3::GenEventPtrs& hepmcevents ) const {
  // Initialize counters
  int nParticles( 0 ), nParticlesStable( 0 );
  int nParticlesStableCharged( 0 ), nParChStabEtaAcc( 0 );
  int nPileUp( 0 );

  for ( auto& hepmcevent : hepmcevents ) {
    auto gen_name =
        hepmcevent->attribute<HepMC3::StringAttribute>( Gaussino::HepMC::Attributes::GeneratorName )->value();

    // Check if monitor has to be applied to this event
    if ( !m_generatorName.empty() ) {
      if ( m_generatorName != gen_name ) { continue; }
    }
    debug() << "Monitor for " << gen_name << endmsg;

    // Get the signal process ID from the attributes
    if ( m_produceHistos.value() ) {
      auto sig_proc_id_attr =
          hepmcevent->attribute<HepMC3::IntAttribute>( Gaussino::HepMC::Attributes::SignalProcessID );
      if ( sig_proc_id_attr ) {
        auto sig_proc_id = sig_proc_id_attr->value();
        ++m_hProcess[sig_proc_id];
      }
    }

    bool primFound = false;
    nPileUp++;
    for ( auto& hepMCpart : hepmcevent->particles() ) {
      nParticles++;
      if ( m_produceHistos.value() ) {
        // Identify primary vertex and fill histograms
        if ( !primFound ) {
          if ( ( hepMCpart->status() == 1 ) || ( hepMCpart->status() == 888 ) ) {
            primFound = true;
            if ( hepMCpart->production_vertex() ) {
              ++m_hPrimX[hepMCpart->production_vertex()->position().x() / Gaudi::Units::mm];
              ++m_hPrimY[hepMCpart->production_vertex()->position().y() / Gaudi::Units::mm];
              ++m_hPrimZ[hepMCpart->production_vertex()->position().z() / Gaudi::Units::mm];
              ++m_hPrimZV[hepMCpart->production_vertex()->position().z() / Gaudi::Units::mm];
              ++m_hPrimZE[hepMCpart->production_vertex()->position().z() / Gaudi::Units::mm];
              ++m_hPrimT[hepMCpart->production_vertex()->position().t() / Gaudi::Units::ns];
              ++m_hPrimXvsZ[{ hepMCpart->production_vertex()->position().z() / Gaudi::Units::mm,
                              hepMCpart->production_vertex()->position().x() / Gaudi::Units::mm }];
              ++m_hPrimYvsZ[{ hepMCpart->production_vertex()->position().z() / Gaudi::Units::mm,
                              hepMCpart->production_vertex()->position().y() / Gaudi::Units::mm }];
            }
          }
        }

        ++m_hPartP[hepMCpart->momentum().p3mod() / Gaudi::Units::GeV];
        ++m_hPartPDG[hepMCpart->pdg_id()];
      }
      // Note that the following is really multiplicity of particle defined
      // as stable by Pythia just after hadronization: all particles known by
      // EvtGen are defined stable for Pythia (it will count rho and pi0
      // and gamma all togheter as stable ...
      if ( ( hepMCpart->status() != 2 ) && ( hepMCpart->status() != 3 ) ) {
        nParticlesStable++;
        if ( m_produceHistos.value() ) {
          ++m_hProtoP[hepMCpart->momentum().p3mod() / Gaudi::Units::GeV];
          ++m_hProtoPDG[hepMCpart->pdg_id()];
          ++m_hProtoLTime[GaussGenUtil::lifetime( hepMCpart ) / Gaudi::Units::mm];
        }
        // Charged stable particles meaning really stable after EvtGen
        LHCb::ParticleID pID( hepMCpart->pdg_id() );
        if ( 0.0 != pID.threeCharge() ) {
          // A stable particle does not have an outgoing vertex
          if ( !hepMCpart->end_vertex() ) {
            // should be the same as the following
            //             if ( ( hepMCpart -> status() == 999 )
            ++nParticlesStableCharged;
            double pseudoRap = hepMCpart->momentum().pseudoRapidity();
            // in LHCb acceptance
            if ( ( pseudoRap > m_minEta ) && ( pseudoRap < m_maxEta ) ) { ++nParChStabEtaAcc; }
            if ( m_produceHistos.value() ) {
              ++m_hStableEta[pseudoRap];
              ++m_hStablePt[hepMCpart->momentum().perp() / Gaudi::Units::GeV];
            }
          }
        }
      }
    }
  }

  if ( m_produceHistos.value() ) {
    ++m_hNPart[nParticles];
    ++m_hNStable[nParticlesStable];
    ++m_hNSCharg[nParticlesStableCharged];
    ++m_hNSChEta[nParChStabEtaAcc];
    ++m_hNPileUp[nPileUp];
  }
  m_counter += nParticles;
  m_counterstable += nParticlesStable;
  m_counterCharged += nParticlesStableCharged;
  m_counterChInEta += nParChStabEtaAcc;
  m_nEvents++;
}

StatusCode GenMonitorAlg::finalize() {
  info() << std::endl
         << "======================== Generators Statistics ====================" << std::endl
         << "=                                                                 =" << std::endl
         << "= Number of particles generated: " << m_counter << std::endl
         << "= Number of events: " << m_nEvents << std::endl
         << "= Mean multiplicity: " << m_counter / (double)m_nEvents << std::endl
         << "=                                                                 =" << std::endl
         << "= Number of pseudo stable particles generated: " << m_counterstable << std::endl
         << "= Number of events: " << m_nEvents << std::endl
         << "= Mean pseudo stable multiplicity: " << m_counterstable / (double)m_nEvents << std::endl
         << "=                                                                 =" << std::endl
         << "= Number of charged stable particles generated: " << m_counterCharged << std::endl
         << "= Number of events: " << m_nEvents << std::endl
         << "= Mean charged stable multiplicity: " << m_counterCharged / (double)m_nEvents << std::endl
         << "=                                                                 =" << std::endl
         << "= Number of charged stable particles in LHCb eta " << m_counterChInEta << std::endl
         << "= Number of events: " << m_nEvents << std::endl
         << "= Mean charged stable multiplicity in LHCb eta: " << m_counterChInEta / (double)m_nEvents << std::endl
         << "=                                                                 =" << std::endl
         << "===================================================================" << endmsg;
  return Consumer::finalize();
}
