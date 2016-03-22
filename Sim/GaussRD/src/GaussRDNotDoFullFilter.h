// $Id: GaussRDDFullFilter.h,v 1.4 2007-01-12 15:23:41 ranjard Exp $
#ifndef GaussRDNotDoFullFilter_H
#define GaussRDNotDoFullFilter_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGaussRDCtr;  ///< GaussRD counter service

/** @class GaussRDNotDoFullFilter GaussRDNotDoFullFilter.h
 *
 *  An algorithm to control processing in a GaudiSequencer.
 *  Following members of the sequence are only processed
 *  if a new event needs to be generated
 *
 *  @author Dominik Muller
 *  @date   2016-3-15
 */
class GaussRDNotDoFullFilter : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRDNotDoFullFilter(const std::string& Name, ISvcLocator* SvcLoc);

  virtual ~GaussRDNotDoFullFilter();  ///< Destructor

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
};

#endif  // GaussRDNotDoFullFilter_H
