#ifndef GaussRedecayCtrFilter_H
#define GaussRedecayCtrFilter_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGaussRedecayCtr;  ///< GaussRedecay counter service

/** @class GaussRedecayCtrFilter GaussRedecayCtrFilter.h
 *
 *  An algorithm to control processing in a GaudiSequencer for GaussRedecay.
 *
 *  @author Dominik Muller
 *  @date   2016-3-15
 */
class GaussRedecayCtrFilter : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRedecayCtrFilter(const std::string& Name, ISvcLocator* SvcLoc);

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  protected:
  /** accessor to GaussRedecay Service
   *  @return pointer to GaussRedecay Service
   */
  inline IGaussRedecayCtr* gaussRDSvc() const { return m_gaussRDSvc; }

  private:
  std::string m_gaussRDSvcName;
  IGaussRedecayCtr* m_gaussRDSvc;

  bool m_registerNewEvent;
  int m_isPhase;
  int m_isPhaseNot;
  int m_setPhase;
  std::string m_check_for = "";
  bool m_checked = false;
  std::string m_genHeader ; ///< Location where to store Gen Header
};

#endif  // GaussRedecayCtrFilter_H
