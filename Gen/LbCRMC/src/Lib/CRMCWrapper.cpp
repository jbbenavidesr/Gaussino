#include "LbCRMC/CRMCWrapper.h"

#include "HepMC/HeavyIon.h"
#include "HepMC/HEPEVT_Wrapper.h"
#include "HepMC/IO_GenEvent.h"
#include "HepMC/PdfInfo.h"
#include "HepMC/GenCrossSection.h"
#include "HepMC/Units.h"

#include "GaudiKernel/PhysicalConstants.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdexcept>

//-----------------------------------------------------------------------------
//  Implementation file for class: CRMCWrapper
//
//  2014-02-03 : Dmitry Popov
//-----------------------------------------------------------------------------


struct CRMCData {
  CRMCData() :
    fNParticles(0),
    fImpactParameter(0),
    sigtot(-1),
    sigine(-1),
    sigela(-1),
    sigdd(-1),
    sigsd(-1),
    sloela(-1),
    sigtotaa(-1),
    sigineaa(-1),
    sigelaaa(-1),
    npjevt(-1),
    ntgevt(-1),
    kolevt(-1),
    npnevt(-1),
    ntnevt(-1),
    nppevt(-1),
    ntpevt(-1),
    nglevt(-1),
    ng1evt(-1),
    ng2evt(-1),
    bimevt(-1),
    phievt(-1),
    fglevt(-1) {}
  void Clean() { fNParticles = 0; }

  // fortran output
  const static unsigned int fMaxParticles = 10000;

  int    fNParticles;
  double fImpactParameter;
  int    fPartId[fMaxParticles];
  double fPartPx[fMaxParticles];
  double fPartPy[fMaxParticles];
  double fPartPz[fMaxParticles];
  double fPartEnergy[fMaxParticles];
  double fPartMass[fMaxParticles];
  int    fPartStatus[fMaxParticles];

  double sigtot;
  double sigine;
  double sigela;
  double sigdd;
  double sigsd;
  double sloela;
  double sigtotaa;
  double sigineaa;
  double sigelaaa;
  int npjevt;
  int ntgevt;
  int kolevt;
  int kohevt;
  int npnevt;
  int ntnevt;
  int nppevt;
  int ntpevt;
  int nglevt;
  int ng1evt;
  int ng2evt;
  double bimevt;
  double phievt;
  double fglevt;
  int typevt;
};
extern CRMCData g_CRMC_data;

CRMCData g_CRMC_data;

extern "C" {
  extern struct {
    float sigtot;    // ........ h-p total cross section in mb
    float sigcut;    // ........ h-p cut cross section in mb : in principle it is the non-diffractive xs but the definition depends on the model
    float sigela;    // ........ h-p elastic cross section in mb
    float sloela;    // ........ h-p elastic slope
    float sigsd;     // ........ h-p single diffractive cross section in mb (both side)
    float sigine;    // ........ h-p inelastic cross section in mb (all inelastic processes=sigtot-sigela)
    float sigdif;    // ........ h-p diffractive cross section in mb (SD+DD+DPE) (in principle sigdif+sigcut=sigine but it depends how DPE xs is counted (in EPOS 1.99 it is counted as elastic because nothing was produced but in EPOS LHC DPE are produced)
    float sigineaa;  // ........ h-A or A-A cross section in mb (inelastic cross section to be used as CURRENT EVENT XS for defined projectile and target, previous h-p xs are really for h-p even if the projectile/target were defined as a nuclei)
    float sigtotaa;  // ........ h-A or A-A total cross section in mb
    float sigelaaa;  // ........ h-A or A-A elastic cross section in mb
    float sigcutaa;  // ........ h-A or A-A ND xs or production xs mb
    float sigdd;     // ........ h-p double diffractive cross section in mb (both side)
  } hadr5_;          // crmc-aaa.f
}

