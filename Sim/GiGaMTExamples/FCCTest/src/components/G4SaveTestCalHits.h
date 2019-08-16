#pragma once
// Gaudi
#include "GaudiAlg/GaudiHistoTool.h"

#include "SimInterfaces/IG4MonitoringTool.h"
#include "AIDA/IHistogram1D.h"
#include <mutex>


/** @class G4SaveTestCalHits SimG4Components/src/G4SaveTestCalHits.h G4SaveTestCalHits.h
 *
 *  Save tracker hits tool.
 *
 *  @author Anna Zaborowska
 *
 *  Modification for MT Gaussino
 *
 *  @author Dominik Muller
 */

class G4SaveTestCalHits: public extends<GaudiHistoTool, IG4MonitoringTool> {
  Gaudi::Property<std::string> m_calType{this, "caloType"};
public:
  using extends::extends;

  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() override;

  /**  Save the data output.
   *   Saves the tracker hits and clusters from every hits collection associated with the event
   *   that has m_calType in its name.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode monitor(const G4Event& aEvent) override;
  void bookHistos();
private:
  /// Pointer to histogram filled with total energy deposited in event
  AIDA::IHistogram1D* m_totHitEnergy = nullptr;
  /// Pointer to histogram filled with energy deposited in each hit
  AIDA::IHistogram1D* m_hitEnergy = nullptr;
  /// Pointer to histogram filled with x position of hits
  AIDA::IHistogram1D* m_hitX = nullptr;
  /// Pointer to histogram filled with y position of hits
  AIDA::IHistogram1D* m_hitY = nullptr;
  /// Pointer to histogram filled with z position of hits
  AIDA::IHistogram1D* m_hitZ = nullptr;
  /// Name of the calorimeter type (ECal/HCal)
  std::mutex m_hist_lock;

};
