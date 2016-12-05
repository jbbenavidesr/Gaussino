#ifndef GAUSS_IGAUSSRDSTR_H
#define GAUSS_IGAUSSRDSTR_H 1
/// STD and STL
//#include   <utility>
/// Include files from the Framework
#include "GaudiKernel/IService.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"

// From Geant4
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

class MCCloner;
#include "Event/GenCollision.h"
#include "Event/MCCaloHit.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/Particle.h"

/** @class IGaussRedecayStr IGaussRedecayStr.h GiGa/IGaussRedecayStr.h
 *
 *  Abstract interface to the storage of GaussRedecay
 *
 *
 *  @author Dominik Muller
 */

class GAUDI_API IGaussRedecayStr : virtual public IService {
  public:
  DeclareInterfaceID(IGaussRedecayStr, 1, 0);

  /**  initialize
   *   @return status code
   */
  virtual StatusCode initialize() = 0;

  /**  initialize
   *   @return status code
   */
  virtual StatusCode finalize() = 0;

  public:
  struct Particle {
    int pdg_id = 0;
    Gaudi::LorentzVector momentum;
    Gaudi::XYZTPoint point;
  };
  static const int PlaceholderPDGID = 424242;

  /** Function to register a particle for the redecay, returns
   * a temp particle ID which can be used to tag the particle.
   *
   *  @param Particle struct holding the necessary information
   *  @return int temp particle id unique for this Particle
   */
  virtual int registerForRedecay(Particle part, int pileup_id) = 0;

  /*This function rolls over all saved pileup events and returns the map of
   * objects to redecay in this one pile up. It relies on the caller to call
   * it n-redecay times.*/

  virtual std::map<int, Particle>* getRegisteredForRedecay() = 0;
  virtual int getNPileUp() = 0;
  virtual std::set<int> getUsedPlaceholderIDs() = 0;

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

  virtual LHCb::MCVertex* cloneMCV(const LHCb::MCVertex* mcVertex) = 0;
  virtual LHCb::MCVertices* getClonedMCVs() = 0;

  virtual LHCb::MCHit* cloneMCHit(const LHCb::MCHit* mchit,
                                  const std::string& vol) = 0;
  virtual LHCb::MCHits* getClonedMCHits(const std::string& vol) = 0;

  virtual LHCb::MCCaloHit* cloneMCCaloHit(const LHCb::MCCaloHit* mchit,
                                          const std::string& vol) = 0;
  virtual LHCb::MCCaloHits* getClonedMCCaloHits(const std::string& vol) = 0;

  virtual LHCb::MCRichHit* cloneMCRichHit(const LHCb::MCRichHit* mchit) = 0;
  virtual LHCb::MCRichHits* getClonedMCRichHits() = 0;

  virtual LHCb::MCRichOpticalPhoton* cloneMCRichOpticalPhoton(
      const LHCb::MCRichOpticalPhoton* mchit) = 0;
  virtual LHCb::MCRichOpticalPhotons* getClonedMCRichOpticalPhotons() = 0;

  virtual LHCb::MCRichSegment* cloneMCRichSegment(
      const LHCb::MCRichSegment* mchit) = 0;
  virtual LHCb::MCRichSegments* getClonedMCRichSegments() = 0;

  virtual LHCb::MCRichTrack* cloneMCRichTrack(
      const LHCb::MCRichTrack* mchit) = 0;
  virtual LHCb::MCRichTracks* getClonedMCRichTracks() = 0;

  virtual LHCb::GenCollision* cloneGenCollision(
      const LHCb::GenCollision* mchit) = 0;
  virtual LHCb::GenCollisions* getClonedGenCollisions() = 0;

  protected:
  /// virtual destructor
  virtual ~IGaussRedecayStr(){};
};

// ============================================================================
#endif  ///< GIGA_GIGASVC_H
// ============================================================================