extern "C" {
  extern struct {
    float phievt; // ........ angle of impact parameter
    int   nevt;   // ........ error code. 1=valid event, 0=invalid event
    float bimevt; // ........ absolute value of impact parameter
    int   kolevt; // ........ number of collisions
    int   koievt; // ........ number of inelastic collisions
    float pmxevt; // ........ reference momentum
    float egyevt; // ........ pp cm energy (hadron) or string energy (lepton)
    int   npjevt; // ........ number of primary projectile participants
    int   ntgevt; // ........ number of primary target participants
    int   npnevt; // ........ number of primary projectile neutron spectators
    int   nppevt; // ........ number of primary projectile proton spectators
    int   ntnevt; // ........ number of primary target neutron spectators
    int   ntpevt; // ........ number of primary target proton spectators
    int   jpnevt; // ........ number of absolute projectile neutron spectators
    int   jppevt; // ........ number of absolute projectile proton spectators
    int   jtnevt; // ........ number of absolute target neutron spectators
    int   jtpevt; // ........ number of absolute target proton spectators
    float xbjevt; // ........ bjorken x for dis
    float qsqevt; // ........ q**2 for dis
    int   nglevt; // ........ number of collisions acc to  Glauber
    float zppevt; // ........ average Z-parton-proj
    float zptevt; // ........ average Z-parton-targ
    int   minfra; // ........
    int   maxfra; // ........
    int   kohevt; // ........ number of inelastic hard collisions
  } cevt_;        // epos.inc
}

extern "C" {
  extern struct {
    int   ng1evt; // ........ number of collisions acc to  Glauber
    int   ng2evt; // ........ number of Glauber participants with at least two IAs
    float rglevt; // ........
    float sglevt; // ........
    float eglevt; // ........
    float fglevt; // ........
    int   ikoevt; // ........ number of elementary parton-parton scatterings
    float typevt; // ........ type of event (1=Non Diff, 2=Double Diff, 3=Central Diff, 4=AB->XB, -4=AB->AX)
  } c2evt_;       // epos.inc
}

// CRMC random generator variables
extern "C" {
  extern struct {
    double seedi;
    double seedj;
    double seedj2;
    double seedc;
    int iseqini;
    int iseqsim;
  } cseed_;
}

// Constructor
CRMCWrapper::CRMCWrapper() :
  m_generator_ready(false),
  m_event_printouts(false),
  m_event_number(0),
  fLibrary(NULL),
  fNCollision(1),
  fSeed(12345678),
  fProjectileId(1),
  fTargetId(1),
  fHEModel(crmc_generators::EPOS_LHC),
  fTypout(0),
  fProjectileMomentum(4000),
  fTargetMomentum(-4000),
  fParamFileName("crmc.param"),
  fOutputFileName(""),
  fProduceTables(false),
  fSeedProvided(false),
  fTest(false),
  crmc_generate(NULL),
  crmc_set(NULL),
  crmc_init(NULL),
  crmc_xsection(NULL)
{
  // Instantiate an IO strategy for reading from HEPEVT
  hepevtio = new HepMC::IO_HEPEVT();
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
}

// Destructor
CRMCWrapper::~CRMCWrapper() {
  if (hepevtio)
    delete hepevtio;

  if (fLibrary) {
    dlclose(fLibrary);
    fLibrary = NULL;
  }
}

int CRMCWrapper::seed() {
  //return crmc_interface.fSeed;
  return fSeed;
}

void CRMCWrapper::set_seed(int new_seed) {
  //crmc_interface.fSeed = new_seed;
  fSeed = new_seed;
}

int CRMCWrapper::projectile_id() {
  //return crmc_interface.fProjectileId;
  return fProjectileId;
}

// Projectile's id, e.g.: 1=p, 12=C, 120=pi+, 208=Pb, PDG ID
void CRMCWrapper::set_projectile_id(int new_id) {
  //crmc_interface.fProjectileId = new_id;
  fProjectileId = new_id;
}

