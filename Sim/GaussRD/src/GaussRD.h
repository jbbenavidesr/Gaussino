// $Id: GiGa.h,v 1.9 2009-10-14 13:50:02 gcorti Exp $
#ifndef REDECAY_SERVICE_H
#define REDECAY_SERVICE_H 1

// Include files
// from STD & STL
#include <string>
#include <list>
#include <vector>
#include <map>

// from Gaudi
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/IToolSvc.h"

// from GaussRD
#include "GaussRD/IGaussRDStr.h"  //Abstract storage interface
#include "GaussRD/IGaussRDCtr.h"  //Abstract control interface

// Forwad declarations
// from Gaudi
class ISvcLocator;
template <class TYPE>
class SvcFactory;
class MCCloner;

/**  @class GaussRD GaussRD.h
 *
 *   Implementation of abstract Interfaces IGaussRDStr
 *   (for event-by-event communications with Geant4)
 *    and IGiGaSetUpSvc (for configuration of Geant4)
 *
 *    @author: Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class GaussRD : public Service, virtual public IGaussRDStr, virtual public IGaussRDCtr {
  /// friend factory
  friend class SvcFactory<GaussRD>;

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

  // Implementation of the control interface IGaussRDCtr
  virtual size_t numberOfRedecays() const override { return m_max_rd_counter; };

  /** Allows any algorithm to query the service and ask what we currently up to.
   * 0 - default running, no redecay etc.
   * 1 - UE simulation phase
   * 2 - Signal simulation phase
   *  @return int
   */
  virtual int whatShouldIDo() const override;
  virtual void setPhase(int p) override { m_phase = p; }

  // Implementation of the storage interface IGaussRDStr
  //
  /** Functions to save the different MC objects.
   *  string argument allows storage split by the string.
   *  Necessary as e.g. MCHits are stored seperately for
   *  each subdetector in the GetTrackerHitsAlg
   *
   *  @param Pointer to the object to clone.
   *  @param Optional string. e.g. TES location
   */

  /** Registers a new event, returns false if the UD is already simulated and should be reused.
   *  Returns true if everything needs to be redone and deletes the internal storage objects.
   *
   *  @return bool
   */

  virtual bool registerNewEvent() override;

  virtual LHCb::MCParticle* cloneMCP(const LHCb::MCParticle* mcp) override;
  virtual LHCb::MCParticles* getClonedMCPs() override;

  virtual void setSignal(LHCb::MCParticle* mcp) override { m_signal_particle = m_org_signal_particle = mcp; };
  virtual LHCb::MCParticle* getSignal() override { return m_signal_particle; };

  virtual LHCb::MCVertex* cloneMCV(const LHCb::MCVertex* mcVertex) override;
  virtual LHCb::MCVertices* getClonedMCVs() override;

  virtual LHCb::MCHit* cloneMCHit(const LHCb::MCHit* mchit, const std::string& vol) override;
  virtual LHCb::MCHits* getClonedMCHits(const std::string& vol) override;

  virtual LHCb::MCCaloHit* cloneMCCaloHit(const LHCb::MCCaloHit* mchit, const std::string& vol) override;
  virtual LHCb::MCCaloHits* getClonedMCCaloHits(const std::string& vol) override;

  virtual LHCb::MCRichHit* cloneMCRichHit(const LHCb::MCRichHit* mchit) override;
  virtual LHCb::MCRichHits* getClonedMCRichHits() override;

  virtual LHCb::MCRichOpticalPhoton* cloneMCRichOpticalPhoton(const LHCb::MCRichOpticalPhoton* mchit) override;
  virtual LHCb::MCRichOpticalPhotons* getClonedMCRichOpticalPhotons() override;

  virtual LHCb::MCRichSegment* cloneMCRichSegment(const LHCb::MCRichSegment* mchit) override;
  virtual LHCb::MCRichSegments* getClonedMCRichSegments() override;

  virtual LHCb::MCRichTrack* cloneMCRichTrack(const LHCb::MCRichTrack* mchit) override;
  virtual LHCb::MCRichTracks* getClonedMCRichTracks() override;

  virtual LHCb::GenCollision* cloneGenCollision(const LHCb::GenCollision* mchit) override;
  virtual LHCb::GenCollisions* getClonedGenCollisions() override;

  protected:
  /** standard constructor
   *  @see Service
   *  @param name instrance name
   *  @param svc  pointer to service locator
   */
  GaussRD(const std::string& name, ISvcLocator* svc);

  /// (virtual destructor)
  virtual ~GaussRD();

  private:
  MCCloner* m_mc_cloner;
  MCCloner* m_mc_cloner_copy;

  // Pointer to the signal MC particle in the current scope, will be changed and adjusted by
  // registerNewEvent()
  LHCb::MCParticle* m_signal_particle;
  // Pointer to the signal particle in the original version of everything, needed to obtain newest clone in each event.
  // registerNewEvent()
  LHCb::MCParticle* m_org_signal_particle;

  // Counter and max event number
  size_t m_rd_counter;
  size_t m_max_rd_counter;
  int m_phase;
};

#endif  ///<  REDECAY_SERVICE_H
