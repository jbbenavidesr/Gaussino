#ifndef GaussRDRetrieveFromService_H
#define GaussRDRetrieveFromService_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGaussRDStr;  ///< GaussRD counter service
class IGaussRDCtr;  ///< GaussRD counter service

/** @class GaussRDRetrieveFromService GaussRDRetrieveFromService.h
 *
 *  An algorithm to control processing in a GaudiSequencer.
 *  Following members of the sequence are only processed
 *  if a new event needs to be generated
 *
 *  @author Dominik Muller
 *  @date   2016-3-21
 */
class GaussRDRetrieveFromService : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRDRetrieveFromService(const std::string& Name, ISvcLocator* SvcLoc);

  virtual ~GaussRDRetrieveFromService();  ///< Destructor

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  protected:
  /** accessor to GaussRD Service
   *  @return pointer to GaussRD Service
   */
  inline IGaussRDStr* gaussRDStrSvc() const { return m_gaussRDStrSvc; }
  inline IGaussRDCtr* gaussRDCtrSvc() const { return m_gaussRDCtrSvc; }

  private:
  std::string m_gaussRDSvcName;
  IGaussRDStr* m_gaussRDStrSvc;
  IGaussRDCtr* m_gaussRDCtrSvc;

  std::string m_particlesLocation;  ///< Location in TES of output MCParticles.
  std::string m_verticesLocation;   ///< Location in TES of output MCVertices.
  std::vector<std::string> m_hitsLocations;   ///< Location in TES of output MCHits.
  std::vector<std::string> m_calohitsLocations;   ///< Location in TES of output MCCaloHits.
  std::string m_richHitsLocation;
  std::string m_richOpticalPhotonsLocation;
  std::string m_richSegmentsLocation;
  std::string m_richTracksLocation;
  std::string m_GenCollisionsLocation;

};

#endif  // GaussRDRetrieveFromService_H