int CRMCWrapper::target_id() {
  //return crmc_interface.fTargetId;
  return fTargetId;
}

// Target's id, e.g.: 1=p, 12=C, 208=Pb, PDG nuc ID
void CRMCWrapper::set_target_id(int new_id) {
  //crmc_interface.fTargetId = new_id;
  fTargetId = new_id;
}

int CRMCWrapper::high_energy_model() {
  //return crmc_interface.fHEModel;
  return fHEModel;
}

int CRMCWrapper::type_output(){
  //return crmc_outputtype
  return fTypout;
}

void CRMCWrapper::set_high_energy_model(int new_model_id) {
  // Disable PYTHIA and Hijing
  if (new_model_id == crmc_generators::PYTHIA || new_model_id == crmc_generators::HIJING) {
    std::cerr << "-----------------------------------------------------------------------------" << std::endl;
    std::cerr << "LbCRMC : ERROR : CRMC's PYTHIA and Hijing are disabled not to interfere with LHCb's versions!" << std::endl;
    std::cerr << "-----------------------------------------------------------------------------" << std::endl;
    throw std::runtime_error("CRMC's versions of PYTHIA and Hijing generators are disabled by default in LHCb SW!");
  }

  // Check if the provided generator ID makes sense
  if (!((new_model_id >= crmc_generators::EPOS_LHC && new_model_id <= crmc_generators::PHOJET) || new_model_id == crmc_generators::DPMJET)) {
    std::cerr << "-----------------------------------------------------------------------------" << std::endl;
    std::cerr << "LbCRMC : ERROR : Unsupported HEModel ID specified!" << std::endl;
    std::cerr << "HEModel ID must be : "
              << crmc_generators::EPOS_LHC    << "(EPOS LHC), "
              << crmc_generators::EPOS_199    << "(EPOS 1.99), "
              << crmc_generators::QGSJET01    << "(QGSJet01), "
              << crmc_generators::GHEISHA     << "(Gheisha),"
              << crmc_generators::PYTHIA      << "(Pythia), " << std::endl;
    std::cerr << crmc_generators::HIJING      << "(Hijing), "
              << crmc_generators::SIBYLL      << "(Sibyll), "
              << crmc_generators::QGSJETII_04 << "(QGSJetII-04), "
              << crmc_generators::PHOJET      << "(Phojet), "
              << crmc_generators::DPMJET      << "(Dpmjet)!" << std::endl;
    std::cerr << "-----------------------------------------------------------------------------" << std::endl;
    throw std::runtime_error("LbCRMC : Erratic generator ID specified!");
  }
  //crmc_interface.fHEModel = new_model_id;
  fHEModel = new_model_id;
}

double CRMCWrapper::projectile_momentum() {
  //return crmc_interface.fProjectileMomentum;
  return fProjectileMomentum;
}

// Projectile's momentum/(GeV/c)
void CRMCWrapper::set_projectile_momentum(double new_p) {
  //crmc_interface.fProjectileMomentum = new_p;
  fProjectileMomentum = new_p;
}

double CRMCWrapper::targt_momentum() {
  //return crmc_interface.fTargetMomentum;
  return fTargetMomentum;
}

// Target's momentum/(GeV/c)
void CRMCWrapper::set_target_momentum(double new_p) {
  //crmc_interface.fTargetMomentum = new_p;
  fTargetMomentum = new_p;
}

bool CRMCWrapper::produce_tables() {
  //return crmc_interface.fTargetMomentum;
  return fProduceTables;
}

// Produce EPOS tables
void CRMCWrapper::set_produce_tables(bool new_bool) {
  //crmc_interface.fTargetMomentum = new_p;
  fProduceTables = new_bool;
}

std::string CRMCWrapper::parameters_file_name() {
  //return crmc_interface.fParamFileName;
  return fParamFileName;
}

