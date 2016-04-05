#ifndef GaussRDMergeAndClean_H
#define GaussRDMergeAndClean_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <utility>
#include "Event/MCVertex.h"

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
  /*
   *Function to find the correct signal vertex in the list of mc vertices.
   *Multiple options are tried, returning if they are successful in the
   *following order:
   * 1. Find the placeholder 424242 particle
   * 2. Find the only matching vertex based on position
   * 3. Return the first matching vertex based on position
   * 4. Recreate a new vertex with the correct position
   */
  LHCb::MCVertex* findVertex(LHCb::MCVertices*, LHCb::MCParticles*);
  LHCb::MCParticle* findPlaceholder(const LHCb::MCParticles* parts);
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
  /// Delete a complete tree from event record
  void deleteParticle(LHCb::MCParticle* P, LHCb::MCVertices* m_vertexContainer,
                      LHCb::MCParticles* m_particleContainer);
};

template <typename T>
std::pair<T*, T*> GaussRDMergeAndClean::get_and_print(const std::string& loc) {
  auto con = get<T>(loc);
  auto loc_s = m_signal_tes_prefix + loc;
  auto con_s = get<T>(loc_s);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Got " << con->size() << " from " << loc << endmsg;
    debug() << "Got " << con_s->size() << " from " << loc_s << endmsg;
  }
  return std::pair<T*, T*>(con, con_s);
}
#endif  // GaussRDMergeAndClean_H
