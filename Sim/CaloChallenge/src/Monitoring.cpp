/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

#include "CaloHit.h"
#include "CollectorHit.h"

#include <Gaudi/Accumulators/Histogram.h>
#include <Gaudi/FSMCallbackHolder.h>
#include <GaudiAlg/Consumer.h>
#include <GaudiAlg/FixTESPath.h>
#include <GaudiKernel/SystemOfUnits.h>

namespace Acc = Gaudi::Accumulators;

namespace Gsino::CaloChallenge {
  using BaseClass_t = Gaudi::Functional::Traits::BaseClass_t<Gaudi::FSMCallbackHolder<FixTESPath<Gaudi::Algorithm>>>;

  template <class... TCaloHits>
  using MonitoringBase = Gaudi::Functional::Consumer<void( const EDM::Hits&, const TCaloHits&... ), BaseClass_t>;

  /**
   * @brief Monitoring algorithm for calorimeter hits in the Gaussino CaloChallenge.
   *
   * This algorithm monitors calorimeter hits and collector hits, providing histograms
   * and counters for various properties of the hits.
   *
   * @tparam TCaloHits Types of calorimeter hits to monitor.
   * @author Michał Mazurek
   * @date 2023
   */
  template <class... TCaloHits>
  class Monitoring : public MonitoringBase<TCaloHits...> {
    mutable std::mutex m_lock;

    // detector
    Gaudi::Property<double> m_cellSizeZ{ this, "CellSizeZ", -1 };
    Gaudi::Property<double> m_cellSizeRho{ this, "CellSizeRho", -1 };
    Gaudi::Property<double> m_cellSizePhi{ this, "CellSizePhi", -1 };
    Gaudi::Property<int>    m_cellNumZ{ this, "CellNumZ", -1 };
    Gaudi::Property<int>    m_cellNumRho{ this, "CellNumRho", -1 };
    Gaudi::Property<int>    m_cellNumPhi{ this, "CellNumPhi", -1 };

    // histogram
    Gaudi::Property<double> m_maxEnergyHist{ this, "MaxEnergyHist", 1. * Gaudi::Units::GeV };

    mutable Acc::Histogram<1> h_energyParticle{ this, "energyParticle", "Primary energy;E_{MC} (GeV);Entries" };
    mutable Acc::Histogram<1> h_energyDeposited{ this, "energyDeposited", "Deposited energy;E_{MC} (GeV);Entries" };
    mutable Acc::StaticHistogram<1> h_energyRatio{
        this, "energyRatio", "Ratio of energy deposited to primary;E_{dep} /  E_{MC};Entries", { 1024, 0, 1 } };
    mutable Acc::ProfileHistogram<1> h_longProfile{ this, "longProfile", "Longitudinal profile;t (mm);#LTE#GT (MeV)" };
    mutable Acc::ProfileHistogram<1> h_transProfile{ this, "transProfile", "Transverse profile;r (mm);#LTE#GT (MeV)" };
    mutable Acc::Histogram<1>        h_longFirstMoment{
        this, "longFirstMoment", "First moment of longitudinal distribution;#LT#lambda#GT (mm);Entries" };
    mutable Acc::Histogram<1> h_longSecondMoment{
        this, "longSecondMoment", "Second moment of longitudinal distribution;#LT#lambda^{2}#GT (mm^{2});Entries" };
    mutable Acc::Histogram<1> h_transFirstMoment{ this, "transFirstMoment",
                                                  "First moment of transverse distribution;#LTr#GT (mm);Entries" };
    mutable Acc::Histogram<1> h_transSecondMoment{
        this, "transSecondMoment", "Second moment of transverse distribution;#LTr^{2}#GT (mm^{2});Entries" };
    mutable Acc::StaticHistogram<1> h_hitType{
        this, "hitType", "hit type;type (0=full, 1= fast);Entries", { 2, -.5, 1.5 } };
    mutable Acc::ProfileHistogram<1> h_phiProfile{ this, "phiProfile",
                                                   "Azimuthal angle profile, centred at mean;phi;#LTE#GT (MeV)" };
    mutable Acc::StaticHistogram<1>  h_numHits{ this, "numHits", "Number of hits above 0.5 keV", { 4048, 0, 40500 } };
    mutable Acc::StaticHistogram<1>  h_cellEnergy{
        this, "cellEnergy", "Cell energy distribution;log10(E/MeV);Entries", { 1024, -4, 2 } };