void CRMCWrapper::set_parameters_file_name(std::string new_file_name) {
  //crmc_interface.fParamFileName = new_file_name;
  fParamFileName = new_file_name;
}

// Generator engine initialization
bool CRMCWrapper::initialize_generator() {
  if (!load_generator_library(fHEModel))
    return false;

  // Set parameters in CRMC
  /* crmc_set(fNCollision,
           fSeed,
           fProjectileMomentum,
           fTargetMomentum,
           fProjectileId,
           fTargetId,
           fHEModel,
           fProduceTables,
           fTypout,
           fOutputFileName.c_str(),
           fParamFileName.c_str());*/
   crmc_set(fNCollision,
           fSeed,
           fProjectileMomentum,
           fTargetMomentum,
           fProjectileId,
           fTargetId,
           fHEModel,
           fProduceTables,
           fTypout,
           fParamFileName.c_str());

  // Kick CRMC initialisation
  crmc_init();

  m_generator_ready = true;

  return true;
}

// Produce event
void CRMCWrapper::generate_event(HepMC::GenEvent *event) {
  if (!m_generator_ready)
    throw std::runtime_error("LbCRMC : The generator was not initialized properly!");

  // Clean the store, call CRMC
  g_CRMC_data.Clean();

  crmc_generate(fTypout,
                m_event_number,
                g_CRMC_data.fNParticles,
                g_CRMC_data.fImpactParameter,
                g_CRMC_data.fPartId[0],
                g_CRMC_data.fPartPx[0],
                g_CRMC_data.fPartPy[0],
                g_CRMC_data.fPartPz[0],
                g_CRMC_data.fPartEnergy[0],
                g_CRMC_data.fPartMass[0],
                g_CRMC_data.fPartStatus[0]);

  m_event_number++;

  g_CRMC_data.sigtot = double(hadr5_.sigtot);
  g_CRMC_data.sigine = double(hadr5_.sigine);
  g_CRMC_data.sigela = double(hadr5_.sigela);
  g_CRMC_data.sigdd = double(hadr5_.sigdd);
  g_CRMC_data.sigsd = double(hadr5_.sigsd);
  g_CRMC_data.sloela = double(hadr5_.sloela);
  g_CRMC_data.sigtotaa = double(hadr5_.sigtotaa);
  g_CRMC_data.sigineaa = double(hadr5_.sigineaa);
  g_CRMC_data.sigelaaa = double(hadr5_.sigelaaa);
  g_CRMC_data.npjevt = cevt_.npjevt;
  g_CRMC_data.ntgevt = cevt_.ntgevt;
  g_CRMC_data.kolevt = cevt_.kolevt;
  g_CRMC_data.kohevt = cevt_.kohevt;
  g_CRMC_data.npnevt = cevt_.npnevt;
  g_CRMC_data.ntnevt = cevt_.ntnevt;
  g_CRMC_data.nppevt = cevt_.nppevt;
  g_CRMC_data.ntpevt = cevt_.ntpevt;
  g_CRMC_data.nglevt = cevt_.nglevt;
  g_CRMC_data.ng1evt = c2evt_.ng1evt;
  g_CRMC_data.ng2evt = c2evt_.ng2evt;
  g_CRMC_data.bimevt = double(cevt_.bimevt);
  g_CRMC_data.phievt = double(cevt_.phievt);
  g_CRMC_data.fglevt = double(c2evt_.fglevt);
  g_CRMC_data.typevt = int(c2evt_.typevt);

  fill_event(event);

  if (m_event_printouts)
    print_event(event);

 
}

