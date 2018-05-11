// $Id: Generation.h,v 1.9 2007-07-03 16:31:36 gcorti Exp $
#ifndef GENERATORS_GENERATION_H
#define GENERATORS_GENERATION_H 1

#include "GaudiAlg/Transformer.h"

#include <array>
#include <atomic>
#include <string>
#include <tuple>

#include "Event/GenFSR.h"

// Forward declarations
class ISampleGenerationTool;
class IPileUpTool;
class IDecayTool;
class IVertexSmearingTool;
class IFullGenEventCutTool;
class ICounterLogFile;

#include "Defaults/Locations.h"

#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "HepMC/GenEvent.h"
#include "NewRnd/RndAlgSeeder.h"

namespace HepMC
{
  class GenParticle;
}

/** @class Generation Generation.h "Generation.h"
 *
 *  Main algorithm to generate Monte Carlo events.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-11
 *
 *  HepMC3 + Gaudi functional modifications
 *
 *  @author Dominik Muller
 *  @date 2018-03-16
 */
class Generation
    : public Gaudi::Functional::MultiTransformer<
          std::tuple<std::vector<HepMC::GenEvent>, LHCb::GenCollisions, LHCb::GenHeader>( const LHCb::GenHeader& ),
          Gaudi::Functional::Traits::BaseClass_t<RndAlgSeeder>>
{
private:
  Gaudi::Property<std::string> m_sampleGenerationToolName{this, "SampleGenerationTool", "MinimumBias",
                                                          "Name of the ISampleGenerationTool - MinimumBias, ..."};
  Gaudi::Property<int> m_eventType{this, "EventType", 30000000, "Event type"};
  Gaudi::Property<std::string> m_pileUpToolName{this, "PileUpTool", "FixedLuminosity", "Name of the IPileUpTool"};
  Gaudi::Property<std::string> m_decayToolName{this, "DecayTool", "EvtGenDecay", "Name of the IDecayTool"};
  Gaudi::Property<std::string> m_vertexSmearingToolName{this, "VertexSmearingTool", "BeamSpotSmearVertex",
                                                        "Name of the IVertexSmearingTool"};
  Gaudi::Property<std::string> m_fullGenEventCutToolName{this, "FullGenEventCutTool", "",
                                                         "Name of the IFullGenEventCutTool"};
  Gaudi::Property<bool> m_commonVertex{this, "CommonVertex", false,
                                       "Flag to generate all pile up events at the same PV"};

  // FIXME: This will need adapting for the new TES access eventually
  Gaudi::Property<std::string> m_FSRName{this, "GenFSRLocation", LHCb::GenFSRLocation::Default,
                                         "Location where to store FSR counters"};

public:
  /// Standard constructor
  Generation( const std::string& name, ISvcLocator* pSvcLocator )
      : MultiTransformer( name, pSvcLocator,
                          {KeyValue{"GenHeaderInputLocation", Gaussino::GenHeaderLocation::PreGeneration}},
                          {{KeyValue{"HepMCEventLocation", Gaussino::HepMCEventLocation::Default},
                            KeyValue{"GenCollisionLocation", LHCb::GenCollisionLocation::Default},
                            KeyValue{"GenHeaderOutputLocation", Gaussino::GenHeaderLocation::Default}}} )
  {
  }

  virtual ~Generation() = default;

  /** Algorithm initialization.
   *  -# Initializes the common Gaudi random number generator used in all
   *     generators,
   *  -# Retrieve Pile-up tool, sample generation tool, decay tool,
   *     vertex smearing tool and full event cut tool used in the generation
   *     of events.
   */
  virtual StatusCode initialize() override;

  /** Algorithm execution.
   *  Repeat the following sequence until a good set of interactions is
   *  generated.
   *  -# Compute the number of interactions to generate in the event, using
   *     the IPileUpTool.
   *  -# Generate this number of interactions using the IProductionTool.
   *     The IProductionTool indicates if a good event has been generated
   *     (containing for example a B hadron).
   *  -# Decay with the IDecayTool all particles in the event, if this is
   *     a good event.
   *  -# Apply the full event generator level cut (using IFullEventGenCutTool)
   *     and accept or reject the event.
   *  -# Store in event store the accepted event.
   */
  virtual std::tuple<std::vector<HepMC::GenEvent>, LHCb::GenCollisions, LHCb::GenHeader>
  operator()( const LHCb::GenHeader& ) const override;

  /** Algorithm finalization.
   *  Print generation counters.
   */
  virtual StatusCode finalize() override;

protected:
  /// Decay the event with the IDecayTool.
  StatusCode decayEvent( HepMC::GenEvent* theEvent , CLHEP::HepRandomEngine & engine ) const;

private:
  /// Reference to file records data service
  IDataProviderSvc* m_fileRecordSvc = nullptr;

  /// Pile-up tool
  IPileUpTool* m_pileUpTool = nullptr;

  /// Decay tool
  IDecayTool* m_decayTool = nullptr;

  /// Xml Log file tool
  ICounterLogFile* m_xmlLogTool = nullptr;

  /// Sample tool
  ISampleGenerationTool* m_sampleGenerationTool = nullptr;

  /// Smearing tool
  IVertexSmearingTool* m_vertexSmearingTool = nullptr;

  /// Cut tool
  IFullGenEventCutTool* m_fullGenEventCutTool = nullptr;

  /// Number of generated events
  mutable std::atomic_uint m_nEvents{0};

  /// Number of accepted events
  mutable std::atomic_uint m_nAcceptedEvents{0};

  /// Number of generated interactions
  mutable std::atomic_uint m_nInteractions{0};

  /// Number of interactions in accepted events
  mutable std::atomic_uint m_nAcceptedInteractions{0};

  /// Description of the counter index
  enum interationCounterType {
    Oneb = 0, ///< interaction with >= 1 b quark
    Threeb,   ///< interaction with >= 3 b quarks
    PromptB,  ///< interaction with prompt B
    Onec,     ///< interaction with >= 1 c quark
    Threec,   ///< interaction with >= 3 c quarks
    PromptC,  ///< interaction with prompt C
    bAndc     ///< interaction with b and c
  };

  /// Type for interaction counter
  typedef std::array<std::atomic_uint, 7> interactionCounter;
  typedef std::array<std::string, 7> interactionCNames;

  /// Counter of content of generated interactions
  // FIXME: More braces never hurt but does this actually what we want?
  // As far as I know, just {} should trigger zero-initialization and yield
  // the same result as {{{0},{0},{0},{0},{0},{0},{0}}}
  mutable interactionCounter m_intC{};
  mutable interactionCounter m_intCAccepted{};

  /// Array of counter names
  const interactionCNames m_intCName{{"generated interactions with >= 1b", "generated interactions with >= 3b",
                                      "generated interactions with 1 prompt B", "generated interactions with >= 1c",
                                      "generated interactions with >= 3c", "generated interactions with >= prompt C",
                                      "generated interactions with b and c"}};

  /// Array of accepted counter names
  const interactionCNames m_intCAcceptedName{
      {"accepted interactions with >= 1b", "accepted interactions with >= 3b", "accepted interactions with 1 prompt B",
       "accepted interactions with >= 1c", "accepted interactions with >= 3c", "accepted interactions with >= prompt C",
       "accepted interactions with b and c"}};

  /// Counter of events before the full event generator level cut
  mutable std::atomic_uint m_nBeforeFullEvent{0};

  /// Counter of events after the full event generator level cut
  mutable std::atomic_uint m_nAfterFullEvent{0};

  /// TDS container
  /// LHCb::GenFSRs* m_genFSRs;
  /// FSR for current file
  LHCb::GenFSR* m_genFSR = nullptr;

  /** Update the counters counting on interactions.
   *  @param[in,out] theCounter Counter of events
   *  @param[in]     theEvent  The interaction to study
   */
  void updateInteractionCounters( interactionCounter& theCounter, const HepMC::GenEvent* theEvent ) const;

  /** Update the counters counting on interactions.
   *  @param[in,out] m_genFSR     The counters in FSR
   *  @param[in]     theCounter   The interaction counters
   *  @param[in]     option       Separate generated and accepted counters
   */
  void updateFSRCounters( interactionCounter& theCounter, LHCb::GenFSR* m_genFSR, const std::string option ) const;

};
#endif // GENERATORS_GENERATION_H
