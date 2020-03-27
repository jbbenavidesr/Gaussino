// $Id: ParticleGun.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_PARTICLEGUN_H
#define PARTICLEGUNS_PARTICLEGUN_H 1

#include "Defaults/Locations.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "GaudiAlg/Transformer.h"
#include "NewRnd/RndAlgSeeder.h"
#include "HepMC3/GenParticle.h"
#include "HepMCUser/typedefs.h"

#include <atomic>
#include <vector>
#include <mutex>

// Forward declarations
class IParticleGunTool;
class IPileUpTool;
class IDecayTool;
class ISampleGenerationTool;
class IVertexSmearingTool;
class IFullGenEventCutTool;
class IGenCutTool;

/** @class ParticleGun ParticleGun.h "ParticleGun.h"
 *
 *  Main algorithm to generate particle gun events.
 *
 *  @author Patrick Robbe
 *  @date   2008-05-18
 */
class ParticleGun
    : public Gaudi::Functional::MultiTransformer<
          std::tuple<HepMC3::GenEventPtrs, LHCb::GenCollisions, LHCb::GenHeader>( const LHCb::GenHeader& ), Gaudi::Functional::Traits::BaseClass_t<RndAlgSeeder>>
{
private:
  Gaudi::Property<std::string> m_particleGunToolName{this, "ParticleGunTool", "GenericGun"};
  Gaudi::Property<int> m_eventType{this, "EventType", 50000000};
  Gaudi::Property<std::string> m_numberOfParticlesToolName{this, "NumberOfParticlesTool",
                                                           "FixedNInteractions/FixedNParticles"};
  Gaudi::Property<std::string> m_decayToolName{this, "DecayTool", ""};
  Gaudi::Property<std::string> m_vertexSmearingToolName{this, "VertexSmearingTool", ""};
  Gaudi::Property<std::string> m_fullGenEventCutToolName{this, "FullGenEventCutTool", ""};
  Gaudi::Property<std::string> m_genCutToolName{this, "GenCutTool", ""};
  Gaudi::Property<int> m_sigPdgCode{this, "SignalPdgCode", 0};
  Gaudi::Property<bool> m_sampleMass{this, "SampleMass", false};
  Gaudi::Property<double> m_MassRange_min{this, "MassRange_min", -1.};
  Gaudi::Property<double> m_MassRange_max{this, "MassRange_max", -1.};

public:
  typedef std::vector<HepMC3::GenParticlePtr> ParticleVector;
  /// Standard constructor
  ParticleGun( const std::string& name, ISvcLocator* pSvcLocator )
      : MultiTransformer( name, pSvcLocator,
                          {KeyValue{"GenHeaderInputLocation", Gaussino::GenHeaderLocation::PreGeneration}},
                          {{KeyValue{"HepMCEventLocation", Gaussino::HepMCEventLocation::Default},
                            KeyValue{"GenCollisionLocation", LHCb::GenCollisionLocation::Default},
                            KeyValue{"GenHeaderOutputLocation", Gaussino::GenHeaderLocation::Default}}} )
  {
  }

  virtual ~ParticleGun() = default;

  /** Algorithm initialization.
   *  -# Initializes the common Gaudi random number generator used in all
   *     generators,
   *  -# Retrieve particle gun tool, decay tool, vertex smearing tool and
   *     full event cut tool used in the generation of events.
   */
  virtual StatusCode initialize() override;

  /** Algorithm execution.
   *  Repeat the following sequence until a good set of interactions is
   *  generated.
   *  -#
   */
  virtual std::tuple<HepMC3::GenEventPtrs, LHCb::GenCollisions, LHCb::GenHeader>
  operator()( const LHCb::GenHeader& ) const override;

  /** Algorithm finalization.
   *  Print generation counters.
   */
  virtual StatusCode finalize() override;

protected:
  /// Decay the event with the IDecayTool.
  HepMC3::GenParticlePtr decayEvent( HepMC3::GenEventPtr theEvent, ParticleVector& particleList, HepRandomEnginePtr & engine, StatusCode& sc ) const;

  /// Perpare the particle containers
  void prepareInteraction( HepMC3::GenEventPtrs * theEvents, LHCb::GenCollisions* theCollisions,
                           HepMC3::GenEventPtr& theGenEvent, LHCb::GenCollision*& theGenCollision ) const;

private:
  IParticleGunTool* m_particleGunTool{nullptr};
  IPileUpTool* m_numberOfParticlesTool{nullptr};
  IDecayTool* m_decayTool{nullptr};
  ISampleGenerationTool* m_sampleGenerationTool{nullptr};
  IVertexSmearingTool* m_vertexSmearingTool{nullptr};
  IFullGenEventCutTool* m_fullGenEventCutTool{nullptr};
  IGenCutTool* m_genCutTool{nullptr};

  /// Name to put in the event
  std::string m_particleGunName;

  mutable std::atomic_uint m_nEvents{}; ///< Number of generated events

  mutable std::atomic_uint m_nAcceptedEvents{}; ///< Number of accepted events

  mutable std::atomic_uint m_nParticles{}; ///< Number of generated particles

  /// Number of particles in accepted events
  mutable std::atomic_uint m_nAcceptedParticles{};

  /// Counter of events before the full event generator level cut
  mutable std::atomic_uint m_nBeforeFullEvent{};

  /// Counter of events after the full event generator level cut
  mutable std::atomic_uint m_nAfterFullEvent{};

  /// Counter of events before the generator level cut
  mutable std::atomic_uint m_nBeforeCut{};

  /// Counter of events after the generator level cut
  mutable std::atomic_uint m_nAfterCut{};
  mutable std::mutex debug_lock;
};
#endif // PARTICLEGUNS_PARTICLEGUN_H