// Manipulations on the event
void CRMCWrapper::fill_event(HepMC::GenEvent *event) {
  if (!event)
    throw std::runtime_error("LbCRMC : Null pointer for the event passed to fill_event()!");

  // Here hepevt_ a COMMON fortran block gets read in
  const bool res = hepevtio->fill_next_event(event);
  if (!res)
    throw std::runtime_error("LbCRMC : HepEvtIO could not read next event!");

  // Set cross section information for this event
  HepMC::GenCrossSection cross_section;
  cross_section.set_cross_section(double(g_CRMC_data.sigineaa)*1e9); //required in pB
  event->set_cross_section(cross_section);
  
  /*   printf("sigtot=%f\n",g_CRMC_data.sigtot);
          printf("sigcut=%f\n",(double)hadr5_.sigcut);
          printf("sigela=%f\n",g_CRMC_data.sigela);
          printf("sloela=%f\n",g_CRMC_data.sloela);
          printf("sigsd=%f\n",g_CRMC_data.sigsd);
	  printf("sigine=%f\n",g_CRMC_data.sigine);
	  printf("sigdif=%f\n",(double)hadr5_.sigdif);
	  printf("sigineaa=%f\n",g_CRMC_data.sigineaa);
	  printf("sigtotaa=%f\n",g_CRMC_data.sigtotaa);
	  printf("sigelaaa=%f\n",g_CRMC_data.sigelaaa);
	  printf("sigcutaa=%f\n",(double)hadr5_.sigcutaa);
	  printf("sigdd=%f\n",g_CRMC_data.sigdd);

          printf("phievt=%f\n",cevt_.phievt);
	  printf("nevt=%i\n",cevt_.nevt);
	  printf("bimevt=%f\n",cevt_.bimevt);
	  printf("kolevt=%i\n",cevt_.kolevt);
          printf("koievt=%i\n",cevt_.koievt);
          printf("pmxevt=%f\n",cevt_.pmxevt);
	  printf("egyevt=%f\n",cevt_.egyevt);
          printf("npjevt=%i\n",cevt_.npjevt);
          printf("ntgevt=%i\n",cevt_.ntgevt);
          printf("npnevt=%i\n",cevt_.npnevt);
          printf("nppevt=%i\n",cevt_.nppevt);
          printf("ntnevt=%i\n",cevt_.ntnevt);
          printf("ntpevt=%i\n",cevt_.ntpevt);
	  printf("jpnevt=%i\n",cevt_.jpnevt);
          printf("jppevt=%i\n",cevt_.jppevt);
          printf("jtnevt=%i\n",cevt_.jtnevt);
          printf("jtpevt=%i\n",cevt_.jtpevt);
          printf("jpnevt=%i\n",cevt_.jpnevt);
          printf("xbjevt=%f\n",cevt_.xbjevt);
          printf("qsqevt=%f\n",cevt_.qsqevt);
          printf("nglevt=%i\n",cevt_.nglevt);
          printf("zppevt=%f\n",cevt_.zppevt);
          printf("zptevt=%f\n",cevt_.zptevt);
          printf("minfra=%i\n",cevt_.minfra);
          printf("maxfra=%i\n",cevt_.maxfra);
          printf("kohevt=%i\n",cevt_.kohevt);

          printf("ng1evt=%i\n",c2evt_.ng1evt);
          printf("ng2evt=%i\n",c2evt_.ng2evt);
          printf("rglevt=%f\n",c2evt_.rglevt);
          printf("sglevt=%f\n",c2evt_.sglevt);
          printf("eglevt=%f\n",c2evt_.eglevt);
          printf("fglevt=%f\n",c2evt_.fglevt);
          printf("ikoevt=%i\n",c2evt_.ikoevt);
          printf("typevt=%f\n",c2evt_.typevt);

          printf("fTypeOuput=%i\n",fTypout);*/
   

  // Provide optional pdf set id numbers for CMSSW to work flavour of partons and stuff... hope it's optional
  HepMC::PdfInfo pdf(0, 0, 0, 0, 0, 0, 0);
  event->set_pdf_info(pdf);




  // Setting heavy ion infromation
  //      int   Ncoll_hard          // Number of hard scatterings
  //      int   Npart_proj          // Number of projectile participants
  //      int   Npart_targ          // Number of target participants
  //      int   Ncoll               // Number of NN (nucleon-nucleon) collisions
  //      int   spectator_neutrons           // Number of spectator neutrons
  //      int   spectator_protons            // Number of spectator protons
  //      int   N_Nwounded_collisions        // Number of N-Nwounded collisions (here Glauber number of participants with at least 1 interaction)
  //      int   Nwounded_N_collisions        // Number of Nwounded-N collisons (here Glauber number of participants with at least 2 interaction2)
  //      int   Nwounded_Nwounded_collisions // Number of Nwounded-Nwounded collisions (here GLauber number of collisions)
  //      float impact_parameter        // Impact Parameter(fm) of collision
  //      float event_plane_angle       // Azimuthal angle of event plane
  //      float eccentricity            // eccentricity of participating nucleons
  //                                        in the transverse plane
  //                                        (as in phobos nucl-ex/0510031)
  //      float sigma_inel_NN           // nucleon-nucleon inelastic
  //                                        (including diffractive) cross-section
  HepMC::HeavyIon ion(g_CRMC_data.kohevt,
                      g_CRMC_data.npjevt,
                      g_CRMC_data.ntgevt,
                      g_CRMC_data.kolevt,
                      g_CRMC_data.npnevt + g_CRMC_data.ntnevt,
                      g_CRMC_data.nppevt + g_CRMC_data.ntpevt,
                      g_CRMC_data.ng1evt,  // -1
                      g_CRMC_data.ng2evt,  // -1
                      g_CRMC_data.nglevt,  // -1
                      g_CRMC_data.bimevt,
                      g_CRMC_data.phievt,
                      g_CRMC_data.fglevt,  // defined only if phimin=phimax=0. // -1
                      g_CRMC_data.sigine*1e9); //required in pB
  event->set_heavy_ion(ion);

  

  // Integer ID uniquely specifying the signal process (i.e. MSUB in Pythia)
  int sig_id = -1;
  // If negative typevt mini plasma was created by event (except -4)
  switch (g_CRMC_data.typevt) {
    case  0: break; //unknown for qgsjetII
    case  1: sig_id = 101;
             break;
    case -1: sig_id = 101;
             break;
    case  2: sig_id = 105;
             break;
    case -2: sig_id = 105;
             break;
    case  3: sig_id = 102;
             break;
    case -3: sig_id = 102;
             break;
    case  4: sig_id = 103;
             break;
    case -4: sig_id = 104;
             break;
    default: std::cerr << "LbCRMC : Signal ID not recognised for setting HEPEVT" << std::endl;
  }
  event->set_signal_process_id(sig_id);

  // Convert units
  if (event->is_valid())
	  convert_to_mev_and_mm(event);
}

