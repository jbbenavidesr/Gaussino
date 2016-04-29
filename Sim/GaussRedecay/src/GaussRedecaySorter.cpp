// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"

// from GaussRedecay
#include "GaussRedecay/IGaussRedecayCtr.h"
#include "GaussRedecay/IGaussRedecayStr.h"

// From Kernel. To access signal particle properties
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// from Event
#include "Event/GenCollision.h"
#include "GenEvent/HepMCUtils.h"
#include "HepMC/GenRanges.h"

// local
#include "GaussRedecaySorter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussRedecaySorter
//
//
// 2016-03-15 : Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(GaussRedecaySorter)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussRedecaySorter::GaussRedecaySorter(const std::string& Name,
                                       ISvcLocator* SvcLoc)
    : GaudiAlgorithm(Name, SvcLoc) {
  declareProperty("GaussRedecay", m_gaussRDSvcName = "GaussRedecay");
  declareProperty("HepMCEventLocation",
                  m_generationLocation = LHCb::HepMCEventLocation::Default,
                  "Location to read the HepMC event.");
}

//=============================================================================
// Destructor
//=============================================================================
GaussRedecaySorter::~GaussRedecaySorter() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GaussRedecaySorter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  m_gaussRDCtrSvc = svc<IGaussRedecayCtr>(m_gaussRDSvcName, true);
  m_gaussRDStrSvc = svc<IGaussRedecayStr>(m_gaussRDSvcName, true);
  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaussRedecaySorter::execute() {
  m_theSignal = nullptr;
  if (nullptr == m_gaussRDCtrSvc) {
    return Error(" execute(): IGaussRedecayCtr* points to NULL");
  }

  if (nullptr == m_gaussRDStrSvc) {
    return Error(" execute(): IGaussRedecayStr* points to NULL");
  }

  if (nullptr == m_ppSvc) {
    return Error(" execute(): IParticlePropertySvc* points to NULL");
  }
  auto rd_mode = m_gaussRDCtrSvc->getRedecayMode();
  LHCb::HepMCEvents* generationEvents =
      get<LHCb::HepMCEvents>(m_generationLocation);
  if (!generationEvents) {
    return Error(
        " execute(): Could load HepMC event. That is ... eeehhh ... bad ...");
  }

  /*Get the signal, going to need this in any case*/
  m_theSignal = find_signal(generationEvents);
  switch (rd_mode) {
    case (0):
      /*Redecay only the signal particle*/
      if (msgLevel(MSG::DEBUG)) {
        debug() << "Registering only the signal for redecay." << endmsg;
      }
      store_particle(m_theSignal);
      break;
    case (1):
      /*Redecay everything that could potentially decay into the signal. Do not
       * mess with ProdGen stuff though.*/
      if (msgLevel(MSG::DEBUG)) {
        debug() << "Registering everything heavier than the signal for redecay"
                << endmsg;
      }
      store_heavier_than_signal(generationEvents);
      break;
    default:
      return Error(
          " execute(): Algorithm is being executed but no valid redecay mode "
          "was set. Check configuration!");
      /*Do nothing by default, algorithm should not run in this case, hence make
       * a pretty error*/
      break;
  }

  return StatusCode::SUCCESS;
}

HepMC::GenParticle* GaussRedecaySorter::find_signal(LHCb::HepMCEvents* evts) {
  HepMC::GenParticle* match = nullptr;
  /*Identify the signal particle as the SignalInLabFrame flagged particle going
   * into the signal vertex.*/
  for (auto& ev : *evts) {
    auto sv = ev->pGenEvt()->signal_process_vertex();
    if (sv) {
      for (auto part : sv->particles(HepMC::parents)) {
        if (part->status() == LHCb::HepMCEvent::SignalInLabFrame) {
          match = part;
        }
      }
    }
  }

  return match;
}

void GaussRedecaySorter::store_particle(HepMC::GenParticle* part) {
  IGaussRedecayStr::Particle temp_str_part;
  temp_str_part.momentum = Gaudi::LorentzVector(part->momentum());
  temp_str_part.point = Gaudi::XYZTPoint(part->production_vertex()->position());
  temp_str_part.pdg_id = part->pdg_id();

  /*Now store it, delete the daugthers and replace the pdg id with the
   * placeholder.*/
  auto new_id = m_gaussRDStrSvc->registerForRedecay(temp_str_part);
  HepMCUtils::RemoveDaughters(part);
  part->set_pdg_id(new_id);
}

void GaussRedecaySorter::store_heavier_than_signal(LHCb::HepMCEvents* evts) {
  if (!m_theSignal) {
    m_theSignal = find_signal(evts);
  }
  auto evt = m_theSignal->parent_event();
  auto sig_pdg_id = m_theSignal->pdg_id();
  auto PID = LHCb::ParticleID(sig_pdg_id);
  auto sig_info = m_ppSvc->find(PID);
  /*Get the invariant mass of the particle to decay everything that is heavier.
   * Multiplied by a factor just smaller than one to all prevent floating point
   * precision problems when checking particles identical to the signal
   * itself.*/
  double inv_mass = 0.9999 * sig_info->mass();
  std::set<HepMC::GenParticle*> heavy_stuff;

  /*Following code is taken from the decayHeavyParticles function in
   * ExternalGenerator.cpp using the default case of the switch statement to
   * decide what was further decayed to find the signal.*/
  for (auto part : evt->particle_range()) {
    auto status = part->status();
    /*Only pick up particles that make sense to redecay. Not constraining this
     * any further results in quarks in the list of particles to redecay
     * depending on the value of part->generated_mass().*/
    if (status != LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen &&
        status != LHCb::HepMCEvent::DecayedByDecayGen &&
        status != LHCb::HepMCEvent::SignalInLabFrame) {
      continue;
    }
    if (15 == PID.abspid()) {  // tau ?
      LHCb::ParticleID pid(part->pdg_id());
      if ((pid.hasQuark(LHCb::ParticleID::charm)) ||
          (pid.hasQuark(LHCb::ParticleID::bottom)))
        heavy_stuff.insert(part);
    } else {
      LHCb::ParticleID pid(part->pdg_id());
      if (part->generated_mass() > inv_mass) {
        heavy_stuff.insert(part);
        debug() << "This should be redecayed: " << endmsg;
        printChildren(part);
      }
      // if signal is KS then decay also K0
      else if ((m_theSignal->pdg_id() == 310) && (pid.abspid() == 311))
        heavy_stuff.insert(part);
    }
  }
  for (auto& part : heavy_stuff) {
    recursive_tagger(part);
  }
  std::set<HepMC::GenParticle*> selected_heavy_stuff;
  for (auto& part : heavy_stuff) {
    if(part->status() == 1042){
      selected_heavy_stuff.insert(part);
    }
  }
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Particles in redecay container after tagging: " << endmsg;
    for (auto& part : selected_heavy_stuff) {
      printChildren(part);
    }
  }
  for (auto& part : selected_heavy_stuff) {
    store_particle(part);
  }
  if (msgLevel(MSG::DEBUG)) {
    debug() << "After saving them in service." << endmsg;
    for (auto& part : selected_heavy_stuff) {
      printChildren(part);
    }
  }
}

void GaussRedecaySorter::recursive_tagger(HepMC::GenParticle* part) {
  if (part->status() != 1043) {
    part->set_status(1042);
  }
  auto ev = part->end_vertex();
  if (ev) {
    for (auto child : ev->particles(HepMC::children)) {
      child->set_status(1043);
      recursive_tagger(child);
    }
  }
}

void GaussRedecaySorter::printChildren(HepMC::GenParticle* part, int level) {
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
//=============================================================================