    mutable Acc::StatCounter<> m_calohits{ this, "#CaloHits" };
    mutable Acc::StatCounter<> m_collhits{ this, "#CollectorHits" };
    mutable Acc::StatCounter<> m_fullMatched{ this, "#FullSimCollectorHits" };
    mutable Acc::StatCounter<> m_fastMatched{ this, "#FastSimCollectorHits" };
    mutable Acc::StatCounter<> m_numNonZeroThresholdCells{ this, "NonZeroThresholdCells (>.5 keV)" };
    mutable Acc::StatCounter<> m_fastHits{ this, "#FastSimHits" };
    mutable Acc::StatCounter<> m_fullHits{ this, "#FullSimHits" };
    mutable Acc::StatCounter<> m_totalEnergy{ this, "Energy Deposit [MeV]" };
    mutable Acc::StatCounter<> m_particleEnergy{ this, "Particle Energy [MeV]" };

    template <size_t... I>
    Monitoring( const std::string& name, ISvcLocator* pSvcLocator, std::integer_sequence<size_t, I...> )
        : MonitoringBase<TCaloHits...>(
              name, pSvcLocator,
              { { "CollectorHitsLocation", "" }, { "CaloHitsLocation" + std::to_string( I ), "" }... } ) {}

  public:
    Monitoring( const std::string& name, ISvcLocator* pSvcLocator )
        : Monitoring( name, pSvcLocator, std::make_integer_sequence<size_t, sizeof...( TCaloHits )>{} ) {}

    StatusCode initialize() override {
      return MonitoringBase<TCaloHits...>::initialize().andThen( [&]() -> StatusCode {
        if ( m_cellSizeZ < 0 ) {
          this->error() << "Property 'CellSizeZ' must be set!" << endmsg;
          return StatusCode::FAILURE;
        }

        if ( m_cellSizeRho < 0 ) {
          this->error() << "Property 'CellSizeRho' must be set!" << endmsg;
          return StatusCode::FAILURE;
        }

        if ( m_cellSizePhi < 0 ) {
          this->error() << "Property 'CellSizePhi' must be set!" << endmsg;
          return StatusCode::FAILURE;
        }

        if ( m_cellNumZ < 0 ) {
          this->error() << "Property 'CellNumZ' must be set!" << endmsg;
          return StatusCode::FAILURE;
        }

        if ( m_cellNumRho < 0 ) {
          this->error() << "Property 'CellNumRho' must be set!" << endmsg;
          return StatusCode::FAILURE;
        }

        if ( m_cellNumPhi < 0 ) {
          this->error() << "Property 'CellNumPhi' must be set!" << endmsg;
          return StatusCode::FAILURE;
        }

        h_energyParticle.setAxis<0>( { 1024, 0, 1.1 * m_maxEnergyHist / Gaudi::Units::GeV } );
        h_energyDeposited.setAxis<0>( { 1024, 1.1 * m_maxEnergyHist / Gaudi::Units::GeV } );
        h_longProfile.setAxis<0>( { (unsigned int)m_cellNumZ, -.5 * m_cellSizeZ, ( m_cellNumZ - .5 ) * m_cellSizeZ } );
        h_transProfile.setAxis<0>(
            { (unsigned int)m_cellNumRho, -.5 * m_cellSizeRho, ( m_cellNumRho - .5 ) * m_cellSizeRho } );
        h_longFirstMoment.setAxis<0>( { 1024, -.5 * m_cellSizeZ, m_cellNumZ * m_cellSizeZ / 2. } );
        h_transFirstMoment.setAxis<0>( { 1024, -.5 * m_cellSizeRho, m_cellNumRho * m_cellSizeRho } );
        h_longSecondMoment.setAxis<0>( { 1024, 0, std::pow( m_cellNumZ * m_cellSizeZ, 2 ) / 25 } );
        h_transSecondMoment.setAxis<0>( { 1024, 0, std::pow( m_cellNumRho * m_cellSizeRho, 2 ) / 5 } );
        h_phiProfile.setAxis<0>( { (unsigned int)m_cellNumPhi, -( m_cellNumPhi - .5 ) * m_cellSizePhi,
                                   ( m_cellNumPhi - 0.5 ) * m_cellSizePhi } );
        return StatusCode::SUCCESS;
      } );
    }

