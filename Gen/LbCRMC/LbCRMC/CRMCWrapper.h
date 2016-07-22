#ifndef LBCRMC_CRMCWRAPPER_H
#define LBCRMC_CRMCWRAPPER_H 1

#include "HepMC/GenEvent.h"
#include "HepMC/IO_HEPEVT.h"


//-----------------------------------------------------------------------------
//  Interface file for class: CRMCWrapper
//
//  2014-02-03 : Dmitry Popov
//-----------------------------------------------------------------------------


// List of CRMC's generators libraries
namespace crmc_generators {
  enum GenList {
    EPOS_LHC = 0,
    EPOS_199 = 1,
    QGSJET01 = 2,
    GHEISHA = 3,
    PYTHIA = 4,
    HIJING = 5,
    SIBYLL = 6,
    QGSJETII_04 = 7,
    PHOJET = 8,
    DPMJET = 12,
  };
}

class CRMCWrapper {
  public:
    // Constructor and Destructor
    CRMCWrapper();
    ~CRMCWrapper();

    // Initialize the generator engine
    bool initialize_generator();

    // Run the generator to simulate a single event
    void generate_event(HepMC::GenEvent *theEvent);

    // Manipulations on the generated event
    void fill_event(HepMC::GenEvent *theEvent);

    // Print information about every generated event
    void set_event_printouts(bool print_or_not) { m_event_printouts = print_or_not; }
    bool event_printouts() { return m_event_printouts; }

    // Initial seed to initialize random number generator in CRMC
    int seed();
    void set_seed(int new_seed);

    // Projectile's id, e.g: 1=p, 12=C, 120=pi+, 208=Pb, PDG ID
    int projectile_id();
    void set_projectile_id(int new_id);

    // Target's id, e.g.: 1=p, 12=C, 208=Pb, PDG nuc ID
    int target_id();
    void set_target_id(int new_id);

    // CRMC's generator id
    int high_energy_model();
    void set_high_energy_model(int new_model_id);

    //CRMC type of output
    int type_output();

    // Projectile's momentum/(GeV/c)
    double projectile_momentum();
    void set_projectile_momentum(double new_p);

    // Target's momentum/(GeV/c)
    double targt_momentum();
    void set_target_momentum(double new_p); 

    //Produce Epos Tables
    bool produce_tables();
    void set_produce_tables(bool new_bool); 

    // CRMC parameters file
    std::string parameters_file_name();
    void set_parameters_file_name(std::string new_file_name);

    //Apply a trick to force energy conservation for all the particle
    void correct_particle_energy(HepMC::GenEvent *theEvent);

    // Convert event particles values from GeV to MeV
    void convert_to_mev_and_mm(HepMC::GenEvent *theEvent);

    // Reinitialize generator's random sequences for the next event
    void reinitialize_random_number_generator(double new_seed);

    // Print generator's random sequences variables for debugging
    void print_random_number_generator_variables();

  private:
    bool m_generator_ready;
    bool m_event_printouts;
    int m_event_number;

    // CRMC's generator library]
    void* fLibrary;

    // CRMC generator parameters
    int fNCollision;
    int fSeed;
    int fProjectileId;
    int fTargetId;
    int fHEModel;
    int fTypout;

    double fProjectileMomentum;
    double fTargetMomentum;
   

    std::string fParamFileName;
    std::string fOutputFileName;

    bool fProduceTables;
    bool fSeedProvided;
    bool fTest;
   

    // HepMC IO
    HepMC::IO_HEPEVT *hepevtio;

    // Load CRMC's generator shared library
    bool load_generator_library(int HEmodel);

    // CRMC's functions
    void (*crmc_generate)(const int&, const int&, int&, double&, int&, double&,
                          double&, double&,double&, double&, int&);

    /*void (*crmc_set)(const int&, const int&, const double&, const double&,
                     const int&, const int&, const int&, const int&, const int&,
                     const char*, const char*);*/
    void (*crmc_set)(const int&, const int&, const double&, const double&,
                     const int&, const int&, const int&, const int&, const int&,
                     const char*);


    void (*crmc_init)();

    void (*crmc_xsection)(double&, double&, double&, double&, double&, double&, double&, double&, double&);

    void (*crmc_epos_ranfini)(double&, int&, int&);

    void (*crmc_epos_ranfcv)(double&);

    // Debugging method - prints an event
    void print_event(HepMC::GenEvent *event);
};

#endif
// EOF
