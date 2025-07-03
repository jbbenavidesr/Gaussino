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

// Gaussino
#include "CaloHit.h"
#include "CollectorHit.h"
#include "GaudiAlg/Consumer.h"

// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/SystemOfUnits.h"

// AIDA
#include "AIDA/IHistogram1D.h"

namespace Gsino::CaloChallenge {
  using BaseClass_t = Gaudi::Functional::Traits::BaseClass_t<GaudiHistoAlg>;

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

    // histograms
    AIDA::IHistogram1D* h_energyParticle    = nullptr;
    AIDA::IHistogram1D* h_energyDeposited   = nullptr;
    AIDA::IHistogram1D* h_energyRatio       = nullptr;
    AIDA::IHistogram1D* h_time              = nullptr;
    AIDA::IHistogram1D* h_longProfile       = nullptr;
    AIDA::IHistogram1D* h_transProfile      = nullptr;
    AIDA::IHistogram1D* h_longFirstMoment   = nullptr;
    AIDA::IHistogram1D* h_longSecondMoment  = nullptr;
    AIDA::IHistogram1D* h_transFirstMoment  = nullptr;
    AIDA::IHistogram1D* h_transSecondMoment = nullptr;
    AIDA::IHistogram1D* h_hitType           = nullptr;
    AIDA::IHistogram1D* h_phiProfile        = nullptr;
    AIDA::IHistogram1D* h_numHits           = nullptr;
    AIDA::IHistogram1D* h_cellEnergy        = nullptr;

    // histogram properties
    Gaudi::Property<double> m_maxEnergyHist{ this, "MaxEnergyHist", 1. * Gaudi::Units::GeV };

    // counters
    mutable Gaudi::Accumulators::StatCounter<> m_calohits{ this, "#CaloHits" };
    mutable Gaudi::Accumulators::StatCounter<> m_collhits{ this, "#CollectorHits" };
    mutable Gaudi::Accumulators::StatCounter<> m_fullMatched{ this, "#FullSimCollectorHits" };
    mutable Gaudi::Accumulators::StatCounter<> m_fastMatched{ this, "#FastSimCollectorHits" };
    mutable Gaudi::Accumulators::StatCounter<> m_numNonZeroThresholdCells{ this, "NonZeroThresholdCells (>.5 keV)" };
    mutable Gaudi::Accumulators::StatCounter<> m_fastHits{ this, "#FastSimHits" };
    mutable Gaudi::Accumulators::StatCounter<> m_fullHits{ this, "#FullSimHits" };
    mutable Gaudi::Accumulators::StatCounter<> m_totalEnergy{ this, "Energy Deposit [MeV]" };
    mutable Gaudi::Accumulators::StatCounter<> m_particleEnergy{ this, "Particle Energy [MeV]" };

  public:
    using KeyValue = class MonitoringBase<TCaloHits...>::KeyValue;

    constexpr static std::size_t caloHitsContainersNo = sizeof...( TCaloHits );
    Monitoring( const std::string& name, ISvcLocator* pSvcLocator )
        : MonitoringBase<TCaloHits...>(
              name, pSvcLocator,
              std::tuple_cat( std::make_tuple( KeyValue{ "CollectorHitsLocation", "" } ), std::invoke( []() {
                                std::array<KeyValue, caloHitsContainersNo> caloHitsLocs{};
                                for ( size_t i = 0; i < caloHitsContainersNo; i++ ) {
                                  caloHitsLocs[i] = KeyValue{ "CaloHitsLocation" + std::to_string( i ), "" };
                                }
                                return std::tuple_cat( caloHitsLocs );
                              } ) ) ){};

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