    void operator()( const EDM::Hits& collhits, const TCaloHits&... caloHitsContainers ) const override {

      std::lock_guard<std::mutex> guard_lock( m_lock );

      int    numNonZeroThresholdCells = 0;
      int    fastHits                 = 0;
      int    fullHits                 = 0;
      double totalEnergy              = 0.;
      double tFirstMoment             = 0.;
      double tSecondMoment            = 0.;
      double rFirstMoment             = 0.;
      double rSecondMoment            = 0.;
      double phiMean                  = 0.;
      double primaryEnergy            = 0.;

      std::map<int, bool> fullMatched = {};
      std::map<int, bool> fastMatched = {};

      // collector hits
      m_collhits += collhits.size();
      for ( const auto& thit : collhits ) {
        auto hit = std::dynamic_pointer_cast<CollectorHit>( thit );
        primaryEnergy += hit->GetPrimaryEnergy();
        fullMatched[hit->GetTrackID()] = false;
        fastMatched[hit->GetTrackID()] = false;
      }
      ++h_energyParticle[primaryEnergy / Gaudi::Units::GeV];
      m_particleEnergy += primaryEnergy;

      // calo hits
      size_t caloHitsNo = 0;
      for ( const auto& caloHits : { caloHitsContainers... } ) {
        caloHitsNo += caloHits.size();
        for ( const auto& thit : caloHits ) {
          auto hit   = std::dynamic_pointer_cast<CaloHit>( thit );
          auto hitEn = hit->GetEdep();
          totalEnergy += hitEn;
          auto tDistance   = hit->GetZId() * m_cellSizeZ.value();
          auto rDistance   = hit->GetRhoId() * m_cellSizeRho.value();
          auto phiDistance = hit->GetPhiId() * m_cellSizePhi.value();
          tFirstMoment += hitEn * tDistance;
          rFirstMoment += hitEn * rDistance;
          phiMean += hitEn * phiDistance;
          h_longProfile[tDistance] += hitEn;
          h_transProfile[rDistance] += hitEn;
          auto hitType = hit->GetType();
          ++h_hitType[hitType];
          if ( hitType == 1 ) {
            fastHits += 1;
            fastMatched[hit->GetTrackID()] = true;
          } else if ( hitType == 0 ) {
            fullHits += 1;
            fullMatched[hit->GetTrackID()] = true;
          }
          if ( hitEn > 5e-4 * Gaudi::Units::MeV ) { // e > 0.5 keV
            numNonZeroThresholdCells++;
            ++h_cellEnergy[std::log10( hitEn )];
          }
        }
      }
      m_fullMatched +=
          std::accumulate( fullMatched.begin(), fullMatched.end(), 0.0,
                           []( double sum, const std::pair<int, bool>& pair ) { return sum + pair.second; } );
      m_fastMatched +=
          std::accumulate( fastMatched.begin(), fastMatched.end(), 0.0,
                           []( double sum, const std::pair<int, bool>& pair ) { return sum + pair.second; } );
      m_calohits += caloHitsNo;
      m_totalEnergy += totalEnergy;
      m_numNonZeroThresholdCells += numNonZeroThresholdCells;
      m_fullHits += fullHits;
      m_fastHits += fastHits;

      tFirstMoment /= totalEnergy;
      rFirstMoment /= totalEnergy;
      phiMean /= totalEnergy;
      ++h_energyDeposited[totalEnergy / Gaudi::Units::GeV];
      ++h_energyRatio[totalEnergy / primaryEnergy];
      ++h_longFirstMoment[tFirstMoment];
      ++h_transFirstMoment[rFirstMoment];
      ++h_numHits[numNonZeroThresholdCells];
      for ( const auto& caloHits : { caloHitsContainers... } ) {
        for ( const auto& thit : caloHits ) {
          auto hit   = std::dynamic_pointer_cast<CaloHit>( thit );
          auto hitEn = hit->GetEdep();
          if ( hitEn > 0 ) {
            auto tDistance   = hit->GetZId() * m_cellSizeZ;
            auto rDistance   = hit->GetRhoId() * m_cellSizeRho;
            auto phiDistance = hit->GetPhiId() * m_cellSizePhi;
            tSecondMoment += hitEn * std::pow( tDistance - tFirstMoment, 2 );
            rSecondMoment += hitEn * std::pow( rDistance - rFirstMoment, 2 );
            h_phiProfile[phiDistance - phiMean] += hitEn;
          }
        }
      }
      tSecondMoment /= totalEnergy;
      rSecondMoment /= totalEnergy;
      ++h_longSecondMoment[tSecondMoment];
      ++h_transSecondMoment[rSecondMoment];
    };
  };
  using DetailedSimMonitoring        = Monitoring<EDM::CaloHits>;
  using DetailedAndFastSimMonitoring = Monitoring<EDM::CaloHits, EDM::CaloHits>;
} // namespace Gsino::CaloChallenge

DECLARE_COMPONENT_WITH_ID( Gsino::CaloChallenge::DetailedSimMonitoring, "CaloChallengeDetailedSimMonitoring" )
DECLARE_COMPONENT_WITH_ID( Gsino::CaloChallenge::DetailedAndFastSimMonitoring,
                           "CaloChallengeDetailedAndFastSimMonitoring" )
