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

// from GaussRedecay
#include "GaussRedecay/IGaussRedecayStr.h"  //Abstract storage interface
#include "GaussRedecay/IGaussRedecayCtr.h"  //Abstract control interface

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

    virtual StatusCode queryInterface(const InterfaceID& iid,
                                      void** pI) override;

    // Implementation of the control interface IGaussRedecayCtr
    virtual size_t numberOfRedecays() const override {
        return m_max_rd_counter;
    };

    /** Allows any algorithm to query the service and ask what we currently up
     * to.
     * 0 - default running, no redecay etc.
     * 1 - UE simulation phase
     * 2 - Signal simulation phase
     *  @return int
     */
    virtual int whatShouldIDo() const override;
    virtual void setPhase(int p) override { m_phase = p; }

    // Implementation of the storage interface IGaussRedecayStr
    //
    /** Functions to save the different MC objects.
     *  string argument allows storage split by the string.
     *  Necessary as e.g. MCHits are stored seperately for
     *  each subdetector in the GetTrackerHitsAlg
     *
     *  @param Pointer to the object to clone.
     *  @param Optional string. e.g. TES location
     */

    /** Registers a new event, returns false if the UD is already simulated and
     * should be reused.
     *  Returns true if everything needs to be redone and deletes the internal
     * storage objects.
     *
     *  @return bool
     */

    virtual bool registerNewEvent() override;

    virtual void setSignalMomentum(const Gaudi::LorentzVector& mom) override {
        m_sig_mom = mom;
    }
    virtual void setSignalOrigin(const Gaudi::XYZTPoint& point) override {
        m_sig_point = point;
    }
    virtual void setSignalID(int id) override { m_sig_id = id; }
    virtual Gaudi::LorentzVector getSignalMomentum() override {
        return m_sig_mom;
    }
    virtual Gaudi::XYZTPoint getSignalOrigin() override { return m_sig_point; }
    virtual int getSignalID() override { return m_sig_id; }

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
    virtual LHCb::MCRichOpticalPhotons* getClonedMCRichOpticalPhotons()
        override;

    virtual LHCb::MCRichSegment* cloneMCRichSegment(
        const LHCb::MCRichSegment* mchit) override;
    virtual LHCb::MCRichSegments* getClonedMCRichSegments() override;

    virtual LHCb::MCRichTrack* cloneMCRichTrack(
        const LHCb::MCRichTrack* mchit) override;
    virtual LHCb::MCRichTracks* getClonedMCRichTracks() override;

    virtual LHCb::GenCollision* cloneGenCollision(
        const LHCb::GenCollision* mchit) override;
    virtual LHCb::GenCollisions* getClonedGenCollisions() override;

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

    Gaudi::LorentzVector m_sig_mom;
    Gaudi::XYZTPoint m_sig_point;
    int m_sig_id;
};

#endif  ///<  REDECAY_SERVICE_H
