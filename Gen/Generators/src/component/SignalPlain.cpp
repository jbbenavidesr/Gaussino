// $Id: SignalPlain.cpp,v 1.16 2007-02-22 13:30:24 robbep Exp $
// Include files

// local
#include "SignalPlain.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/GenCollision.h"
#include "Event/HepMCEvent.h"
#include "HepMC/GenRanges.h"

// Kernel
#include "LoKi/PrintHepMCDecay.h"
#include "MCInterfaces/IDecayTool.h"
#include "MCInterfaces/IGenCutTool.h"

// from Generators
#include "GenEvent/HepMCUtils.h"
#include "Generators/IProductionTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalPlain
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY(SignalPlain)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SignalPlain::SignalPlain(const std::string& type, const std::string& name,
                         const IInterface* parent)
    : Signal(type, name, parent) {
  ;
}

//=============================================================================
// Destructor
//=============================================================================
SignalPlain::~SignalPlain() { ; }

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool SignalPlain::generate(const unsigned int nPileUp,
                           LHCb::HepMCEvents* theEvents,
                           LHCb::GenCollisions* theCollisions) {
  StatusCode sc;
  bool result = false;
  // Memorize if the particle is inverted
  bool isInverted = false;
  bool hasFlipped = false;
  bool hasFailed = false;
  LHCb::GenCollision* theGenCollision(0);
  HepMC::GenEvent* theGenEvent(0);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "#########################################" << endmsg;
    debug() << "Generating a new event!" << endmsg;
    debug() << "#########################################" << endmsg;
  }

  for (unsigned int i = 0; i < nPileUp; ++i) {
    debug() << "=========================================" << endmsg;
    debug() << "Redecay Pileup: " << i << endmsg;
    debug() << "=========================================" << endmsg;
    prepareInteraction(theEvents, theCollisions, theGenEvent, theGenCollision);

    sc = m_productionTool->generateEvent(theGenEvent, theGenCollision);
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Event from beam end_vertex before decay" << endmsg;
      debug() << "-----------------------------------------" << endmsg;
      printChildren(theGenEvent->beam_particles().first);
      debug() << "-----------------------------------------" << endmsg;
    }
    if (sc.isFailure()) Exception("Could not generate event");

    if (!result) {
      // Decay particles heavier than the particles to look at
      decayHeavyParticles(theGenEvent, m_signalQuark, m_signalPID);
      if (msgLevel(MSG::DEBUG)) {
        debug() << "All vertices with no parents after decay" << endmsg;
        debug() << "Event from beam end_vertex after decay" << endmsg;
        debug() << "-----------------------------------------" << endmsg;
        printChildren(theGenEvent->beam_particles().first);
        debug() << "-----------------------------------------" << endmsg;
      }

      // Check if one particle of the requested list is present in event
      ParticleVector theParticleList;
      if (checkPresence(m_pids, theGenEvent, theParticleList)) {
        debug() << "Signals found: " << endmsg;
        for (auto& p : theParticleList) {
          debug() << p->pdg_id() << " with ";
          auto ev = p->end_vertex();
          if (ev) {
            debug() << ev->particles_out_size() << " children.";
          }
          debug() << endmsg;
        }

        // establish correct multiplicity of signal
        if (ensureMultiplicity(theParticleList.size())) {
          debug() << "Passed multiplicity check." << endmsg;

          // choose randomly one particle and force the decay
          hasFlipped = false;
          isInverted = false;
          hasFailed = false;
          HepMC::GenParticle* theSignal = chooseAndRevert(
              theParticleList, isInverted, hasFlipped, hasFailed);
          if (hasFailed) {
            HepMCUtils::RemoveDaughters(theSignal);
            Error("Skip event");
            return false;
          }
          debug() << "Begin after chooseAndRevert." << endmsg;
          for (auto& p : theParticleList) {
            debug() << p->pdg_id() << " with ";
            auto ev = p->end_vertex();
            if (ev) {
              debug() << ev->particles_out_size() << " children.";
            }
            debug() << endmsg;
          }
          debug() << "End after chooseAndRevert." << endmsg;

          theParticleList.clear();
          theParticleList.push_back(theSignal);

          if (!hasFlipped) {
            m_nEventsBeforeCut++;
            // count particles in 4pi
            updateCounters(theParticleList, m_nParticlesBeforeCut,
                           m_nAntiParticlesBeforeCut, false, false);

            bool passCut = true;
            if (0 != m_cutTool)
              passCut = m_cutTool->applyCut(theParticleList, theGenEvent,
                                            theGenCollision);

            if (passCut && (!theParticleList.empty())) {
              if (!isInverted) m_nEventsAfterCut++;

              if (isInverted) ++m_nInvertedEvents;

              // Count particles passing the generator level cut with pz > 0
              updateCounters(theParticleList, m_nParticlesAfterCut,
                             m_nAntiParticlesAfterCut, true, isInverted);

              if (m_cleanEvents) {
                sc = isolateSignal(theSignal);
                if (!sc.isSuccess()) Exception("Cannot isolate signal");
              }
              theGenEvent->set_signal_process_vertex(theSignal->end_vertex());

              theGenCollision->setIsSignal(true);

              // Count signal B and signal Bbar
              if (theSignal->pdg_id() > 0)
                ++m_nSig;
              else
                ++m_nSigBar;

              // Update counters
              GenCounters::updateHadronCounters(
                  theGenEvent, m_bHadC, m_antibHadC, m_cHadC, m_anticHadC,
                  m_bbCounter, m_ccCounter);
              GenCounters::updateExcitedStatesCounters(theGenEvent, m_bExcitedC,
                                                       m_cExcitedC);

              result = true;
            } else {
              // event does not pass cuts
              HepMCUtils::RemoveDaughters(theSignal);
            }
          } else {
            // has flipped:
            HepMCUtils::RemoveDaughters(theSignal);
            theSignal->set_pdg_id(-(theSignal->pdg_id()));
          }
        }
      }
    }
  }

  return result;
}

void SignalPlain::printChildren(HepMC::GenParticle* part, int level) {
  std::map<int, std::string> id_to_name;
  id_to_name[0] = "Unknown";
  id_to_name[1] = "StableInProdGen";
  id_to_name[2] = "DecayedByProdGen";
  id_to_name[3] = "DocumentationParticle";
  id_to_name[777] = "DecayedByDecayGen";
  id_to_name[888] = "DecayedByDecayGenAndProducedByProdGen";
  id_to_name[889] = "SignalInLabFrame";
  id_to_name[998] = "SignalAtRest";
  id_to_name[999] = "StableInDecayGen";
  id_to_name[1042] = "Redecay";
  id_to_name[1043] = "ChildOfRedecay";
  std::string space = "";
  for (int i = 0; i < level; i++) {
    space += "|---> ";
  }
  std::string idname = "";
  if (id_to_name.find(part->status()) != id_to_name.end()) {
    idname = id_to_name[part->status()];
  }
  debug() << space << part->pdg_id() << " -> " << part->status() << ": "
          << idname << endmsg;
  if (part->end_vertex()) {
    for (auto p : part->particles_out(HepMC::children)) {
      printChildren(p, level + 1);
    }
  }
}
