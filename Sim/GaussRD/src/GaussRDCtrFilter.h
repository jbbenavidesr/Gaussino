// $Id: GaussRDCtrFilter.h,v 1.4 2007-01-12 15:23:41 ranjard Exp $
#ifndef GaussRDCtrFilter_H
#define GaussRDCtrFilter_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGaussRDCtr;  ///< GaussRD counter service

/** @class GaussRDCtrFilter GaussRDCtrFilter.h
 *
 *  An algorithm to control processing in a GaudiSequencer for GaussRD.
 *
 *  @author Dominik Muller
 *  @date   2016-3-15
 */
class GaussRDCtrFilter : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRDCtrFilter(const std::string& Name, ISvcLocator* SvcLoc);

  virtual ~GaussRDCtrFilter();  ///< Destructor

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  protected:
  /** accessor to GaussRD Service
   *  @return pointer to GaussRD Service
   */
  inline IGaussRDCtr* gaussRDSvc() const { return m_gaussRDSvc; }

  private:
  std::string m_gaussRDSvcName;
  IGaussRDCtr* m_gaussRDSvc;

  bool m_registerNewEvent;
  int m_isPhase;
  int m_isPhaseNot;
  int m_setPhase;
};

#endif  // GaussRDCtrFilter_H