// Convert event particles values from GeV to MeV
void CRMCWrapper::convert_to_mev_and_mm(HepMC::GenEvent *event) {
  if (!event)
    throw std::runtime_error("LbCRMC : Null pointer for the event passed to convert_to_mev_and_mm()!");

  // Scale factor for momentum
  double momentum_scale_factor = 1.;
  momentum_scale_factor = HepMC::Units::conversion_factor(HepMC::Units::GEV, event->momentum_unit());

  // Set the new 4-vector and generated mass with adjusted values
  if (momentum_scale_factor != 1.) {
    for (HepMC::GenEvent::particle_iterator particle = event->particles_begin(); particle != event->particles_end(); ++particle) {
      (*particle)->set_momentum(HepMC::FourVector((*particle)->momentum().px() * momentum_scale_factor,
                                                  (*particle)->momentum().py() * momentum_scale_factor,
                                                  (*particle)->momentum().pz() * momentum_scale_factor,
                                                  (*particle)->momentum().e() * momentum_scale_factor));
      (*particle)->set_generated_mass((*particle)->generated_mass() * momentum_scale_factor);
     }
   }

  // Scale factor for length
  double length_scale_factor = 1.;
  length_scale_factor  = HepMC::Units::conversion_factor(HepMC::Units::MM, event->length_unit());

  // Adjust length
  if (length_scale_factor != 1.) {
    for (HepMC::GenEvent::vertex_iterator vertex = event->vertices_begin(); vertex != event->vertices_end(); ++vertex) {
      (*vertex)->set_position(HepMC::FourVector((*vertex)->position().x(),
                                                (*vertex)->position().y(),
                                                (*vertex)->position().z(),
                                                (*vertex)->position().t() * Gaudi::Units::mm / Gaudi::Units::c_light));
    }
  }
}

