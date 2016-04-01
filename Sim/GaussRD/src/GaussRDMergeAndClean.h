#ifndef GaussRDMergeAndClean_H
#define GaussRDMergeAndClean_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <utility>

class IGaussRDStr;

/** @class GaussRDMergeAndClean GaussRDMergeAndClean.h
 *
 * Algorithm to merge all the signal stuff back into the main containers
 * and clean the signal tree in the TES.
 *
 *  @author Dominik Muller
 *  @date   2016-4-1
 */
class GaussRDMergeAndClean : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRDMergeAndClean(const std::string& Name, ISvcLocator* SvcLoc);

  virtual ~GaussRDMergeAndClean();  ///< Destructor

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  protected:
  /** accessor to GaussRD Service
   *  @return pointer to GaussRD Service
   */
  private:
  std::string m_gaussRDSvcName;
  IGaussRDStr* m_gaussRDStrSvc;

  std::string m_particlesLocation;
  std::string m_verticesLocation;
  std::vector<std::string> m_hitsLocations;
  std::vector<std::string> m_calohitsLocations;
  std::string m_richHitsLocation;
  std::string m_richOpticalPhotonsLocation;
  std::string m_richSegmentsLocation;
  std::string m_richTracksLocation;
  std::string m_mcHeaderLocation;
  std::string m_signal_tes_prefix;
  std::string m_hepMCEventLocation;
  std::string m_genCollisionLocation;
  template <typename T>
  std::pair<T*, T*> get_and_print(const std::string&);
};

template <typename T>
std::pair<T*, T*> GaussRDMergeAndClean::get_and_print(const std::string& loc) {
  auto con = get<T>(loc);
  auto loc_s = m_signal_tes_prefix+loc;
  auto con_s = get<T>(loc_s);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Got " << con->size() << " from " << loc << endmsg;
    debug() << "Got " << con_s->size() << " from " << loc_s << endmsg;
  }
  return std::pair<T*,T*>(con, con_s);
}
#endif  // GaussRDMergeAndClean_H
