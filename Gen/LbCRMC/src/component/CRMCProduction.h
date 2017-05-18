#ifndef LBCRMC_CRMCPRODUCTION_H
#define LBCRMC_CRMCPRODUCTION_H 1

#include "GaudiAlg/GaudiTool.h"

#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IProductionTool.h"

// forward declarations
class CRMCWrapper ;
class IBeamTool ;


//-----------------------------------------------------------------------------
//  Interface file for class: CRMCProduction
//
//  2014-02-03 : Dmitry Popov
// 2015-08-25 :modification to go from EPOS Fortan to HepMC C++ (Laure Massacrier)
//-----------------------------------------------------------------------------


class CRMCProduction : public GaudiTool, virtual public IProductionTool {

public:
  typedef std::vector<std::string> CommandVector;

  // Default constructor
  CRMCProduction(const std::string &type, const std::string &name, const IInterface *parent);

  // Default destructor
  virtual ~CRMCProduction();

  // Tool initialization
  virtual StatusCode initialize();

  // Tool finilization
  virtual StatusCode finalize();

  // Initialize the generator
  virtual StatusCode initializeGenerator();

  // Generate an event
  virtual StatusCode generateEvent(HepMC::GenEvent *theEvent, LHCb::GenCollision *theCollision);

  // This method is not implemented
  virtual void setStable(const LHCb::ParticleProperty *thePP);

  // This method is not implemented
  virtual void updateParticleProperties(const LHCb::ParticleProperty *thePP);

  // This method is not implemented
  virtual bool isSpecialParticle(const LHCb::ParticleProperty *thePP) const;

  // This method is not implemented
  virtual void turnOnFragmentation();

  // This method is not implemented
  virtual void turnOffFragmentation();

  // This method is not implemented
  virtual StatusCode setupForcedFragmentation(const int thePdgId);

  // This method is not implemented
  virtual StatusCode hadronize(HepMC::GenEvent *theEvent, LHCb::GenCollision *theCollision);

  // This method is not implemented
  virtual void savePartonEvent(HepMC::GenEvent *theEvent);

  // This method is not implemented
  virtual void retrievePartonEvent(HepMC::GenEvent *theEvent);

  // This method is not implemented
  virtual void printRunningConditions();

  // Create temporary CRMC parameters file with default options
  StatusCode writeTempGeneratorParamFiles(CommandVector &options);

  // Printout out how the generator was configured to run
  void printOutGeneratorConfiguration();
  
private:
  CommandVector m_defaultSettings;  // Default settings
  CommandVector m_userSettings;     // User settings
  
  bool m_printEvent;                // Flag to print events on screen
  int m_nEvents;                    // Generated events counter
  std::string m_tempParamFileName;  // Temporary file for CRMC parameters
  std::string m_tempParamFileName_backup;
  
  CRMCWrapper *m_CRMCEngine;        // CRMC engine
  
  bool m_reseedCRMCRandGen;         // Use CRMC random generator over Gaudi
  Rndm::Numbers m_random;           // Random number generator
  
  std::string m_beamToolName;       // Beam tool name
  IBeamTool *m_beamTool;            // Beam tool
  
  // CRMC configuration flags
  double      m_seed;                    // Random number generator seed
  int         m_HEModel ;// Flag for the MC model
  double      m_projectileMomentum;
  double      m_targetMomentum;
  int         m_projectileID;
  int         m_targetID;
  double      m_minDecayLength;          // CRMC variable to determine stable particles
  std::string m_paramFileName;      // Path to the CRMC parameters file
  bool        m_switchOffEventTruncation;  // Switch off event truncation
  bool        m_produceTables; //Produce EPOS tables
  bool        m_impactParameter; //To define a user impact parameter range
  double      m_minImpactParameter; //min value of the impact parameter
  double      m_maxImpactParameter; //max value of the impact parameter
  bool        m_addUserSettingsToDefault; //add user settings to default settings
  std::string m_frame; //give the frame for the outputs
  
  // Return the particle mass (GeV), selected from the ParticlePropertiesService by mapping CRMC id to PDG id
  double particleMass(int pID);
  
  // Create default CRMC configuration file
  void createDefaultCRMCConfiguration();
  
  // Fill HepMCEvent
  HepMC::GenEvent* FillHepMC(HepMC::GenEvent *theEvent);
};

#endif