// Reinitialize generator's random sequences for the next event
void CRMCWrapper::reinitialize_random_number_generator(double new_seed) {
  if (m_generator_ready && (fHEModel == crmc_generators::EPOS_LHC || fHEModel == crmc_generators::EPOS_199)) {
    // iqq = 1 - initialize sequence for initialization
    // iqq = 2 - initialize sequence for first event
    // int iqq = 1;
    // cseed_.seedj = new_seed;
    // crmc_epos_ranfini(cseed_.seedj, cseed_.iseqsim, iqq);

    int ranfiniArg1 = 1, ranfiniArg2 = 2;

    crmc_epos_ranfcv(new_seed);
    crmc_epos_ranfini(new_seed, ranfiniArg1, ranfiniArg2);
  }
}

// Print generator's random sequences variables for debugging
void CRMCWrapper::print_random_number_generator_variables() {
  if (m_generator_ready && (fHEModel == crmc_generators::EPOS_LHC || fHEModel == crmc_generators::EPOS_199)) {
    std::cout << "\n\nLbCRMC : cseed_ :: seedi \t" << cseed_.seedi << std::endl;
    std::cout << "LbCRMC : cseed_ :: seedj \t" << cseed_.seedj << std::endl; 
    std::cout << "LbCRMC : cseed_ :: seedj2 \t" << cseed_.seedj2 << std::endl;
    std::cout << "LbCRMC : cseed_ :: seedc \t" << cseed_.seedc << std::endl;
    std::cout << "LbCRMC : cseed_ :: iseqini \t" << cseed_.iseqini << std::endl;
    std::cout << "LbCRMC : cseed_ :: iseqsim \t" << cseed_.iseqsim << std::endl;
  }
}

