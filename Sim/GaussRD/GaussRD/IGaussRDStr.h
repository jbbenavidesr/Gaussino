// $Id: IGaussRDStr.h,v 0.1 2015-12-10 18:58:18 ibelyaev Exp $
// ============================================================================
// ============================================================================
//
// ============================================================================
#ifndef GAUSS_IGAUSSRDSTR_H
#define GAUSS_IGAUSSRDSTR_H 1
/// STD and STL
//#include   <utility>
/// Include files from the Framework
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IService.h"

// From Geant4
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

class MCCloner;
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/GenCollision.h"

static const InterfaceID IID_IGaussRDStr(517635934, 1, 0);

/** @class IGaussRDStr IGaussRDStr.h GiGa/IGaussRDStr.h
 *
 *  definition of the abstract interface to Geant4 Service
 *   for event-by-event communications with Geant4
 *
 *  @author Vanya Belyaev
 */

class IGaussRDStr : virtual public IService {
  public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGaussRDStr; };

  /**  initialize
   *   @return status code
   */
  virtual StatusCode initialize() = 0;

  /**  initialize
   *   @return status code
   */
  virtual StatusCode finalize() = 0;

  public:
  /** Functions to save the different MC objects.
   *  string argument allows storage split by the string.
   *  Necessary as e.g. MCHits are stored seperately for
   *  each subdetector in the GetTrackerHitsAlg
   *
   *  @param Pointer to the object to clone.
   *  @param Optional string. e.g. TES location
   */
  virtual LHCb::MCParticle* cloneMCP(const LHCb::MCParticle* mcp) = 0;
  virtual LHCb::MCParticles* getClonedMCPs() = 0;

  virtual void setSignal(LHCb::MCParticle* mcp) = 0;
  virtual LHCb::MCParticle* getSignal() = 0;

  virtual LHCb::MCVertex* cloneMCV(const LHCb::MCVertex* mcVertex) = 0;
  virtual LHCb::MCVertices* getClonedMCVs() = 0;

  virtual LHCb::MCHit* cloneMCHit(const LHCb::MCHit* mchit, const std::string& vol) = 0;
  virtual LHCb::MCHits* getClonedMCHits(const std::string& vol) = 0;

  virtual LHCb::MCCaloHit* cloneMCCaloHit(const LHCb::MCCaloHit* mchit, const std::string& vol) = 0;
  virtual LHCb::MCCaloHits* getClonedMCCaloHits(const std::string& vol) = 0;

  virtual LHCb::MCRichHit* cloneMCRichHit(const LHCb::MCRichHit* mchit) = 0;
  virtual LHCb::MCRichHits* getClonedMCRichHits() = 0;

  virtual LHCb::MCRichOpticalPhoton* cloneMCRichOpticalPhoton(const LHCb::MCRichOpticalPhoton* mchit) = 0;
  virtual LHCb::MCRichOpticalPhotons* getClonedMCRichOpticalPhotons() = 0;

  virtual LHCb::MCRichSegment* cloneMCRichSegment(const LHCb::MCRichSegment* mchit) = 0;
  virtual LHCb::MCRichSegments* getClonedMCRichSegments() = 0;

  virtual LHCb::MCRichTrack* cloneMCRichTrack(const LHCb::MCRichTrack* mchit) = 0;
  virtual LHCb::MCRichTracks* getClonedMCRichTracks() = 0;

  virtual LHCb::GenCollision* cloneGenCollision(const LHCb::GenCollision* mchit) = 0;
  virtual LHCb::GenCollisions* getClonedGenCollisions() = 0;

  protected:
  /// virtual destructor
  virtual ~IGaussRDStr(){};
};

// ============================================================================
#endif  ///< GIGA_GIGASVC_H
// ============================================================================

