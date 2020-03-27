#pragma once

// Include files
// from STL
#include <atomic>
#include <mutex>
#include <string>

// from Gaudi
#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "HepMC3/GenEvent.h"
#include "HepMCUser/typedefs.h"

// from AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

/** @class GenMonitorAlg GenMonitorAlg.h Algorithms/GenMonitorAlg.h
 *
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
class GenMonitorAlg : public Gaudi::Functional::Consumer<void( const HepMC3::GenEventPtrs& ),
                                                         Gaudi::Functional::Traits::BaseClass_t<GaudiHistoAlg>>
{
public:
  /// Standard constructor
  GenMonitorAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : Consumer( name, pSvcLocator, {KeyValue{"Input", Gaussino::HepMCEventLocation::Default}} )
  {
    setProduceHistos( false );
  };

  virtual ~GenMonitorAlg() = default; ///< Destructor

  void operator()( const HepMC3::GenEventPtrs& ) const override;
  virtual StatusCode finalize() override; ///< Algorithm finalization
  virtual StatusCode initialize() override; ///< Algorithm finalization

protected:
  void bookHistos(); ///< Book histograms

private:
  mutable std::atomic<int> m_counter{0};
  mutable std::atomic<int> m_counterstable{0};
  mutable std::atomic<int> m_counterCharged{0};
  mutable std::atomic<int> m_counterChInEta{0};
  mutable std::atomic<int> m_nEvents{0};

  mutable std::mutex m_histo_lock;

  AIDA::IHistogram1D* m_hNPart;
  AIDA::IHistogram1D* m_hNStable;
  AIDA::IHistogram1D* m_hNSCharg;
  AIDA::IHistogram1D* m_hNSChEta;
  AIDA::IHistogram1D* m_hProcess;
  AIDA::IHistogram1D* m_hNPileUp;
  AIDA::IHistogram1D* m_hPrimX;
  AIDA::IHistogram1D* m_hPrimY;
  AIDA::IHistogram1D* m_hPrimZ;
  AIDA::IHistogram1D* m_hPrimZV;
  AIDA::IHistogram1D* m_hPrimZE;
  AIDA::IHistogram1D* m_hPrimT;
  AIDA::IHistogram1D* m_hPartP;
  AIDA::IHistogram1D* m_hPartPDG;
  AIDA::IHistogram1D* m_hProtoP;
  AIDA::IHistogram1D* m_hProtoPDG;
  AIDA::IHistogram1D* m_hProtoLTime;
  AIDA::IHistogram1D* m_hStableEta;
  AIDA::IHistogram1D* m_hStablePt;
  AIDA::IHistogram2D* m_hPrimXvsZ;
  AIDA::IHistogram2D* m_hPrimYvsZ;

  Gaudi::Property<double> m_minEta{this, "MinEta", 2.0};
  Gaudi::Property<double> m_maxEta{this, "MaxEta", 4.9};
  Gaudi::Property<std::string> m_generatorName{this, "ApplyTo", ""};
};