// Load the appropriate generator library
bool CRMCWrapper::load_generator_library(int HEmodel) {
  std::ostringstream generator_dll_name;
  if (!fLibrary) {
    // rpath is being checked for non absolute paths
    generator_dll_name << "lib";

    switch (HEmodel) {
      case crmc_generators::EPOS_LHC : generator_dll_name << "Epos";
              break;
      case crmc_generators::EPOS_199: generator_dll_name << "Epos";
              break;
      case crmc_generators::QGSJET01: generator_dll_name << "Qgsjet01";
              break;
      case crmc_generators::GHEISHA: generator_dll_name << "Gheisha";
              break;
      case crmc_generators::PYTHIA: generator_dll_name << "Pythia";
              break;
      case crmc_generators::HIJING: generator_dll_name << "Hijing";
              break;
      case crmc_generators::SIBYLL: generator_dll_name << "Sibyll";
              break;
      case crmc_generators::QGSJETII_04: generator_dll_name << "QgsjetII04";
              break;
      case crmc_generators::PHOJET: generator_dll_name << "Phojet";
              break;
      case crmc_generators::DPMJET: generator_dll_name << "Dpmjet";
               break;
      default: generator_dll_name << "UnknownModel";
    }

    generator_dll_name << ".so";
    fLibrary = dlopen(generator_dll_name.str().c_str(), RTLD_NOW);
    std::cout << "LbCRMC : opening library: " << generator_dll_name.str() << std::endl;

    if (!fLibrary) {
      std::ostringstream errMsg;
      errMsg << "\nLbCRMC : cannot open shared library " << generator_dll_name.str() << "\'\n\n"
             << "Dynamic-link error:\n \"" << dlerror() << "\"\n";
      std::cerr << errMsg.str() << std::endl;
      throw std::runtime_error("LbCRMC : Cannot open generator's shared library!");
    }
  }

  // Generate new event method
  crmc_generate = (void(*)(const int&, const int&, int&, double&, int&, double&,
                           double&, double&, double&, double&, int&))
                           dlsym(fLibrary, "crmc_f_");

  if(crmc_generate == NULL) {
    std::ostringstream errMsg;
    errMsg << "LbCRMC : dlsym error:\n \"" << dlerror() << "\"\n";
    std::cerr << errMsg.str() << std::endl;
    throw std::runtime_error("LbCRMC : error while trying to generate a new event : crmc_f_!");
  }

  // Configuration method
  /*  crmc_set = (void(*)(const int&, const int&, const double&, const double&,
                      const int&, const int&, const int&, const int&,
                      const int&, const char*, const char*))
                      dlsym(fLibrary, "crmc_set_f_");*/

  crmc_set = (void(*)(const int&, const int&, const double&, const double&,
                      const int&, const int&, const int&, const int&,
                      const int&, const char*))
                      dlsym(fLibrary, "crmc_set_f_");

  if(crmc_set == NULL) {
    std::ostringstream errMsg;
    errMsg << "LbCRMC : dlsym error:\n \"" << dlerror() << "\"\n";
    std::cerr << errMsg.str() << std::endl;
    throw std::runtime_error("LbCRMC : error while trying to configure the generator : crmc_set_f_!");
  }

  // Initialization method
  crmc_init = (void(*)()) dlsym(fLibrary, "crmc_init_f_");

  if(crmc_init == NULL) {
    std::ostringstream errMsg;
    errMsg << "LbCRMC : dlsym error:\n \"" << dlerror() << "\"\n";
    std::cerr << errMsg.str() << std::endl;
    throw std::runtime_error("LbCRMC : error during initialization : crmc_init_f_!");
  }

  // Crossections?
  crmc_xsection = (void(*)(double&, double&, double&, double&, double&,
                           double&, double&, double&, double&))
                           dlsym(fLibrary, "crmc_xsection_f_");

  if(crmc_xsection == NULL) {
    std::ostringstream errMsg;
    errMsg << "LbCRMC : dlsym error:\n \"" << dlerror() << "\"\n";
    std::cerr << errMsg.str() << std::endl;
    throw std::runtime_error("LbCRMC : dlsym error : crmc_xsection_f_!");
  }

  // EPOS random engine related functions
  crmc_epos_ranfini = (void(*)(double&, int&, int&)) dlsym(fLibrary, "ranfini_");
  if(crmc_epos_ranfini == NULL) {
    std::ostringstream errMsg;
    errMsg << "LbCRMC : dlsym error:\n \"" << dlerror() << "\"\n";
    std::cerr << errMsg.str() << std::endl;
    throw std::runtime_error("LbCRMC : error during initialization : ranfini_!");
  }

  crmc_epos_ranfcv = (void(*)(double&)) dlsym(fLibrary, "ranfcv_");
  if(crmc_epos_ranfcv == NULL) {
    std::ostringstream errMsg;
    errMsg << "LbCRMC : dlsym error:\n \"" << dlerror() << "\"\n";
    std::cerr << errMsg.str() << std::endl;
    throw std::runtime_error("LbCRMC : error during initialization : ranfcv_!");
  }

  return true;
}

// Printout event information
void CRMCWrapper::print_event(HepMC::GenEvent *event) {
  if (!event)
    throw std::runtime_error("LbCRMC : Null pointer for the event passed to print_event()!");

  if (event->is_valid())
		event->print();
}
// EOF
