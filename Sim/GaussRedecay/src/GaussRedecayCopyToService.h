#ifndef GaussRedecayCopyToService_H
#define GaussRedecayCopyToService_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGaussRedecayStr;  ///< GaussRedecay counter service
class IGaussRedecayCtr;  ///< GaussRedecay counter service

/** @class GaussRedecayCopyToService GaussRedecayCopyToService.h
 *
 * Algorithm to copy all the MC objects into the service for later reuse.
 *
 *  @author Dominik Muller
 *  @date   2016-3-15
 */
class GaussRedecayCopyToService : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRedecayCopyToService(const std::string& Name, ISvcLocator* SvcLoc);

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  protected:
  /** accessor to GaussRedecay Service
   *  @return pointer to GaussRedecay Service
   */

  private:
  std::string m_gaussRDSvcName;
  IGaussRedecayCtr* m_gaussRDCtrSvc;
  IGaussRedecayStr* m_gaussRDStrSvc;

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

#endif  // GaussRedecayCopyToService_H
