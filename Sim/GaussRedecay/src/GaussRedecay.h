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
  virtual StatusCode initialize() override;

  /**  initialize
   *   @return status code
   */
  virtual StatusCode finalize() override;

  virtual StatusCode queryInterface(const InterfaceID& iid, void** pI) override;

  // Implementation of the control interface IGaussRedecayCtr
  virtual size_t numberOfRedecays() const override { return m_max_rd_counter; };

  /** Allows any algorithm to query the service and ask what we currently up
   * to.
   * 0 - default running, no redecay etc.
   * 1 - UE simulation phase
   * 2 - Signal simulation phase
   *  @return int
   */
  virtual int getPhase() const override;
  virtual void setPhase(int p) override { m_phase = p; }

  virtual int getRedecayMode() const override { return m_rd_mode; }

  // Implementation of the storage interface IGaussRedecayStr

  virtual int registerForRedecay(Particle part, int pileup_id) override;
  virtual std::map<int, Particle> *getRegisteredForRedecay() override;
  int getNPileUp() override {return m_sig_map.size();};

  /** Registers a new event, returns false if the UD is already simulated and
   * should be reused.
   *  Returns true if everything needs to be redone and deletes the internal
   * storage objects.
   *
   *  @return bool
   */

  virtual bool registerNewEvent() override;

  /** Functions to save the different MC objects.
   *  string argument allows storage split by the string.
   *  Necessary as e.g. MCHits are stored seperately for
   *  each subdetector in the GetTrackerHitsAlg
   *
   *  @param Pointer to the object to clone.
   *  @param Optional string. e.g. TES location
   */

  virtual LHCb::MCParticle* cloneMCP(const LHCb::MCParticle* mcp) override;
  virtual LHCb::MCParticles* getClonedMCPs() override;

  virtual LHCb::MCVertex* cloneMCV(const LHCb::MCVertex* mcVertex) override;
  virtual LHCb::MCVertices* getClonedMCVs() override;

  virtual LHCb::MCHit* cloneMCHit(const LHCb::MCHit* mchit,
                                  const std::string& vol) override;
  virtual LHCb::MCHits* getClonedMCHits(const std::string& vol) override;

  virtual LHCb::MCCaloHit* cloneMCCaloHit(const LHCb::MCCaloHit* mchit,
                                          const std::string& vol) override;
  virtual LHCb::MCCaloHits* getClonedMCCaloHits(
      const std::string& vol) override;

  virtual LHCb::MCRichHit* cloneMCRichHit(
      const LHCb::MCRichHit* mchit) override;
  virtual LHCb::MCRichHits* getClonedMCRichHits() override;

  virtual LHCb::MCRichOpticalPhoton* cloneMCRichOpticalPhoton(
      const LHCb::MCRichOpticalPhoton* mchit) override;
  virtual LHCb::MCRichOpticalPhotons* getClonedMCRichOpticalPhotons() override;

  virtual LHCb::MCRichSegment* cloneMCRichSegment(
      const LHCb::MCRichSegment* mchit) override;
  virtual LHCb::MCRichSegments* getClonedMCRichSegments() override;

  virtual LHCb::MCRichTrack* cloneMCRichTrack(
      const LHCb::MCRichTrack* mchit) override;
  virtual LHCb::MCRichTracks* getClonedMCRichTracks() override;

  virtual LHCb::GenCollision* cloneGenCollision(
      const LHCb::GenCollision* mchit) override;
  virtual LHCb::GenCollisions* getClonedGenCollisions() override;
  virtual std::set<int> getUsedPlaceholderIDs() override;

  protected:
  /** standard constructor
   *  @see Service
   *  @param name instrance name
   *  @param svc  pointer to service locator
   */
  GaussRedecay(const std::string& name, ISvcLocator* svc);

  /// (virtual destructor)
  virtual ~GaussRedecay();

  private:
  MCCloner* m_mc_cloner;
  MCCloner* m_mc_cloner_copy;

  // Counter and max event number
  size_t m_rd_counter;
  size_t m_max_rd_counter;
  int m_phase;
  int m_rd_mode;
  int m_g4_reserve;
  bool m_g4_initialized=false;

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
