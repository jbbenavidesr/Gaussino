// $Id: ParticleGun.h, v0.1 2016-03-29 16:18:24 dmuller Exp $
#ifndef GAUSSRD_SIGNALDECAY_H
#define GAUSSRD_SIGNALDECAY_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"

// Forward declarations
class IPileUpTool;
class IDecayTool;
class ISampleGenerationTool;
class IFullGenEventCutTool;
class IGenCutTool;

class IGaussRDStr;

namespace HepMC {
class GenEvent;
}

/** @class GaussRDSignalDecay GaussRDSignalDecay.h "GaussRDSignalDecay.h"
 *
 *  Algorithm to take vertex and particle, build a new HepMC event and decay it.
 *  Many things taken from ParticleGun class by Patrick Robbe
 *
 *  @author Dominik Muller
 *  @date   2016-03-29
 */
class GaussRDSignalDecay : public GaudiAlgorithm {
  public:
  typedef std::vector<HepMC::GenParticle*> ParticleVector;
  /// Standard constructor
  GaussRDSignalDecay(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~GaussRDSignalDecay();

  /** Algorithm initialization.
   *  -# Initializes the common Gaudi random number generator used in all
   *     generators,
   *  -# Retrieve particle gun tool, decay tool, vertex smearing tool and
   *     full event cut tool used in the generation of events.
   */
  virtual StatusCode initialize();

  /** Algorithm execution.
   *  Decay the particle until a good candidate is found.
   *  -#
   */
  virtual StatusCode execute();

  /** Algorithm finalization.
   */
  virtual StatusCode finalize();

  protected:
  /// Decay the event with the IDecayTool.
  HepMC::GenParticle* decayEvent(LHCb::HepMCEvent* theEvent, ParticleVector& particleList, StatusCode& sc);

  /// Perpare the particle containers
  void prepareInteraction(LHCb::HepMCEvents* theEvents, LHCb::GenCollisions* theCollisions, HepMC::GenEvent*& theGenEvent,
                          LHCb::GenCollision*& theGenCollision) const;

  private:
  int m_eventType;  ///< Event type (set by options)

  /// Location where to store generator events (set by options)
  std::string m_hepMCEventLocation;

  /// Location where to store the Header of the events (set by options)
  std::string m_genHeaderLocation;

  /// Location where to store HardInfo (set by options)
  std::string m_genCollisionLocation;

  IDecayTool* m_decayTool = nullptr;                      ///< Decay tool
  IFullGenEventCutTool* m_fullGenEventCutTool = nullptr;  ///< Cut tool
  IGenCutTool* m_genCutTool = nullptr;                    ///< Cut tool
  IGaussRDStr* m_gaussRDStrSvc = nullptr;                 ///< GaussRD storage service

  /// Name of the IDecayTool (set by options)
  std::string m_decayToolName;

  /// Name of the IFullGenEventCutTool (set by options)
  std::string m_fullGenEventCutToolName;

  /// Name of the IGenCutTool (set by options)
  std::string m_genCutToolName;

  unsigned int m_nEvents = 0;  ///< Number of generated events

  unsigned int m_nAcceptedEvents = 0;  ///< Number of accepted events

  /// Number of particles in accepted events
  unsigned int m_nAcceptedParticles = 0;

  /// Counter of events before the full event generator level cut
  unsigned int m_nBeforeFullEvent = 0;

  /// Counter of events after the full event generator level cut
  unsigned int m_nAfterFullEvent = 0;

  /// Counter of events before the generator level cut
  unsigned int m_nBeforeCut = 0;

  /// Counter of events after the generator level cut
  unsigned int m_nAfterCut = 0;

  /// PDG id of signal particle.
  int m_sigPdgCode;

  std::string m_gaussRDSvcName;
};
#endif  // GAUSSRD_SIGNALDECAY_H
