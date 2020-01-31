#ifndef LBPYTHIA8_PYTHIA8PRODUCTION_H
#define LBPYTHIA8_PYTHIA8PRODUCTION_H 1

// LbPythia8.
#include "LbPythia8/BeamToolForPythia8.h"
#include "LbPythia8/LhcbHooks.h"

// Gaudi.
#include "GaudiAlg/GaudiTool.h"
#include "GenInterfaces/IProductionTool.h"
#include "GenInterfaces/ICounterLogFile.h"

// Pythia8.
#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/LHAFortran.h"

//#include "Pythia8Plugins/HepMC2.h"
#include <mutex>
#include "Utils/LocalTL.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

/** 
 * Production tool to generate events with Pythia 8.
 *
 * Production of events using the Pythia 8 multi-purpose Monte Carlo generator
 * is provided by this class. THe Pythia 8 generator is a complete rewrite in
 * C++ of the previous FORTRAN Pythia 6 event generator. Currently the physics
 * manual for Pythia 8 is the same as for Pythia 6, arXiv:hep-ph/0603175. A
 * brief introduction to the specifics of Pythia 8 can be found in
 * arXiv:0710.3820 while a continually updated online manual can be found at
 * http://home.thep.lu.se/~torbjorn/pythia81html/Welcome.html.
 *
 * @class  Pythia8Production
 * @file   Pythia8Production.h
 * @author Arthur de Gromard
 * @author Philip Ilten
 * @date   2007-06-28
 */
class Pythia8Production : public GaudiTool, virtual public IProductionTool {
public:
  typedef std::vector<std::string> CommandVector ;
  
  /// Default constructor.
  Pythia8Production(const std::string& type, const std::string& name,
		    const IInterface* parent);

  /// Default destructor.
  virtual ~Pythia8Production();

  /**
   * Initialize the tool.
   *
   * Intitializes the tool, but leaves the actual Pythia 8 initialization to
   * initializeGenerator(). Here the Gaudi tool, random number generator,
   * beam tool, user hooks (if not already supplied), and XML log file are
   * initialized.
   */
  StatusCode initialize() override;

  /// Initialize the Pythia 8 generator.
  StatusCode initializeGenerator() override;

  /// Finalize the tool.
  StatusCode finalize() override;

  /// Generate an event.
  virtual StatusCode generateEvent(HepMC3::GenEventPtr theEvent, 
				   LHCb::GenCollision* theCollision , HepRandomEnginePtr & engine ) const override;

  /**
   * Convert Pythia 8 event to HepMC format.
   *
   * This method converts the native output of Pythia 8 to the HepMC format
   * using the conversion method provided by Pythia 8. However the status codes
   * and vertex positions must be modified to match the LHCb standard. The
   * hard process information is also set.
   */
  StatusCode toHepMC(HepMC3::GenEventPtr theEvent, 
		     LHCb::GenCollision* theCollision) const;

  /// Set particle stable.
  void setStable(const LHCb::ParticleProperty* thePP) override;

  /// Update a particle.
  void updateParticleProperties(const LHCb::ParticleProperty* thePP) override;

  /// Sets Pythia 8's "HadronLevel:Hadronize" flag to true.
  void turnOnFragmentation() override;

  /// Sets Pythia 8's "HadronLevel:Hadronize" flag to false.
  void turnOffFragmentation() override;

  /// Hadronize an event.
  virtual StatusCode hadronize(HepMC3::GenEventPtr theEvent, 
			       LHCb::GenCollision* theCollision,
			       HepRandomEnginePtr & engine ) override;
  
  /// Save the Pythia 8 event record.
  virtual void savePartonEvent(HepMC3::GenEventPtr theEvent) override;

  /// Retrieve the Pythia 8 event record.
  virtual void retrievePartonEvent(HepMC3::GenEventPtr theEvent) override;

  /**
   * Print the running conditions.
   *
   * If the message level is at MSG::DEBUG or above and the ListAllParticles
   * property is set to true all particle data is printed. If the message level
   * is MSG::VERBOSE then all settings are listed, whereas if MSG::DEBUG then
   * only the changed settings are printed. Note that this method duplicates
   * the built in functionality of Pythia 8 and should be removed.
   */
  void printRunningConditions() const override;

  /**
   * Returns whether a particle has special status.
   *
   * If a particle has special status, then the particle cannot be modified.
   * This method checks if the particle is within the special particle set
   * built during construction of the class.
   */
  bool isSpecialParticle( const LHCb::ParticleProperty* thePP) const override;

  /**
   * Setup forced fragmentation.
   *
   * Used in conjuntion with the hadronize method. Here the Pythia 8 setting
   * of "PartonLevel:all" is set to off which stops both showers and
   * hadronization from being performed.
   */
  StatusCode setupForcedFragmentation(const int thePdgId) override;

  // The Pythia 8 members (needed externally).
  Pythia8::Pythia*    m_pythia; ///< The Pythia 8 generator.
  Pythia8::UserHooks* m_hooks;  ///< User hooks to veto events.
  Pythia8::LHAup*     m_lhaup;  ///< User specified hard process.
  mutable LocalTL<Pythia8::Event> m_event;  ///< The Pythia 8 event record.

  // Members needed externally.

  std::string m_beamToolName;        ///< The name of the beam tool.
  
protected:

  /**
   * Return the Pythia 8 ID.
   *
   * In most cases the Pythia 8 ID is equivalent to the PDG ID. However,
   * currently the LHCb PDG ID codes are out-dated so the following conversions
   * need to be made:
   * f_0(1370): 30221 -> 10221
   * Lambda_c(2625)+: 104124 -> 4124
   * Any particle that is not found within the Pythia 8 particle database is
   * assigned an ID of 0.
   */
  int pythia8Id(const LHCb::ParticleProperty* thePP) const;
  
  // Additional members.
  IBeamTool* m_beamTool;                 ///< The Gaudi beam tool.
  BeamToolForPythia8* m_pythiaBeamTool;  ///< The Pythia 8 beam tool.
  mutable std::atomic_int m_nEvents;     ///< Number of generated events.
  CommandVector m_userSettings;          ///< The user settings vector.
  std::string m_tuningFile;              ///< The global tuning file.
  std::string m_tuningUserFile;          ///< The user tuning file.
  bool m_validate_HEPEVT;                ///< Flag to validate the event.
  bool m_listAllParticles;               ///< Flag to list all the particles.
  bool m_checkParticleProperties ;       ///< Flag to check particle properties.
  bool m_showBanner;                     ///< Flag to print the Pythia 8 banner.
  ICounterLogFile* m_xmlLogTool;         ///< The XML log file. 
  std::set<unsigned int> m_special;           ///< The set of special particles.
  mutable std::mutex m_pythia_lock;
  std::set<int> m_bws;                   ///< Set of particles with a valid BW.
  /// Location where to store FSR counters (set by options)
  std::string  m_FSRName;
  class RndForPythia : public Pythia8::RndmEngine {
    public:
    RndForPythia(CLHEP::HepRandomEngine & engine ):m_gen(engine, 0, 1){}
    virtual double flat(){return m_gen();}

    private:
      CLHEP::RandFlat m_gen;
  };

};

#endif // LBPYTHIA8_PYTHIA8PRODUCTION_H
