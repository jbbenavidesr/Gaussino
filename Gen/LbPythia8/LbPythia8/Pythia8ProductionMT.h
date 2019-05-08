#pragma once

// LbPythia8.
#include "LbPythia8/BeamToolForPythia8.h"
#include "LbPythia8/LhcbHooks.h"
#include "Utils/LocalTL.h"

// Gaudi.
#include "GaudiAlg/GaudiTool.h"
#include "GenInterfaces/ICounterLogFile.h"
#include "GenInterfaces/IProductionTool.h"

// Pythia8.
#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/LHAFortran.h"

//#include "Pythia8Plugins/HepMC2.h"
#include <condition_variable>
#include <mutex>

using namespace std;

/**
 * Production tool to generate events with Pythia 8.
 *
 *  Modified Pythia8Production tool which instantiates multiple instances
 *  of pythia, one per thread.
 *  For now, this is just a copy and paste of Pythia8Production and slightly refactored.
 *
 * @class  Pythia8ProductionMT
 * @file   Pythia8ProductionMT.h
 * @author Dominik Muller
 * @date   5.7.2018
 */
class Pythia8ProductionMT : public GaudiTool, virtual public IProductionTool
{
public:
  typedef vector<string> CommandVector;

  /// Default constructor.
  Pythia8ProductionMT( const string& type, const string& name, const IInterface* parent );

  /// Default destructor.
  virtual ~Pythia8ProductionMT();

  /**
   * Initialize the tool.
   *
   * Intitializes the tool, but leaves the actual Pythia 8 initialization to
   * initializeGenerator(). Here the Gaudi tool, random number generator,
   * beam tool, user hooks (if not already supplied), and XML log file are
   * initialized.
   */
  virtual StatusCode initialize();

  /// Initialize the Pythia 8 generator.
  virtual StatusCode initializeGenerator();

  /// Finalize the tool.
  virtual StatusCode finalize();

  /// Generate an event.
  virtual StatusCode generateEvent( HepMC3::GenEvent* theEvent, LHCb::GenCollision* theCollision,
                                    HepRandomEnginePtr& engine );

  /**
   * Convert Pythia 8 event to HepMC format.
   *
   * This method converts the native output of Pythia 8 to the HepMC format
   * using the conversion method provided by Pythia 8. However the status codes
   * and vertex positions must be modified to match the LHCb standard. The
   * hard process information is also set.
   */
  StatusCode toHepMC( HepMC3::GenEvent* theEvent, LHCb::GenCollision* theCollision );

  /// Set particle stable.
  virtual void setStable( const LHCb::ParticleProperty* thePP );
  virtual void setStableImpl( const LHCb::ParticleProperty* thePP );

  /// Update a particle.
  virtual void updateParticleProperties( const LHCb::ParticleProperty* thePP );
  virtual void updateParticlePropertiesImpl( const LHCb::ParticleProperty* thePP );

  /// Sets Pythia 8's "HadronLevel:Hadronize" flag to true.
  virtual void turnOnFragmentation();

  /// Sets Pythia 8's "HadronLevel:Hadronize" flag to false.
  virtual void turnOffFragmentation();

  /// Hadronize an event.
  virtual StatusCode hadronize( HepMC3::GenEvent* theEvent, LHCb::GenCollision* theCollision );

  /// Save the Pythia 8 event record.
  virtual void savePartonEvent( HepMC3::GenEvent* theEvent );

  /// Retrieve the Pythia 8 event record.
  virtual void retrievePartonEvent( HepMC3::GenEvent* theEvent );

  /**
   * Print the running conditions.
   *
   * If the message level is at MSG::DEBUG or above and the ListAllParticles
   * property is set to true all particle data is printed. If the message level
   * is MSG::VERBOSE then all settings are listed, whereas if MSG::DEBUG then
   * only the changed settings are printed. Note that this method duplicates
   * the built in functionality of Pythia 8 and should be removed.
   */
  virtual void printRunningConditions();

