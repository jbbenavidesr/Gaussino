// Local 
#include "CRMCProduction.h"

// std
#include <list>
#include "TMath.h"
#include <fstream>

// Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"

// Generators
#include "Generators/IBeamTool.h"
#include "Generators/StringParse.h"

// HepMC
#include "HepMC/GenEvent.h"
#include "HepMC/HeavyIon.h"
#include "HepMC/PdfInfo.h"
#include "HepMC/GenCrossSection.h"
#include "HepMC/Units.h"

// EPOS
#include "LbCRMC/Epos.h"
#include "LbCRMC/CRMCWrapper.h"

// FORTRAN function calls
extern "C" {
  int idtrafo_( char *, char*, int * , int, int) ;
}

extern "C" {
  void idmass_( int * , float *) ;
}

//-----------------------------------------------------------------------------
//  Implementation file for class: CRMCProduction
//
//  2014-02-03 : Dmitry Popov
//  2015-08-25 :modification to go from EPOS Fortan to HepMC C++ (Laure Massacrier)
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY(CRMCProduction)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CRMCProduction::CRMCProduction(const std::string &type, const std::string &name, 
                               const IInterface *parent) :
GaudiTool(type, name, parent),
  m_printEvent(false),
  m_nEvents(0),
  m_tempParamFileName(""),
  m_tempParamFileName_backup(""),
  m_CRMCEngine(0),
  m_reseedCRMCRandGen(false),
  m_beamToolName(""),
  m_beamTool(0),
  m_seed(-1),
  m_HEModel(0),
  m_projectileMomentum(0.123456789),
  m_targetMomentum(0.123456789),
  m_projectileID(2212),
  m_targetID(2212),
  m_minDecayLength(1.),
  m_paramFileName(""),
  m_switchOffEventTruncation(true),
  m_produceTables(false),
  m_impactParameter(false),
  m_minImpactParameter(0.),
  m_maxImpactParameter(20.),
  m_addUserSettingsToDefault(false),
  m_frame("") {
  // Properties
  declareInterface<IProductionTool>(this);
  declareProperty( "Commands"                  , m_userSettings ) ;
  declareProperty( "BeamToolName"              , m_beamToolName = "CollidingBeams" );
  declareProperty( "PrintEvents"               , m_printEvent = false );
  declareProperty( "RandomSeed"                , m_seed = -1 );
  declareProperty( "ReSeedCRMCRandGenEveryEvt" , m_reseedCRMCRandGen = false );
  declareProperty( "HEModel"                   , m_HEModel = crmc_generators::EPOS_LHC );
  declareProperty( "ProjectileID"              , m_projectileID = 2212 );
  declareProperty( "TargetID"                  , m_targetID = 2212 );
  declareProperty( "ProjectileMomentum"        , m_projectileMomentum = 0.123456789 );
  declareProperty( "TargetMomentum"            , m_targetMomentum = 0.123456789 );
  declareProperty( "MinDecayLength"            , m_minDecayLength = 1.0 );
  declareProperty( "EPOSParamFileName"         , m_paramFileName = "" );
  declareProperty( "SwitchOffEventTruncation"  , m_switchOffEventTruncation = true );
  declareProperty( "ProduceTables"             , m_produceTables = false );
  declareProperty( "ImpactParameter"           , m_impactParameter = false );
  declareProperty( "MinImpactParameter"        , m_minImpactParameter = 0. );
  declareProperty( "MaxImpactParameter"        , m_maxImpactParameter = 20. );
  declareProperty( "AddUserSettingsToDefault"  , m_addUserSettingsToDefault = false );
  declareProperty( "Frame"                     , m_frame = "" );

  always() << "=============================================================" << endmsg;
  always() << " LbCRMC : Using as production engine " << this->type() << endmsg;
  always() << "=============================================================" << endmsg;

  std::cout.setf(std::ios::showpoint);
  std::cout.setf(std::ios::fixed);
  std::cout.precision(18);
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CRMCProduction::~CRMCProduction() {
  // Temporary CRMC parameters file cleanup
  if (m_tempParamFileName.length()) {
    always() << "LbCRMC : Cleanup: removing CRMC temporary options file " 
             << m_tempParamFileName << endmsg;
    remove(m_tempParamFileName.c_str());
  }
  
  m_random.finalize();

  if (m_CRMCEngine)
    delete m_CRMCEngine;
}

//=============================================================================
// Initialize method
//=============================================================================
StatusCode CRMCProduction::initialize() {
  debug() << "CRMCProduction::initialize called" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
    Exception("GaudiTool failed to initialize!");

  // If no custom EPOS configuration specified, create a default one
  if (m_paramFileName == "")
    createDefaultCRMCConfiguration();

  // Initialize random number service
  IRndmGenSvc *rndSvc(0);
  try {
    rndSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  }
  catch (const GaudiException &exc) {
    Exception("LbCRMC : RndmGenSvc not found to initialize CRMC random engine!");
  }

  // Initialize random number generator
  sc = m_random.initialize(rndSvc, Rndm::Flat(0,1));
  if (sc.isFailure())
    Exception("LbCRMC : Random seed generator for CRMC failed to initialize!");

  release(rndSvc);

  // Get a seed for this run (CRMC requires one not larger than 1E+9)
  if (m_seed == -1)
    do {
       m_seed = m_random.shoot() * INT_MAX;
    } while (m_seed > 999999999);

  // Initialize the beam tool
  m_beamTool = tool <IBeamTool> (m_beamToolName, this);

  Gaudi::XYZVector particleBeam1;
  Gaudi::XYZVector particleBeam2;

  m_beamTool -> getMeanBeams( particleBeam1 , particleBeam2 ) ;

  // Momenta can not be equal
  if (m_projectileMomentum == m_targetMomentum)
    throw std::runtime_error("LbCRMC : projectile and target momenta are equal!");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Initialize generator
//=============================================================================
StatusCode CRMCProduction::initializeGenerator() {
  debug() << "CRMCProduction::initializeGenerator called" << endmsg;

  m_CRMCEngine = new CRMCWrapper();

  m_CRMCEngine->set_event_printouts(m_printEvent);
  m_CRMCEngine->set_seed(m_seed);
  m_CRMCEngine->set_high_energy_model(m_HEModel);
  m_CRMCEngine->set_projectile_momentum(m_projectileMomentum);
  m_CRMCEngine->set_target_momentum(m_targetMomentum);
  m_CRMCEngine->set_projectile_id(m_projectileID);
  m_CRMCEngine->set_target_id(m_targetID);

  if(m_produceTables)
    m_CRMCEngine->set_produce_tables(m_produceTables);

  // Fix for a Python configurable bug
  if (!m_paramFileName.length())
    m_paramFileName = m_tempParamFileName_backup;

  m_CRMCEngine->set_parameters_file_name(m_paramFileName);

  if(!m_CRMCEngine->initialize_generator())
    return StatusCode::FAILURE;

  // Print out configuration for this run
  printOutGeneratorConfiguration();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finilize method
//=============================================================================
StatusCode CRMCProduction::finalize() {
  debug() << "CRMCProduction::finalize called" << endmsg;

  return GaudiTool::finalize();
}

//=============================================================================
// Function called to generate one event with CRMC
//=============================================================================
StatusCode CRMCProduction::generateEvent(HepMC::GenEvent *theEvent, 
                                         LHCb::GenCollision *) {
  debug() << "CRMCProduction::generateEvent called" << endmsg;

  // Check generator object is valid
  assert(m_CRMCEngine);

  if (m_reseedCRMCRandGen) {
    // Get a random number for this run (EPOS requires one not larger than 1E+9)
    double newRandomNumber;
    do {
       newRandomNumber = m_random.shoot() * INT_MAX;
    } while (newRandomNumber > 1000000000);
    m_CRMCEngine->reinitialize_random_number_generator(newRandomNumber);
  }

  // Ensure to return a valid event - this is useful in the case of PbPb 
  // simulations with EPOS
  // when the number of particles produced in the event is greater than 10K 
  // - the defaul maximum number of records in a HepMC event.
  if(!m_switchOffEventTruncation){
    // when the number of particles produced in the event is greater 
    // than 10K - the default
    // maximum number of records in a HepMC event.
    while (!theEvent->is_valid()) {
      // Generate the event
      m_CRMCEngine->generate_event(theEvent , true );
      if (!theEvent->is_valid()) {
        warning() << "LbCRMC : CRMC returned a malformed HepMC event, "
                  << "will try to generate another one" << endmsg;
        theEvent->clear();
      }
    } 
  } else{
	  //(Here start the modification to bypass the truncation of events problem)
	  m_CRMCEngine->generate_event(theEvent , false );
    //Take the information from the fortran blocks and Fill the HepMC file
    theEvent = CRMCProduction::FillHepMC(theEvent);
  }  //end of else
     
  //increment the number of events
  m_nEvents++;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Set stable the given particle in CRMC
//=============================================================================
void CRMCProduction::setStable(const LHCb::ParticleProperty *) {
  debug() << "CRMCProduction::setStable called" << endmsg;
}

//=============================================================================
// Update particle properties
//=============================================================================
void CRMCProduction::updateParticleProperties(const LHCb::ParticleProperty *) {
  debug() << "CRMCProduction::updateParticleProperties called" << endmsg;
}

//=============================================================================
// Check if particle is a special one
//=============================================================================
bool CRMCProduction::isSpecialParticle(const LHCb::ParticleProperty *) const {
  return false;
}

//=============================================================================
// Switch on the fragmentation
//=============================================================================
void CRMCProduction::turnOnFragmentation() {
  debug() << "CRMCProduction::turnOnFragmentation called" << endmsg;
}

//=============================================================================
// Switch off the fragmentation
//=============================================================================
void CRMCProduction::turnOffFragmentation() {
  debug() << "CRMCProduction::turnOffFragmentation called" << endmsg;
}

//=============================================================================
// Setup forced fragmentation
//=============================================================================
StatusCode CRMCProduction::setupForcedFragmentation(const int) {
  debug() << "CRMCProduction::setupForcedFragmentation called" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Hadronize the event
//=============================================================================
StatusCode CRMCProduction::hadronize(HepMC::GenEvent *, LHCb::GenCollision *) {
  debug() << "CRMCProduction::hadronize called" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Save the partonic event
//=============================================================================
void CRMCProduction::savePartonEvent(HepMC::GenEvent *) {
  debug() << "CRMCProduction::savePartonEvent called" << endmsg;
}

//=============================================================================
// Reload the partonic event
//=============================================================================
void CRMCProduction::retrievePartonEvent(HepMC::GenEvent *) {
  debug() << "CRMCProduction::retrievePartonEvent called" << endmsg;
}

//=============================================================================
// Print the running conditions
//=============================================================================
void CRMCProduction::printRunningConditions() {
  debug() << "CRMCProduction::printRunningConditions called" << endmsg;
}

//=============================================================================
// Create a temporary parameter file
//=============================================================================
StatusCode CRMCProduction::writeTempGeneratorParamFiles(CommandVector &options) {
  char *tmpName = strdup("/tmp/lbcrmc.XXXXXX");
  mkstemp(tmpName);

  std::ofstream tmpFile(tmpName);

  if (!tmpName) return StatusCode::FAILURE ;

  always() << "LbCRMC : Created temporary file " << tmpName 
           << " for generator's parameters" << endmsg;

  // Write the options to the temp file
  for (CommandVector::iterator iter = options.begin() ; iter != options.end() ; 
       ++iter)
    tmpFile << (*iter);
  
  tmpFile.close();

  // Save the file name to remove later
  m_tempParamFileName = tmpName;

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Print the configuration
//=============================================================================
void CRMCProduction::printOutGeneratorConfiguration() {
  always() << std::endl 
           << "________________________________________________________________" 
           << std::endl 
           << std::endl
           << " LbCRMC : RUN CONFIGURATION" << std::endl << std::endl
           << " Initial random seed: " << (int) m_seed << endmsg ;
  
  if (m_HEModel == crmc_generators::EPOS_LHC)
    always() <<  " MC model: " << m_HEModel << " (EPOS LHC)" << endmsg ;
  else if (m_HEModel == crmc_generators::EPOS_199)
    always() <<  " MC model: " << m_HEModel << " (EPOS 1.99)" << endmsg ;
  else if (m_HEModel == crmc_generators::QGSJET01)
    always() <<  " MC model: " << m_HEModel << " (QGSJet01)" << endmsg ;
  else if (m_HEModel == crmc_generators::GHEISHA)
    always() <<  " MC model: " << m_HEModel << " (Gheisha)" << endmsg;
  else if (m_HEModel == crmc_generators::PYTHIA)
    always() <<  " MC model: " << m_HEModel << " (Pythia)" << endmsg;
  else if (m_HEModel == crmc_generators::HIJING)
    always() <<  " MC model: " << m_HEModel << " (Hijing)" << endmsg ;
  else if (m_HEModel == crmc_generators::SIBYLL)
    always() <<  " MC model: " << m_HEModel << " (Sibyll)" << endmsg ;
  else if (m_HEModel == crmc_generators::QGSJETII_04)
    always() <<  " MC model: " << m_HEModel << " (QGSJetII-04)" << endmsg ;
  else if (m_HEModel == crmc_generators::PHOJET)
    always() <<  " MC model: " << m_HEModel << " (Phojet)" << endmsg ;
  else if (m_HEModel == crmc_generators::DPMJET)
    always() <<  " MC model: " << m_HEModel << " (Dpmjet)" << endmsg;
  else
    always() <<  " MC model: " << m_HEModel 
             << " (Unsupported value?! Please check the documentation "
             << "for the correct generator IDs!)" << endmsg ;

  // Projectile and target info
  always() << " Projectile id and momentum: " << m_projectileID 
           << " (id), " << m_projectileMomentum << " (GeV)" << endmsg ;
  always() << " Target id and momentum: " << m_targetID << " (id), " 
           << m_targetMomentum << " (GeV)" << endmsg ;

  double m_proton = 0.938272046 ;
  double eTarget = TMath::Sqrt(m_targetMomentum*m_targetMomentum + 
                               TMath::Power(m_proton,2));
  double eProjectile = TMath::Sqrt(m_projectileMomentum*m_projectileMomentum + 
                                   TMath::Power(m_proton,2));
  double fSqrts = TMath::Sqrt(TMath::Power(eTarget+eProjectile,2)-
                              TMath::Power(m_targetMomentum+m_projectileMomentum,2));

  always() << " Nucleon-Nucleon center-of-mass energy: " << fSqrts << " (GeV)" 
           << endmsg ;

  always() << " Configuration file: " << m_paramFileName << endmsg ;
  always() << "_________________________________________________________________" 
           << endmsg ;
}

//=============================================================================
// Return the particle mass (MeV), selected from the ParticlePropertiesService
//=============================================================================
double CRMCProduction::particleMass(int pID) {
  double particleMass = 0.;
  
  LHCb::IParticlePropertySvc *particlePropertySvc(0);
  try {
    particlePropertySvc = 
      svc <LHCb::IParticlePropertySvc> ("LHCb::ParticlePropertySvc", true);
  }
  catch (const GaudiException &exc) {
    Exception("LbCRMC : Cannot initialize ParticlePropertySvc!", exc);
  }

  particleMass = (particlePropertySvc->find(LHCb::ParticleID(pID)))->mass();

  return particleMass;
}

//=============================================================================
// Create default CRMC configuration file
//=============================================================================
void CRMCProduction::createDefaultCRMCConfiguration() {
  m_defaultSettings.push_back("switch fusion on\n");
  m_defaultSettings.push_back("application hadron\n");
  //EPOS decay MUST always be off in the code without truncation of the events
  m_defaultSettings.push_back("switch decay off\n"); 
 
  if ( m_frame=="nucleon-nucleon" ) 
    m_defaultSettings.push_back("frame nucleon-nucleon\n") ; 
  else if ( m_frame=="target" )
    m_defaultSettings.push_back("frame target\n") ; 
  else {
    std::string message( std::string( "LbCRMC: You have to define a frame " ) + 
                         std::string( "with the command FRAME" ) ) ;
    throw std::runtime_error( message.c_str() );
  }
  
  if(m_impactParameter){
      std::ostringstream tmp_minImpactParameter;
      std::ostringstream tmp_maxImpactParameter;
      tmp_minImpactParameter << m_minImpactParameter;
      tmp_maxImpactParameter << m_maxImpactParameter;
      m_defaultSettings.push_back("set bminim " + 
                                  tmp_minImpactParameter.str() + "\n");
      m_defaultSettings.push_back("set bmaxim " + 
                                  tmp_maxImpactParameter.str() + "\n");
  } else 
  {
    // special case for PbPb: generate only peripheral events
    if ( ( m_projectileID == 1000822080 ) && ( m_targetID == 1000822080 ) )
    {
      std::ostringstream tmp_minImpactParameter;
      std::ostringstream tmp_maxImpactParameter;
      tmp_minImpactParameter << 8. ;
      tmp_maxImpactParameter << 22. ;
      m_defaultSettings.push_back("set bminim " + 
                                  tmp_minImpactParameter.str() + "\n");
      m_defaultSettings.push_back("set bmaxim " + 
                                  tmp_maxImpactParameter.str() + "\n");      
    } 
  }
 
  if( m_addUserSettingsToDefault){
    //Add User settings to the default configuration
    for ( CommandVector::const_iterator iter = m_userSettings.begin();
        m_userSettings.end() != iter; ++iter ) {   
      std::string mystring(* iter);   
      m_defaultSettings.push_back(mystring + "\n");
      if(mystring.find("switch decay") != std::string::npos) 
      {
        std::string message = std::string( "LbCRMC : You are not allowed to switch " ) + 
          std::string( "on EPOS decay. Use EvtGen to make the decay" );
        throw std::runtime_error( message.c_str() );
      }
      
      if(mystring.find("set bminim") != std::string::npos) 
      {
        std::string message = std::string( "LbCRMC : Use MinImpactParameter in CRMCProduction " ) 
          + std::string( "tool to set an impact parameter range" ) ;
        throw std::runtime_error( message.c_str() ) ;
      }
      
      if(mystring.find("set bmaxim") != std::string::npos)
      {
        std::string message = std::string( "LbCRMC : Use MaxImpactParameter in CRMCProduction " ) 
          + std::string(  "tool to set an impact parameter range" ) ;
        throw std::runtime_error( message.c_str() ) ;
      }

      if(mystring.find("application") != std::string::npos) 
      {
        std::string message = std::string( "LbCRMC : Application already set (hadron). " ) 
          + std::string(  "You are not allowed to change this setting" ) ;        
        throw std::runtime_error( message.c_str() ) ;
      }
      
      if(mystring.find("fusion") != std::string::npos)
      {
        std::string message = std::string( "LbCRMC : Fusion already set (switch on by default). " )
          + std::string( "You are not allowed to change this setting" ) ;
        throw std::runtime_error( message.c_str() ) ;
      }

      if(mystring.find("frame") != std::string::npos)
      {
        std::string message = std::string( "LbCRMC : Use Frame in CRMCProduction tool ") 
          + std::string( "to set the frame" ) ;
        throw std::runtime_error( message.c_str() ) ;
      } 
    }
  } else{
    // Get the path to the CRMC tables from the environment
    std::string pathToGeneratorTables = System::getEnv("CRMC_TABS");
    if (pathToGeneratorTables.length() && pathToGeneratorTables != "UNKNOWN") {
      // Generator's includes
      m_defaultSettings.push_back("fdpmjetpho dat  " + pathToGeneratorTables + 
                                  "/phojet_fitpar.dat\n");
      m_defaultSettings.push_back("fdpmjet dat     " + pathToGeneratorTables + 
                                  "/dpmjet.dat\n");
      m_defaultSettings.push_back("fqgsjet dat     " + pathToGeneratorTables + 
                                  "/qgsjet.dat\n");
      m_defaultSettings.push_back("fqgsjet ncs     " + pathToGeneratorTables + 
                                  "/qgsjet.ncs\n");
      m_defaultSettings.push_back("fqgsjetII03 dat " + pathToGeneratorTables + 
                                  "/qgsdat-II-03.lzma\n");
      m_defaultSettings.push_back("fqgsjetII03 ncs " + pathToGeneratorTables + 
                                  "/sectnu-II-03\n");
      m_defaultSettings.push_back("fqgsjetII dat " + pathToGeneratorTables + 
                                  "/qgsdat-II-04.lzma\n");
      m_defaultSettings.push_back("fqgsjetII ncs " + pathToGeneratorTables + 
                                  "/sectnu-II-04\n");
      m_defaultSettings.push_back("fname check none\n");
      m_defaultSettings.push_back("fname initl " + pathToGeneratorTables + 
                                  "/epos.initl\n");
      m_defaultSettings.push_back("fname iniev " + pathToGeneratorTables + 
                                  "/epos.iniev\n");
      m_defaultSettings.push_back("fname inirj " + pathToGeneratorTables + 
                                  "/epos.inirj\n");
      m_defaultSettings.push_back("fname inics " + pathToGeneratorTables + 
                                  "/epos.inics\n");
      m_defaultSettings.push_back("fname inihy " + pathToGeneratorTables + 
                                  "/epos.inihy\n");
    }
  }
  // Mandatory 'EndOfInput' tag
  m_defaultSettings.push_back("EndEposInput\n");

  // Write CRMC parameters to a temporary file
  if( writeTempGeneratorParamFiles(m_defaultSettings).isSuccess() ) {
    m_paramFileName = m_tempParamFileName;
    m_tempParamFileName_backup = m_tempParamFileName;
  } else {
    throw std::runtime_error( "Error with configuration file" ) ;
  }
}

//======================================================================================
// Fill HepMCEvent directly from epos epout
//======================================================================================
HepMC::GenEvent* CRMCProduction::FillHepMC(HepMC::GenEvent *theEvent) {
  //prepare to store event information
  //Define Units in the HepMC file (be extremely carefull with this)
  theEvent->define_units(HepMC::Units::MEV, HepMC::Units::MM); 
  //this is the unit we want at the end. But we Give GEV and MM
  //we then use a conversion from GeV to theEvent->momentum_unit() unit.
  // This means hepmc should have been compiled with MeV default unit!!!!!!!!
  // by explicitely defining the unit to MEV here it should be always ok, whatever the 
  // default unit HepMC was compiled with. 
  
  //Set cross section information for this event
  HepMC::GenCrossSection cross_section;
  cross_section.set_cross_section( (double) (Epos::hadr5().sigineaa())*1e9 ) ;
  theEvent->set_cross_section( cross_section ) ;

  //fill event information
  HepMC::PdfInfo pdf(0, 0, 0, 0, 0, 0, 0);
  theEvent->set_pdf_info(pdf);
  
  // Fill the heavy ion informations of the event
  HepMC::HeavyIon ion(Epos::cevt().kohevt(), 
                      Epos::cevt().npjevt(),
                      Epos::cevt().ntgevt(),
                      Epos::cevt().kolevt(),
                      Epos::cevt().npnevt() + Epos::cevt().ntnevt(),
                      Epos::cevt().nppevt() + Epos::cevt().ntpevt(),
                      Epos::c2evt().ng1evt(), 
                      Epos::c2evt().ng2evt(), 
                      Epos::cevt().nglevt(),
                      Epos::cevt().bimevt(),
                      Epos::cevt().phievt(),
                      Epos::c2evt().fglevt(), 
                      Epos::hadr5().sigine()*1e9) ; 
  
  theEvent->set_heavy_ion(ion);
  
  // Integer ID uniquely specifying the signal process (i.e. MSUB in Pythia)
  int sig_id = -1;
  
  // If negative typevt mini plasma was created by event (except -4)
  switch ((int)Epos::c2evt().typevt()) {  
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
  default: std::cerr << "LbCRMC : Signal ID not recognised" << std::endl;
  }
  
  theEvent -> set_signal_process_id( sig_id ) ;
  //set event number
  theEvent -> set_event_number( Epos::accum().nrevt() ) ;
  //set default vertex
  HepMC::GenVertex* vertex = new HepMC::GenVertex(HepMC::FourVector(0,0,0,0));
  theEvent -> add_vertex( vertex ) ;
  
  //adapt the hepmcstore epos subroutine in a less complex way     
  double pprojin = 0.0;
  double ptargin = 0.0;
  int idpdg;
  int idprin;
  int idtgin;
  float amass;
  int id;
  
  //first loop over particles to prepare the beam particles
  for(int i = 1 ; i <= Epos::cptl().nptl() ; ++i){
    //prepare beam momenta
    if ( i <= Epos::nucl1().maproj() ) 
      pprojin = pprojin + (double)(Epos::cptl().pptl(3,i));
    else if ( i <= (Epos::nucl1().maproj() + Epos::nucl1().matarg()))
      ptargin = ptargin + (double)(Epos::cptl().pptl(3,i));
  }
  
  //store initial target
  if ( Epos::nucl1().maproj() > 1 ) {
	  idprin = 1000000000 + (Epos::nucl1().maproj()*10) + (Epos::nucl1().laproj()*10000);
  } else {
	  idprin = Epos::hadr25().idprojin();
  } 

  id = idtrafo_((char *)"nxs", (char *)"pdg", &idprin,3,3); 
  idmass_( &idprin , &amass ) ;
  
  //store beam particle 
  HepMC::GenParticle * bpart1 = new HepMC::GenParticle() ;
  bpart1 ->set_pdg_id(id);
  //here is GeV
  bpart1 -> set_momentum( HepMC::FourVector(0.0, 0.0, pprojin,
                                            TMath::Sqrt((TMath::Power(pprojin,2))+
                                                        (TMath::Power((double)(amass),2))))); 
  bpart1-> set_generated_mass((double)(amass)); 
  //in HepMC, beam particle status = 4 (here we give 3 for EvtGen (= particle ignored by EvtGen))
  bpart1 -> set_status(3); 
  bpart1 -> suggest_barcode(1);
  //main vertex is at (0,0,0)
  vertex -> add_particle_in( bpart1 ) ;
  
  //target
  if(Epos::nucl1().matarg() > 1){
	  idtgin = 1000000000 + (Epos::nucl1().matarg()*10) + (Epos::nucl1().latarg()*10000);
  } else{
	  idtgin = Epos::hadr25().idtargin();
  } 
	
  id =  idtrafo_((char *)"nxs", (char *)"pdg", &idtgin,3,3);
  idmass_(&idtgin,&amass);
  
  HepMC::GenParticle * bpart2 = new HepMC::GenParticle() ;
  bpart2 -> set_pdg_id(id);
  //here is GeV
  bpart2 -> set_momentum(HepMC::FourVector(0.0, 0.0, ptargin,
                                           TMath::Sqrt((TMath::Power(ptargin,2))+
                                                       (TMath::Power((double)(amass),2)))));  
  bpart2 -> set_generated_mass((double)(amass)); 
  //in HepMC, beam particle status = 4 (here we give 3 for EvtGen (= particle ignored by EvtGen))
  bpart2 -> set_status(3); 
  bpart2 -> suggest_barcode(2);
  //main vertex is at (0,0,0)
  vertex -> add_particle_in( bpart2 );
  
  //set the beam particles inside the event (not sure it is needed since
  // the particles were added to the default vertex)
  theEvent->set_beam_particles(bpart1 , bpart2 ) ;
  
  //give a new index to the particle in hepmc
  int barecode=3;
  
  //loop over particles
  
  for (int i = 1; i <= Epos::cptl().nptl(); i++) {
    //skip non final particles
    if ( Epos::cptl().istptl(i) == 0 ) { //last generation has status 0 in Epos and +1 in HepMC 
      //convert the id of epos to the pdg id
      idpdg = idtrafo_((char *)"nxs", (char *)"pdg", &Epos::cptl().idptl(i),3,3);
      //convert the id of the particle from epos id to pdg id 
      HepMC::GenParticle * gpart = new HepMC::GenParticle() ;
      gpart -> set_pdg_id( idpdg ) ;
      //recalculate the energy to ensure energy conservation (important to use EvtGen)
      double Energy =
        TMath::Sqrt( (double)(Epos::cptl().pptl(1,i))*(double)(Epos::cptl().pptl(1,i)) + 
                     (double)(Epos::cptl().pptl(2,i))*(double)(Epos::cptl().pptl(2,i)) + 
                     (double)(Epos::cptl().pptl(3,i))*(double)(Epos::cptl().pptl(3,i)) + 
                     (double)(Epos::cptl().pptl(5,i))*(double)(Epos::cptl().pptl(5,i)) );
      gpart -> set_momentum(HepMC::FourVector((double)(Epos::cptl().pptl(1,i)), 
                                              (double)(Epos::cptl().pptl(2,i)), 
                                              (double)(Epos::cptl().pptl(3,i)), 
                                              (double)(Energy)));  //here is GeV
      gpart -> set_generated_mass((double)(Epos::cptl().pptl(5,i))); //here is GeV
      gpart -> set_status(TMath::Min(2,Epos::cptl().istptl(i)+1)); //here is hepmc status
      //in the first loop we only set all particles with proper barecode
      gpart -> suggest_barcode(barecode);
      barecode++;
      vertex -> add_particle_out( gpart );    
    } //end of if
  } //end of for

  //don't forget to rescale the units to MeV (if units where GeV)
  m_CRMCEngine -> convert_to_mev_and_mm( theEvent ) ;

  return theEvent;
}