        h_energyParticle  = this->book( "energyParticle", "Primary energy;E_{MC} (GeV);Entries", 0,
                                        1.1 * m_maxEnergyHist / Gaudi::Units::GeV, 1024 );
        h_energyDeposited = this->book( "energyDeposited", "Deposited energy;E_{MC} (GeV);Entries", 0,
                                        1.1 * m_maxEnergyHist / Gaudi::Units::GeV, 1024 );
        h_energyRatio =
            this->book( "energyRatio", "Ratio of energy deposited to primary;E_{dep} /  E_{MC};Entries", 0, 1, 1024 );
        // h_time         = this->book( "time", "Simulation time; time (s);Entries", 0, 100, 1024 );
        h_longProfile  = this->book( "longProfile", "Longitudinal profile;t (mm);#LTE#GT (MeV)", -.5 * m_cellSizeZ,
                                     ( m_cellNumZ - .5 ) * m_cellSizeZ, m_cellNumZ );
        h_transProfile = this->book( "transProfile", "Transverse profile;r (mm);#LTE#GT (MeV)", -.5 * m_cellSizeRho,
                                     ( m_cellNumRho - .5 ) * m_cellSizeRho, m_cellNumRho );
        h_longFirstMoment =
            this->book( "longFirstMoment", "First moment of longitudinal distribution;#LT#lambda#GT (mm);Entries",
                        -.5 * m_cellSizeZ, m_cellNumZ * m_cellSizeZ / 2., 1024 );
        h_transFirstMoment =
            this->book( "transFirstMoment", "First moment of transverse distribution;#LTr#GT (mm);Entries",
                        -.5 * m_cellSizeRho, m_cellNumRho * m_cellSizeRho, 1024 );
        h_longSecondMoment = this->book(
            "longSecondMoment", "Second moment of longitudinal distribution;#LT#lambda^{2}#GT (mm^{2});Entries", 0,
            std::pow( m_cellNumZ * m_cellSizeZ, 2 ) / 25, 1024 );
        h_transSecondMoment =
            this->book( "transSecondMoment", "Second moment of transverse distribution;#LTr^{2}#GT (mm^{2});Entries", 0,
                        std::pow( m_cellNumRho * m_cellSizeRho, 2 ) / 5, 1024 );
        h_hitType = this->book( "hitType", "hit type;type (0=full, 1= fast);Entries", -.5, 1.5, 2 );
        h_phiProfile =
            this->book( "phiProfile", "Azimuthal angle profile, centred at mean;phi;#LTE#GT (MeV)",
                        -( m_cellNumPhi - .5 ) * m_cellSizePhi, ( m_cellNumPhi - 0.5 ) * m_cellSizePhi, m_cellNumPhi );
        h_numHits    = this->book( "numHits", "Number of hits above 0.5 keV", 0, 40500, 4048 );
        h_cellEnergy = this->book( "cellEnergy", "Cell energy distribution;log10(E/MeV);Entries", -4, 2, 1024 );
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
      h_energyParticle->fill( primaryEnergy / Gaudi::Units::GeV );
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
          h_longProfile->fill( tDistance, hitEn );
          h_transProfile->fill( rDistance, hitEn );
          auto hitType = hit->GetType();
          h_hitType->fill( hitType );
          if ( hitType == 1 ) {
            fastHits += 1;
            fastMatched[hit->GetTrackID()] = true;
          } else if ( hitType == 0 ) {
            fullHits += 1;
            fullMatched[hit->GetTrackID()] = true;
          }
          if ( hitEn > 5e-4 * Gaudi::Units::MeV ) { // e > 0.5 keV
            numNonZeroThresholdCells++;
            h_cellEnergy->fill( std::log10( hitEn ) );
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
      h_energyDeposited->fill( totalEnergy / Gaudi::Units::GeV );
      h_energyRatio->fill( totalEnergy / primaryEnergy );
      // h_time->fill(fTimer.GetRealElapsed());
      h_longFirstMoment->fill( tFirstMoment );
      h_transFirstMoment->fill( rFirstMoment );
      h_numHits->fill( numNonZeroThresholdCells );
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
            h_phiProfile->fill( phiDistance - phiMean, hitEn );
          }
        }
      }
      tSecondMoment /= totalEnergy;
      rSecondMoment /= totalEnergy;
      h_longSecondMoment->fill( tSecondMoment );
      h_transSecondMoment->fill( rSecondMoment );
    };
  };
  using DetailedSimMonitoring        = Monitoring<EDM::CaloHits>;
  using DetailedAndFastSimMonitoring = Monitoring<EDM::CaloHits, EDM::CaloHits>;
} // namespace Gsino::CaloChallenge

DECLARE_COMPONENT_WITH_ID( Gsino::CaloChallenge::DetailedSimMonitoring, "CaloChallengeDetailedSimMonitoring" )
DECLARE_COMPONENT_WITH_ID( Gsino::CaloChallenge::DetailedAndFastSimMonitoring,
                           "CaloChallengeDetailedAndFastSimMonitoring" )
