// $Id: GaussRDSignalDecay.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
// Include files

// local
#include "GaussRDSignalDecay.h"
#include "GaussRD/IGaussRDStr.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Vector4DTypes.h"

// from Event
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"

// from LHCb
#include "Kernel/ParticleID.h"

// from Generators
#include "Generators/IPileUpTool.h"
#include "MCInterfaces/IDecayTool.h"
#include "MCInterfaces/IFullGenEventCutTool.h"
#include "MCInterfaces/IGenCutTool.h"
#include "Generators/GenCounters.h"
#include "GenEvent/HepMCUtils.h"

// Gaudi Common Flat Random Number generator
#include "Generators/RandomForGenerator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRDSignalDecay
//
// 2016-03-29 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY(GaussRDSignalDecay)
void GaussRDSignalDecay::printHepMCTree(HepMC::GenParticle* p, int level) {
  std::string spacer = "|---";
  std::string space = "";
  for (int i = 0; i < level; i++) {
    space += spacer;
  }
  debug() << space << " " << p->pdg_id() << " (PT, Eta) = ("
          << p->momentum().perp() << ", " << p->momentum().eta() << ")"
          << endmsg;
  auto ev = p->end_vertex();
  if (ev) {
    auto it = ev->particles_out_const_begin();
    auto itend = ev->particles_out_const_end();
    for (; it != itend; ++it) {
      printHepMCTree(*it, level + 1);
    }
  }
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRDSignalDecay::GaussRDSignalDecay(const std::string& name,
                                       ISvcLocator* pSvcLocator)
    : GaudiAlgorithm(name, pSvcLocator) {
  // Generation Method
  declareProperty("EventType", m_eventType = 50000000);
  declareProperty("HepMCEventLocation",
                  m_hepMCEventLocation = LHCb::HepMCEventLocation::Default);
  declareProperty("GenHeaderLocation",
                  m_genHeaderLocation = LHCb::GenHeaderLocation::Default);
  declareProperty("GenCollisionLocation",
                  m_genCollisionLocation = LHCb::GenCollisionLocation::Default);
  declareProperty("DecayTool", m_decayToolName = "EvtGenDecay");
  declareProperty("FullGenEventCutTool", m_fullGenEventCutToolName = "");
  declareProperty("GenCutTool", m_genCutToolName = "");
  declareProperty("GaussRD", m_gaussRDSvcName = "GaussRD");
}

//=============================================================================
// Destructor
//=============================================================================
GaussRDSignalDecay::~GaussRDSignalDecay() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GaussRDSignalDecay::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();  // Initialize base class
  if (sc.isFailure()) return sc;

  debug() << "==> Initialise" << endmsg;

  // Retrieve decay tool
  if ("" != m_decayToolName) {
    m_decayTool = tool<IDecayTool>(m_decayToolName);
  }

  // Retrieve full gen event cut tool
  if ("" != m_fullGenEventCutToolName)
    m_fullGenEventCutTool =
        tool<IFullGenEventCutTool>(m_fullGenEventCutToolName, this);

  // Retrieve gen cut tool
  if ("" != m_genCutToolName)
    m_genCutTool = tool<IGenCutTool>(m_genCutToolName, this);

  if (nullptr == m_gaussRDStrSvc) {
    m_gaussRDStrSvc = svc<IGaussRDStr>(m_gaussRDSvcName, true);
  }

  if (nullptr == m_gaussRDStrSvc) {
    return Error(" initialize(): IGaussRDStr* points to NULL");
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRDSignalDecay::execute() {
  debug() << "Processing event type " << m_eventType << endmsg;
  StatusCode sc = StatusCode::SUCCESS;

  // Get the header and update the information
  LHCb::GenHeader* theGenHeader = get<LHCb::GenHeader>(m_genHeaderLocation);
  if (!theGenHeader->evType()) {
    theGenHeader->setEvType(m_eventType);
  }

  LHCb::HepMCEvents::iterator itEvents;

  // Create temporary containers for this event
  LHCb::HepMCEvents* theEvents = new LHCb::HepMCEvents();
  LHCb::GenCollisions* theCollisions = new LHCb::GenCollisions();

  // Working set of pointers
  LHCb::GenCollision* theGenCollision(0);
  HepMC::GenEvent* theGenEvent(0);

  Gaudi::LorentzVector theFourMomentum = m_gaussRDStrSvc->getSignalMomentum();
  if (msgLevel(MSG::DEBUG)) {
    debug() << "With (PX, PY, PZ, E) = (" << theFourMomentum.px() << ", "
            << theFourMomentum.py() << ", " << theFourMomentum.pz() << ", "
            << theFourMomentum.e() << ")" << endmsg;
    debug() << "and (PT, eta) = (" << theFourMomentum.pt() << ", "
            << theFourMomentum.eta() << ")" << endmsg;
  }
  auto origin = m_gaussRDStrSvc->getSignalOrigin();
  int thePdgId = m_gaussRDStrSvc->getSignalID();
  m_sigPdgCode = thePdgId;
  if (m_decayTool && m_sigPdgCode != 0) m_decayTool->setSignal(m_sigPdgCode);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "With (X,Y,Z,T) = (" << origin.X() << ", " << origin.Y() << ", "
            << origin.Z() << ", " << origin.T() << ")" << endmsg;
  }

  // Generate sets of particles until a good one is found
  bool goodEvent = false;

  while (!goodEvent) {
    theEvents->clear();
    theCollisions->clear();

    // Prepare event container
    prepareInteraction(theEvents, theCollisions, theGenEvent, theGenCollision);

    // create HepMC Vertex
    HepMC::GenVertex* v = new HepMC::GenVertex(
        HepMC::FourVector(origin.X(), origin.Y(), origin.Z(), origin.T()));
    // create HepMC particle
    HepMC::GenParticle* p = new HepMC::GenParticle(
        HepMC::FourVector(theFourMomentum.Px(), theFourMomentum.Py(),
                          theFourMomentum.Pz(), theFourMomentum.E()),
        thePdgId, LHCb::HepMCEvent::StableInProdGen);

    v->add_particle_out(p);
    theGenEvent->add_vertex(v);
    theGenEvent->set_signal_process_id(1);
    theGenEvent->set_signal_process_vertex(v);

    goodEvent = true;
    // increase event and interactions counters
    ++m_nEvents;

    // Decay the event if it is a good event
    if (0 != m_decayTool) {
      unsigned short iPart(0);
      for (itEvents = theEvents->begin(); itEvents != theEvents->end();
           ++itEvents) {
        ParticleVector theParticleList;
        theParticleList.clear();

        HepMC::GenParticle* theSignal =
            decayEvent(*itEvents, theParticleList, sc);
        if (!sc.isSuccess()) return sc;
        if (msgLevel(MSG::DEBUG)) {
          debug() << "Made a nice, new and shiny signal decay:" << endmsg;
          printHepMCTree(theSignal);
        }

        (*itEvents)->pGenEvt()->set_event_number(++iPart);

        // Add Cut tool
        bool passCut(true);
        if (m_genCutTool && theSignal) {
          ++m_nBeforeCut;
          passCut = m_genCutTool->applyCut(theParticleList, theGenEvent,
                                           theGenCollision);
          // event does not pass cuts
          if (!passCut || theParticleList.empty()) {
            if (msgLevel(MSG::DEBUG)) {
              debug() << "Failed the cut." << endmsg;
            }
            HepMCUtils::RemoveDaughters(theSignal);
            goodEvent = false;
          } else
            ++m_nAfterCut;
        }
      }
    }

    // Apply generator level cut on full event
    if (m_fullGenEventCutTool) {
      if (goodEvent) {
        ++m_nBeforeFullEvent;
        goodEvent =
            m_fullGenEventCutTool->studyFullEvent(theEvents, theCollisions);
        if (goodEvent) ++m_nAfterFullEvent;
      }
    }
  }

  ++m_nAcceptedEvents;
  m_nAcceptedParticles += 1;

  // Now either create the info in the TES or add it to the existing one
  LHCb::HepMCEvents* eventsInTES =
      getOrCreate<LHCb::HepMCEvents, LHCb::HepMCEvents>(m_hepMCEventLocation);

  LHCb::GenCollisions* collisionsInTES =
      getOrCreate<LHCb::GenCollisions, LHCb::GenCollisions>(
          m_genCollisionLocation);

  // Check that number of temporary HepMCEvents is the same as GenCollisions
  if (theEvents->size() != theCollisions->size()) {
    return Error("Number of HepMCEvents and GenCollisions do not match");
  }

  itEvents = theEvents->begin();
  for (LHCb::GenCollisions::const_iterator it = theCollisions->begin();
       theCollisions->end() != it; ++it) {
    // HepMCEvent
    LHCb::HepMCEvent* theHepMCEvent = new LHCb::HepMCEvent();
    theHepMCEvent->setGeneratorName((*itEvents)->generatorName());
    (*theHepMCEvent->pGenEvt()) = (*(*itEvents)->pGenEvt());
    eventsInTES->insert(theHepMCEvent);
    ++itEvents;

    // GenCollision
    LHCb::GenCollision* theGenCollision = new LHCb::GenCollision();
    theGenCollision->setIsSignal((*it)->isSignal());
    theGenCollision->setProcessType((*it)->processType());
    theGenCollision->setSHat((*it)->sHat());
    theGenCollision->setTHat((*it)->tHat());
    theGenCollision->setUHat((*it)->uHat());
    theGenCollision->setPtHat((*it)->ptHat());
    theGenCollision->setX1Bjorken((*it)->x1Bjorken());
    theGenCollision->setX2Bjorken((*it)->x2Bjorken());
    theGenCollision->setEvent(theHepMCEvent);
    collisionsInTES->insert(theGenCollision);

    // GenHeader
    // theGenHeader->addToCollisions(theGenCollision);
  }

  // Clear and delete the temporary containers
  theEvents->clear();
  theCollisions->clear();
  delete (theEvents);
  delete (theCollisions);

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GaussRDSignalDecay::finalize() {
  using namespace GenCounters;
  debug() << "==> Finalize" << endmsg;
  // Print the various counters
  info() << "***********   Generation counters   **************" << std::endl;
  printCounter(info(), "generated events", m_nEvents);

  printCounter(info(), "accepted events", m_nAcceptedEvents);
  printCounter(info(), "particles in accepted events", m_nAcceptedParticles);

  printEfficiency(info(), "full event cut", m_nAfterFullEvent,
                  m_nBeforeFullEvent);
  printEfficiency(info(), "signal cut", m_nAfterCut, m_nBeforeCut);
  info() << endmsg;

  if (0 != m_decayTool) release(m_decayTool);
  if (0 != m_fullGenEventCutTool) release(m_fullGenEventCutTool);
  if (0 != m_genCutTool) release(m_genCutTool);

  return GaudiAlgorithm::finalize();  // Finalize base class
}

//=============================================================================
// Decay in the event all particles which have been left stable by the
// production generator
//=============================================================================
HepMC::GenParticle* GaussRDSignalDecay::decayEvent(
    LHCb::HepMCEvent* theEvent, ParticleVector& theParticleList,
    StatusCode& sc) {
  using namespace LHCb;
  m_decayTool->disableFlip();
  sc = StatusCode::SUCCESS;
  HepMC::GenParticle* theSignal(nullptr);

  HepMC::GenEvent* pEvt = theEvent->pGenEvt();

  // We must use particles_begin to obtain an ordered iterator of GenParticles
  // according to the barcode: this allows to reproduce events !
  HepMCUtils::ParticleSet pSet(pEvt->particles_begin(), pEvt->particles_end());

  HepMCUtils::ParticleSet::iterator itp;

  for (itp = pSet.begin(); itp != pSet.end(); ++itp) {
    HepMC::GenParticle* thePart = (*itp);
    unsigned int status = thePart->status();

    if ((HepMCEvent::StableInProdGen == status) ||
        ((HepMCEvent::DecayedByDecayGenAndProducedByProdGen == status) &&
         (0 == thePart->end_vertex()))) {
      if (m_decayTool->isKnownToDecayTool(thePart->pdg_id())) {
        if (HepMCEvent::StableInProdGen == status)
          thePart->set_status(
              HepMCEvent::DecayedByDecayGenAndProducedByProdGen);
        else
          thePart->set_status(HepMCEvent::DecayedByDecayGen);

        if (abs(m_sigPdgCode) == abs(thePart->pdg_id())) {
          bool hasFlipped(false);
          sc = m_decayTool->generateSignalDecay(thePart, hasFlipped);
          theSignal = thePart;
        } else
          sc = m_decayTool->generateDecay(thePart);

        theParticleList.push_back(thePart);

        if (!sc.isSuccess()) return 0;
      }
    }
  }
  return theSignal;
}

//=============================================================================
// Set up event
//=============================================================================
void GaussRDSignalDecay::prepareInteraction(
    LHCb::HepMCEvents* theEvents, LHCb::GenCollisions* theCollisions,
    HepMC::GenEvent*& theGenEvent, LHCb::GenCollision*& theGenCollision) const {
  LHCb::HepMCEvent* theHepMCEvent = new LHCb::HepMCEvent();
  theHepMCEvent->setGeneratorName("SignalDecay");
  theGenEvent = theHepMCEvent->pGenEvt();

  theGenCollision = new LHCb::GenCollision();
  theGenCollision->setEvent(theHepMCEvent);
  theGenCollision->setIsSignal((m_sigPdgCode != 0));

  theEvents->insert(theHepMCEvent);
  theCollisions->insert(theGenCollision);
}