  /**
   * Returns whether a particle has special status.
   *
   * If a particle has special status, then the particle cannot be modified.
   * This method checks if the particle is within the special particle set
   * built during construction of the class.
   */
  virtual bool isSpecialParticle( const LHCb::ParticleProperty* thePP ) const;

  /**
   * Setup forced fragmentation.
   *
   * Used in conjuntion with the hadronize method. Here the Pythia 8 setting
   * of "PartonLevel:all" is set to off which stops both showers and
   * hadronization from being performed.
   */
  virtual StatusCode setupForcedFragmentation( const int thePdgId );

  // The Pythia 8 members. Just to be sure will have all of them
  // thread local. All these pointers are initialised to zero in
  // the threadlocal storage.
  LocalTL<Pythia8::Pythia*> m_pythia;   ///< The Pythia 8 generator.
  LocalTL<Pythia8::UserHooks*> m_hooks; ///< User hooks to veto events.
  LocalTL<Pythia8::LHAup*> m_lhaup;     ///< User specified hard process.
  LocalTL<Pythia8::Event> m_event;      ///< The Pythia 8 event record.

  std::vector<const LHCb::ParticleProperty*> m_update_pp;
  std::vector<const LHCb::ParticleProperty*> m_stable_pp;

  // Members needed externally.
  string m_beamToolName; ///< The name of the beam tool.

  virtual StatusCode InitializeThread();

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
  int pythia8Id( const LHCb::ParticleProperty* thePP );

  // Additional members.
  IBeamTool* m_beamTool;                         ///< The Gaudi beam tool.
  LocalTL<BeamToolForPythia8*> m_pythiaBeamTool; ///< The Pythia 8 beam tool.
  int m_nEvents;                                 ///< Number of generated events.
  CommandVector m_userSettings;                  ///< The user settings vector.
  string m_tuningFile;                           ///< The global tuning file.
  string m_tuningUserFile;                       ///< The user tuning file.
  bool m_validate_HEPEVT;                        ///< Flag to validate the event.
  bool m_listAllParticles;                       ///< Flag to list all the particles.
  bool m_checkParticleProperties;                ///< Flag to check particle properties.
  bool m_showBanner;                             ///< Flag to print the Pythia 8 banner.
  ICounterLogFile* m_xmlLogTool;                 ///< The XML log file.
  set<unsigned int> m_special;                   ///< The set of special particles.
  static std::mutex m_pythia_lock;
  std::atomic_bool m_first_init{true};

  class Pythia8ThreadManager
  {

  public:
    ~Pythia8ThreadManager()
    {
      for ( auto[pythia, hooks, lhaup, beam] : store ) {
        if ( pythia ) delete pythia;
        if ( hooks ) delete hooks;
        if ( lhaup ) delete lhaup;
        if ( beam ) delete beam;
      }
    }
    std::vector<std::tuple<Pythia8::Pythia*, Pythia8::UserHooks*, Pythia8::LHAup*, BeamToolForPythia8*>> store;
  };
  Pythia8ThreadManager* m_manager{nullptr};

private:
  unsigned int m_nThreads{0};
  std::once_flag m_init_flag;
  class P8MTBarrier
  {
  private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::size_t m_n_waiting;

  public:
    explicit P8MTBarrier( std::size_t count ) : m_n_waiting( count ) {}
    void wait()
    {
      std::unique_lock<std::mutex> lock{_mutex};
      if ( --m_n_waiting == 0 ) {
        _cv.notify_all();
      } else {
        _cv.wait( lock, [this] { return m_n_waiting == 0; } );
      }
    }
    P8MTBarrier()                     = delete;
    P8MTBarrier( const P8MTBarrier& ) = delete;
    P8MTBarrier( P8MTBarrier&& )      = delete;
  };
  static P8MTBarrier& GetInitBarrier( std::size_t num_threads = 0 )
  {
    static P8MTBarrier barrier( num_threads );
    return barrier;
  }
  std::atomic_uint m_p8_init{0};
};
