#ifndef GaussRedecayMergeAndClean_H
#define GaussRedecayMergeAndClean_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <utility>
#include "Event/MCVertex.h"

class MCCloner;
class IGaussRedecayStr;
namespace LHCb {
class MCVertex;
class MCParticle;
}


/** @class GaussRedecayMergeAndClean GaussRedecayMergeAndClean.h
 *
 * Algorithm to merge all the signal stuff back into the main containers
 * and clean the signal tree in the TES.
 *
 *  @author Dominik Muller
 *  @date   2016-4-1
 */
class GaussRedecayMergeAndClean : public GaudiAlgorithm {
  public:
  /// Standard constructor
  GaussRedecayMergeAndClean(const std::string& Name, ISvcLocator* SvcLoc);

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  protected:
  /** accessor to GaussRedecay Service
   *  @return pointer to GaussRedecay Service
   */
  private:
  /* Finds the vertex in the signal origin vertex in the full event
   * using the placeholder id. Also removes the placeholder from
   * the vertex and the list of particles.
   */
  LHCb::MCVertex* findVertex(int placeholder);
  /* Finds the MCParticle of the given placeholder in the provided
   * list of particles.
   */
  LHCb::MCParticle* findPlaceholder(const LHCb::MCParticles* parts, int placeholder);
  std::string m_gaussRDSvcName;
  IGaussRedecayStr* m_gaussRDStrSvc;
  MCCloner* m_temp_cloner = nullptr;

  std::string m_particlesLocation;
  std::string m_verticesLocation;
  std::vector<std::string> m_hitsLocations;
  std::vector<std::string> m_calohitsLocations;
  std::string m_richHitsLocation;
  std::string m_richOpticalPhotonsLocation;
  std::string m_richSegmentsLocation;
  std::string m_richTracksLocation;
  std::string m_signal_tes_prefix;
  std::string m_hepMCEventLocation;
  std::string m_genCollisionLocation;

  std::pair<LHCb::MCParticles*, LHCb::MCParticles*> m_mcparticles;
  std::pair<LHCb::MCVertices*, LHCb::MCVertices*> m_mcvertices;
  template <typename T>
  std::pair<T*, T*> get_and_print(const std::string&);
  /// Delete a complete tree from event record
  void deleteParticle(LHCb::MCParticle* P, LHCb::MCVertices* m_vertexContainer,
                      LHCb::MCParticles* m_particleContainer);
  void fix_connections(int placeholder, int original_id);
};

template <typename T>
std::pair<T*, T*> GaussRedecayMergeAndClean::get_and_print(const std::string& loc) {
  auto con = get<T>(loc);
  auto loc_s = m_signal_tes_prefix + loc;
  auto con_s = get<T>(loc_s);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Got " << con->size() << " from " << loc << endmsg;
    debug() << "Got " << con_s->size() << " from " << loc_s << endmsg;
  }
  return std::pair<T*, T*>(con, con_s);
}
#endif  // GaussRedecayMergeAndClean_H
