#ifndef REDECAY_SERVICE_H
#define REDECAY_SERVICE_H 1

// Include files
// from STD & STL
#include <list>
#include <map>
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/StatusCode.h"

// from GaussRedecay
#include "GaussRedecay/IGaussRedecayCtr.h"  //Abstract control interface
#include "GaussRedecay/IGaussRedecayStr.h"  //Abstract storage interface

// Forwad declarations
// from Gaudi
class ISvcLocator;
template <class TYPE>
class SvcFactory;
class MCCloner;

/**  @class GaussRedecay GaussRedecay.h
 *
 *   Implementation of abstract Interfaces IGaussRedecayStr
 *   (for storing redecay information)
 *    and IGaussRedecayCtr (for controlling redecay flow)
 *
 *    @author: Dominik Muller dominik.muller@cern.ch
 */

class GaussRedecay : public Service,
                     virtual public IGaussRedecayStr,
                     virtual public IGaussRedecayCtr {
  /// friend factory
  friend class SvcFactory<GaussRedecay>;

  public:
  /// useful typedef
  typedef std::vector<std::string> Strings;

  /**  initialize
   *   @return status code
   */
  StatusCode initialize() override;

  /**  initialize
   *   @return status code
   */
  StatusCode finalize() override;

  StatusCode queryInterface(const InterfaceID& iid, void** pI) override;

  // Implementation of the control interface IGaussRedecayCtr
  size_t numberOfRedecays() const override { return m_max_rd_counter; };

  /** Allows any algorithm to query the service and ask what we currently up
   * to.
   * 0 - default running, no redecay etc.
   * 1 - UE simulation phase
   * 2 - Signal simulation phase
   *  @return int
   */
  int getPhase() const override;
  void setPhase(int p) override { m_phase = p; }

  /** Registers a new event, returns false if the UD is already simulated and
   * should be reused.
   *  Returns true if everything needs to be redone and deletes the internal
   * storage objects.
   *
   *  @return bool
   */
  unsigned long long getEncodedOriginalEvtInfo() override;

  int getRedecayMode() const override { return m_rd_mode; }

  // Implementation of the storage interface IGaussRedecayStr

  int registerForRedecay(Particle part, int pileup_id) override;
  std::map<int, Particle>* getRegisteredForRedecay() override;
  int getNPileUp() override { return m_sig_map.size(); };

  /** Registers a new event, returns false if the UD is already simulated and
   * should be reused.
   *  Returns true if everything needs to be redone and deletes the internal
   * storage objects.
   *
   *  @return bool
   */

  bool registerNewEvent(unsigned long long evtNumber,
                        unsigned long long runNumber) override;

  /** Functions to save the different MC objects.
   *  string argument allows storage split by the string.
   *  Necessary as e.g. MCHits are stored seperately for
   *  each subdetector in the GetTrackerHitsAlg
   *
   *  @param Pointer to the object to clone.
   *  @param Optional string. e.g. TES location
   */

  LHCb::MCParticle* cloneMCP(const LHCb::MCParticle* mcp) override;
  LHCb::MCParticles* getClonedMCPs() override;

  LHCb::MCVertex* cloneMCV(const LHCb::MCVertex* mcVertex) override;
  LHCb::MCVertices* getClonedMCVs() override;

  LHCb::MCHit* cloneMCHit(const LHCb::MCHit* mchit,
                          const std::string& vol) override;
  LHCb::MCHits* getClonedMCHits(const std::string& vol) override;

  LHCb::MCCaloHit* cloneMCCaloHit(const LHCb::MCCaloHit* mchit,
                                  const std::string& vol) override;
  LHCb::MCCaloHits* getClonedMCCaloHits(const std::string& vol) override;

  LHCb::MCRichHit* cloneMCRichHit(const LHCb::MCRichHit* mchit) override;
  LHCb::MCRichHits* getClonedMCRichHits() override;

  LHCb::MCRichOpticalPhoton* cloneMCRichOpticalPhoton(
      const LHCb::MCRichOpticalPhoton* mchit) override;
  LHCb::MCRichOpticalPhotons* getClonedMCRichOpticalPhotons() override;

  LHCb::MCRichSegment* cloneMCRichSegment(
      const LHCb::MCRichSegment* mchit) override;
  LHCb::MCRichSegments* getClonedMCRichSegments() override;

  LHCb::MCRichTrack* cloneMCRichTrack(const LHCb::MCRichTrack* mchit) override;
  LHCb::MCRichTracks* getClonedMCRichTracks() override;

  LHCb::GenCollision* cloneGenCollision(
      const LHCb::GenCollision* mchit) override;
  LHCb::GenCollisions* getClonedGenCollisions() override;
  std::set<int> getUsedPlaceholderIDs() override;

  protected:
  /** standard constructor
   *  @see Service
   *  @param name instrance name
   *  @param svc  pointer to service locator
   */
  GaussRedecay(const std::string& name, ISvcLocator* svc);

  /// (virtual destructor)
  ~GaussRedecay();

  private:
  MCCloner* m_mc_cloner;
  MCCloner* m_mc_cloner_copy;

  // Counter and max event number
  size_t m_rd_counter;
  size_t m_max_rd_counter;
  int m_phase;
  int m_rd_mode;
  int m_g4_reserve;
  unsigned long long m_org_evtNumber = 0;
  unsigned long long m_org_runNumber = 0;
  bool m_g4_initialized = false;

  // Signal information storage
  std::map<int, std::map<int, Particle>> m_sig_map;
  std::map<int, std::map<int, Particle>>::iterator m_pileup_it;
  int m_n_particles = 0;
  bool m_first_access = true;

  Gaudi::LorentzVector m_sig_mom;
  Gaudi::XYZTPoint m_sig_point;
  int m_sig_id;
};

#endif  ///<  REDECAY_SERVICE_H
