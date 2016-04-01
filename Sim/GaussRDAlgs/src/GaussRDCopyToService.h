// $Id: GaussRDCtrFilter.h,v 1.4 2007-01-12 15:23:41 ranjard Exp $
#ifndef GaussRDCopyToService_H
#define GaussRDCopyToService_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGaussRDStr;  ///< GaussRD counter service
class IGaussRDCtr;  ///< GaussRD counter service

/** @class GaussRDCopyToService GaussRDCopyToService.h
 *
 * Algorithm to copy all the MC objects into the service for later reuse.
 *
 *  @author Dominik Muller
 *  @date   2016-3-15
 */
class GaussRDCopyToService : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRDCopyToService(const std::string& Name, ISvcLocator* SvcLoc);

  virtual ~GaussRDCopyToService();  ///< Destructor

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  protected:
  /** accessor to GaussRD Service
   *  @return pointer to GaussRD Service
   */

  private:
  std::string m_gaussRDSvcName;
  IGaussRDCtr* m_gaussRDCtrSvc;
  IGaussRDStr* m_gaussRDStrSvc;

  std::string m_particlesLocation;  ///< Location in TES of output MCParticles.
  std::string m_verticesLocation;   ///< Location in TES of output MCVertices.
  std::vector<std::string>
      m_hitsLocations;  ///< Location in TES of output MCHits.
  std::vector<std::string>
      m_calohitsLocations;  ///< Location in TES of output MCCaloHits.
  std::string m_richHitsLocation;
  std::string m_richOpticalPhotonsLocation;
  std::string m_richSegmentsLocation;
  std::string m_richTracksLocation;
  std::string m_GenCollisionsLocation;
};

#endif  // GaussRDCopyToService_H
