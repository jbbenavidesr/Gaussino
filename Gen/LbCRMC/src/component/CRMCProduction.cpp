#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmEngine.h"


#include "Generators/IProductionTool.h"
#include "Generators/IBeamTool.h"

#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"
#include "HepMC/HeavyIon.h"
#include "HepMC/PdfInfo.h"
#include "HepMC/GenCrossSection.h"
#include "HepMC/Units.h"
#include "HepMC/GenParticle.h"

#include "LbCRMC/Epos.h"

#include <list>
#include "TMath.h"

#include "LbCRMC/CRMCWrapper.h"

#include "CRMCProduction.h"



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
CRMCProduction::CRMCProduction(const std::string &type, const std::string &name, const IInterface *parent) :
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
                               m_boostAndRotate(true),
                               m_switchOffEventTruncation(true),
                               m_produceTables(false)

{
  // Properties
  declareInterface<IProductionTool>(this);
  declareProperty("Commands", m_userSettings);
  declareProperty("BeamToolName", m_beamToolName = "CollidingBeams");
  declareProperty("PrintEvents", m_printEvent = false);
  declareProperty("RandomSeed", m_seed = -1);
  declareProperty("ReSeedCRMCRandGenEveryEvt", m_reseedCRMCRandGen = false);
  declareProperty("HEModel", m_HEModel = crmc_generators::EPOS_LHC);
  declareProperty("ProjectileID", m_projectileID = 2212);
  declareProperty("TargetID", m_targetID = 2212);
  declareProperty("ProjectileMomentum", m_projectileMomentum = 0.123456789);
  declareProperty("TargetMomentum", m_targetMomentum = 0.123456789);
  declareProperty("MinDecayLength", m_minDecayLength = 1.0);
  declareProperty("EPOSParamFileName", m_paramFileName = "");
  declareProperty("BoostAndRotate", m_boostAndRotate = true);
  //  declareProperty("SwitchOffDecayInEPOS", m_switchOffDecayInEPOS = false);
  declareProperty("SwitchOffEventTruncation", m_switchOffEventTruncation = true);
  declareProperty("ProduceTables", m_produceTables = false);

  // Matrices for boost and rotation calculation
  m_transformToCMSMatrix = new TMatrixD(4, 4);
  m_transformToLabMatrix = new TMatrixD(4, 4);
  m_rotationToGeneratorMatrix = new TMatrixD(4, 4);
  m_rotationToCMSMatrix = new TMatrixD(4, 4);

  always() << "=============================================================" << endmsg;
  always() << " LbCRMC : Using as production engine " << this->type() << endmsg;
  always() << "=============================================================" << endmsg;

  std::cout.setf(std::ios::showpoint);
  std::cout.setf(std::ios::fixed);
  std::cout.precision(18);
}

// Destructor
CRMCProduction::~CRMCProduction() {
  // Temporary CRMC parameters file cleanup
  if (m_tempParamFileName.length()) {
    always() << "LbCRMC : Cleanup: removing CRMC temporary options file " << m_tempParamFileName << endreq;
    remove(m_tempParamFileName.c_str());
  }

  m_random.finalize();

  if (m_transformToCMSMatrix)
    delete m_transformToCMSMatrix;
  if(m_transformToLabMatrix)
    delete m_transformToLabMatrix;
  if (m_rotationToGeneratorMatrix)
    delete m_rotationToGeneratorMatrix;
  if (m_rotationToCMSMatrix)
    delete m_rotationToCMSMatrix;

  if (m_CRMCEngine)
    delete m_CRMCEngine;
}

//=============================================================================
// Initialize method
//=============================================================================
StatusCode CRMCProduction::initialize() {
  debug() << "CRMCProduction::initialize called" << endreq;

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
    Exception("GaudiTool failed to initialize!");

  // If no custom EPOS configuration specified, create a default one
  if (m_paramFileName == "")
    createDefaultCRMCConfiguratio();

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

  m_beamTool->getMeanBeams(particleBeam1, particleBeam2);

  // Boost and rotate if asked to
  if (m_boostAndRotate) {
    TVectorD tmp_projectile4Vector(4);
    TVectorD tmp_target4Vector(4);

    double tmp_projectileParticleMass = particleMass(m_projectileID);
    double tmp_targetParticleMass = particleMass(m_targetID);

    // Projectile momentum
    StatusCode sc = copyGaudiXYZVecToTVecD(particleBeam1, tmp_projectile4Vector);
    if (sc.isFailure())
      return sc;

    // If there is user-defined momentum - use it
    if (m_projectileMomentum != 0.123456789) {
      tmp_projectile4Vector[1] = 0.;
      tmp_projectile4Vector[2] = 0.;
      tmp_projectile4Vector[3] = m_projectileMomentum * Gaudi::Units::GeV / LHCb::ParticleID(m_projectileID).A(); //momentum per nucleon and not total momentum!
    }
    tmp_projectile4Vector[0] = sqrt(tmp_projectile4Vector[1] * tmp_projectile4Vector[1] +
                                    tmp_projectile4Vector[2] * tmp_projectile4Vector[2] +
                                    tmp_projectile4Vector[3] * tmp_projectile4Vector[3] +
                                    tmp_projectileParticleMass * tmp_projectileParticleMass);

    // Target momentum
    sc = copyGaudiXYZVecToTVecD(particleBeam2, tmp_target4Vector);
    if (sc.isFailure())
      return sc;

    // If there is user-defined momentum - use it
    if (m_targetMomentum != 0.123456789) {
      tmp_target4Vector[1] = 0.;
      tmp_target4Vector[2] = 0.;
      tmp_target4Vector[3] = m_targetMomentum * Gaudi::Units::GeV / LHCb::ParticleID(m_targetID).A();
    }
    tmp_target4Vector[0] = sqrt(tmp_target4Vector[1] * tmp_target4Vector[1] +
                                tmp_target4Vector[2] * tmp_target4Vector[2] +
                                tmp_target4Vector[3] * tmp_target4Vector[3] +
                                tmp_targetParticleMass * tmp_targetParticleMass);

    // Lorentz transformation matrix
    sc = constructTransformMatrix(tmp_projectile4Vector, tmp_target4Vector);
    if (sc.isFailure())
      return sc;

    // Projectile and target 4-vectors in the CMS frame (K')
    TVectorD tmp_projectile4VectorCMS = (*m_transformToCMSMatrix) * tmp_projectile4Vector;
    TVectorD tmp_target4VectorCMS = (*m_transformToCMSMatrix) * tmp_target4Vector;

    // Rotation matrix (K' -> K'')
    sc = constructRotationMatrix(tmp_projectile4VectorCMS);
    if (sc.isFailure())
      return sc;

    // Projectile and target 4-vectors in the generator frame (K'')
    TVectorD tmp_projectile4VectorGenerator = (*m_rotationToGeneratorMatrix) * tmp_projectile4VectorCMS;
    TVectorD tmp_target4VectorGenerator = (*m_rotationToGeneratorMatrix) * tmp_target4VectorCMS;

    // CRMC requires input in GeV
    m_projectileMomentum = tmp_projectile4VectorGenerator[3] / Gaudi::Units::GeV;
    m_targetMomentum = tmp_target4VectorGenerator[3] / Gaudi::Units::GeV;
  } else {
    info() << "LbCRMC : Not applying boost and rotation to the input data." << endmsg;
    if ((m_projectileMomentum == 0.123456789 || m_targetMomentum == 0.123456789)) {
      m_projectileMomentum = particleBeam1.Z() / Gaudi::Units::GeV;
      m_targetMomentum = particleBeam2.Z() / Gaudi::Units::GeV;
    }
  }

  // Momenta can not be equal
  if (m_projectileMomentum == m_targetMomentum)
    throw std::runtime_error("LbCRMC : projectile and target momenta are equal!");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Initialize generator
//=============================================================================
StatusCode CRMCProduction::initializeGenerator() {
  debug() << "CRMCProduction::initializeGenerator called" << endreq;

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
  debug() << "CRMCProduction::finalize called" << endreq;

  return GaudiTool::finalize();
}

//=============================================================================
// Function called to generate one event with CRMC
//=============================================================================
StatusCode CRMCProduction::generateEvent(HepMC::GenEvent *theEvent, LHCb::GenCollision *) {
  debug() << "CRMCProduction::generateEvent called" << endreq;

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

  // Ensure to return a valid event - this is useful in the case of PbPb simulations with EPOS
  // when the number of particles produced in the event is greater than 10K - the default
  // maximum number of records in a HepMC event.
  if(!m_switchOffEventTruncation){  
 
  // when the number of particles produced in the event is greater than 10K - the default
  // maximum number of records in a HepMC event.
    while (!theEvent->is_valid()) {
  	// Generate the event
	  m_CRMCEngine->generate_event(theEvent);
	  theEvent->print();
	  //loop over particles and print some infos to check
	  /* for ( HepMC::GenEvent::particle_iterator itrPart = theEvent->particles_begin(); itrPart != theEvent->particles_end(); ++itrPart ) {
	         HepMC::GenParticle* genPart1=*itrPart;
		 genPart1->print();
		 }*/

	  if (!theEvent->is_valid()) {
	  	warning() << "LbCRMC : CRMC returned a malformed HepMC event, will try to generate another one" << endmsg;
	  	theEvent->clear();
	  }
    } 
  }
  else{
	  //(Here start the modification to bypass the truncation of events problem)
	  m_CRMCEngine->generate_event(theEvent);
	  //clear the information of the event
	  theEvent->clear();
          //Take the information from the fortran blocks and Fill the HepMC file
          theEvent = CRMCProduction::FillHepMC(theEvent);
          //check the format of the HepMC file
	  theEvent->print();
	    
  }  //end of else
    
     
  //increment the number of events
  m_nEvents++;

  // Apply rotation and boost to the simulated event
  if (m_boostAndRotate)
    applyBoostAndRotationToEvent(theEvent);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Set stable the given particle in CRMC
//=============================================================================
void CRMCProduction::setStable(const LHCb::ParticleProperty *) {
  debug() << "CRMCProduction::setStable called" << endreq;
  //std::cout << (*thePP) << "  ---  " << thePP->pid().pid() << std::endl;
}

//=============================================================================
// Update particle properties
//=============================================================================
void CRMCProduction::updateParticleProperties(const LHCb::ParticleProperty *) {
  debug() << "CRMCProduction::updateParticleProperties called" << endreq;
  //std::cout << (*thePP) << "  ---  " << thePP->pid().pid() << std::endl;
}

// Returns TRUE if the particle is a special particle and must not be modified
bool CRMCProduction::isSpecialParticle(const LHCb::ParticleProperty *) const {
  return false;
}

// Turn on fragmentation in CRMC
void CRMCProduction::turnOnFragmentation() {
  debug() << "CRMCProduction::turnOnFragmentation called" << endreq;
}

// Turn off fragmentation in CRMC
void CRMCProduction::turnOffFragmentation() {
  debug() << "CRMCProduction::turnOffFragmentation called" << endreq;
}

// Setup for forced fragmentation
StatusCode CRMCProduction::setupForcedFragmentation(const int) {
  debug() << "CRMCProduction::setupForcedFragmentation called" << endreq;
  return StatusCode::SUCCESS;
}

// Hadronize CRMC event
StatusCode CRMCProduction::hadronize(HepMC::GenEvent *, LHCb::GenCollision *) {
  debug() << "CRMCProduction::hadronize called" << endreq;
  return StatusCode::SUCCESS;
}

// Save parton event
void CRMCProduction::savePartonEvent(HepMC::GenEvent *) {
  debug() << "CRMCProduction::savePartonEvent called" << endreq;
}

// Load parton event
void CRMCProduction::retrievePartonEvent(HepMC::GenEvent *) {
  debug() << "CRMCProduction::retrievePartonEvent called" << endreq;
}

// Debug print out to be printed after all initializations
void CRMCProduction::printRunningConditions() {
  debug() << "CRMCProduction::printRunningConditions called" << endreq;
}

// Create temporary generator parameters file with default options
int CRMCProduction::writeTempGeneratorParamFiles(CommandVector &options) {
  char *tmpName = strdup("/tmp/lbcrmc.XXXXXX");
  mkstemp(tmpName);

  std::ofstream tmpFile(tmpName);

  if (!tmpName)
    return 1;

  always() << "LbCRMC : Created temporary file " << tmpName << " for generator's parameters" << endmsg;

  // Write the options to the temp file
  for (CommandVector::iterator iter = options.begin(); iter != options.end(); iter++)
    tmpFile << (*iter);

  tmpFile.close();

  // Save the file name to remove later
  m_tempParamFileName = tmpName;

  return 0;
}

// Print out generator configuration
void CRMCProduction::printOutGeneratorConfiguration() {
  std::cout << std::endl << "_____________________________________________________________________________" << std::endl << std::endl;
  std::cout << " LbCRMC : RUN CONFIGURATION" << std::endl << std::endl;
  std::cout << " Initial random seed: " << (int) m_seed << std::endl;

  // Which generator is requested
  std::cout << " MC model: " << m_HEModel;
  if (m_HEModel == crmc_generators::EPOS_LHC)
    std::cout << " (EPOS LHC)" << std::endl;
  else if (m_HEModel == crmc_generators::EPOS_199)
    std::cout << " (EPOS 1.99)" << std::endl;
  else if (m_HEModel == crmc_generators::QGSJET01)
    std::cout << " (QGSJet01)" << std::endl;
  else if (m_HEModel == crmc_generators::GHEISHA)
    std::cout << " (Gheisha)" << std::endl;
  else if (m_HEModel == crmc_generators::PYTHIA)
    std::cout << " (Pythia)" << std::endl;
  else if (m_HEModel == crmc_generators::HIJING)
    std::cout << " (Hijing)" << std::endl;
  else if (m_HEModel == crmc_generators::SIBYLL)
    std::cout << " (Sibyll)" << std::endl;
  else if (m_HEModel == crmc_generators::QGSJETII_04)
    std::cout << " (QGSJetII-04)" << std::endl;
  else if (m_HEModel == crmc_generators::PHOJET)
    std::cout << " (Phojet)" << std::endl;
  else if (m_HEModel == crmc_generators::DPMJET)
    std::cout << " (Dpmjet)" << std::endl;
  else
    std::cout << " (Unsupported value?! Please check the documentation for the correct generator IDs!)" << std::endl;

  // Projectile and target info
  std::cout << " Projectile id and momentum: " << m_projectileID << " (id), " << m_projectileMomentum << " (GeV)" << std::endl;
  std::cout << " Target id and momentum: " << m_targetID << " (id), " << m_targetMomentum << " (GeV)" << std::endl;

  // Boost and rotation
  std::cout << " Apply boost and rotation: ";
  if (m_boostAndRotate)
    std::cout << "YES" << std::endl;
  else
    std::cout << "NO" << std::endl;

  // Decay handling
  /* std::cout << " CRMC decays particles: ";
  if (m_switchOffDecayInEPOS)
    std::cout << "NO" << std::endl;
  else
  std::cout << "YES" << std::endl;*/

  std::cout << " Minimum decay length (ignore if using a custom config file): " << m_minDecayLength << std::endl;
  std::cout << " Configuration file: " << m_paramFileName << std::endl;
  std::cout << "_____________________________________________________________________________" << std::endl << std::endl;
}

// Return the particle mass (MeV), selected from the ParticlePropertiesService
double CRMCProduction::particleMass(int pID) {
  double particleMass = 0.;

  LHCb::IParticlePropertySvc *particlePropertySvc(0);
  try {
    particlePropertySvc = svc <LHCb::IParticlePropertySvc> ("LHCb::ParticlePropertySvc", true);
  }
  catch (const GaudiException &exc) {
    Exception("LbCRMC : Cannot initialize ParticlePropertySvc!", exc);
  }

  particleMass = (particlePropertySvc->find(LHCb::ParticleID(pID)))->mass();

  return particleMass;
}

// Copy the values of the Gaudi::XYZVector tp the TVectorD
StatusCode CRMCProduction::copyGaudiXYZVecToTVecD(const Gaudi::XYZVector &gVec, TVectorD &tVec) {
  if (tVec.GetNrows() != 4) {
    return Error("LbCRMC : copyGaudiXYZVecToTVecD : TVector is of a wrong size!");
  } else {
    tVec[1] = gVec.X();
    tVec[2] = gVec.Y();
    tVec[3] = gVec.Z();
  }

  return StatusCode::SUCCESS;
}

// Construct the lab to CMS transformation matrix (K -> K')
StatusCode CRMCProduction::constructTransformMatrix(const TVectorD &projectileMomentum, const TVectorD &targetMomentum) {
  TVectorD beta(3);
  {
    TVectorD tmp_p1 = projectileMomentum.GetSub(1, 3);
    TVectorD tmp_p2 = targetMomentum.GetSub(1, 3);

    // beta = - (p1 + p2) / (E1 + E2)
    beta = tmp_p1 + tmp_p2;
    if (projectileMomentum[0] + targetMomentum[0] != 0) {
      beta *= -1 / (projectileMomentum[0] + targetMomentum[0]);
    } else {
      return Error("LbCRMC : constructTransformMatrix : The summ of the energies is zero!");
    }
  }

  // Calculate Gamma
  double gamma;
  {
    double tmp_gammaDenominator = 1 - beta.Norm2Sqr();
    if (tmp_gammaDenominator != 0) {
      gamma = 1 / sqrt(tmp_gammaDenominator);
    } else {
      return Error("LbCRMC : Beta^2 is 1!");
    }
  }

  // Lab to CMS Lorentz transform matrix (K -> K')
  {
    double tmp_gammaArgument = gamma * gamma / (gamma + 1);

    // Row 1
    (*m_transformToCMSMatrix)[0][0] = gamma;
    (*m_transformToCMSMatrix)[0][1] = gamma * beta[0]; // gamma * Beta_x
    (*m_transformToCMSMatrix)[0][2] = gamma * beta[1]; // gamma * Beta_y
    (*m_transformToCMSMatrix)[0][3] = gamma * beta[2]; // gamma * Beta_z

    // Row 2
    (*m_transformToCMSMatrix)[1][0] = gamma * beta[0];
    (*m_transformToCMSMatrix)[1][1] = 1 + tmp_gammaArgument * beta[0] * beta[0];
    (*m_transformToCMSMatrix)[1][2] = tmp_gammaArgument * beta[0] * beta[1];
    (*m_transformToCMSMatrix)[1][3] = tmp_gammaArgument * beta[0] * beta[2];

    // Row 3
    (*m_transformToCMSMatrix)[2][0] = gamma * beta[1];
    (*m_transformToCMSMatrix)[2][1] = tmp_gammaArgument * beta[1] * beta[0];
    (*m_transformToCMSMatrix)[2][2] = 1 + tmp_gammaArgument * beta[1] * beta[1];
    (*m_transformToCMSMatrix)[2][3] = tmp_gammaArgument * beta[1] * beta[2];

    // Row 4
    (*m_transformToCMSMatrix)[3][0] = gamma * beta[2];
    (*m_transformToCMSMatrix)[3][1] = tmp_gammaArgument * beta[2] * beta[0];
    (*m_transformToCMSMatrix)[3][2] = tmp_gammaArgument * beta[2] * beta[1];
    (*m_transformToCMSMatrix)[3][3] = 1 + tmp_gammaArgument * beta[2] * beta[2];
  }

  // Construct the CMS to Lab transformation matrix (K' -> K)
  (*m_transformToLabMatrix) = (*m_transformToCMSMatrix);
  m_transformToLabMatrix->Invert();

  return StatusCode::SUCCESS;
}

// Construction of the rotation matrix
StatusCode CRMCProduction::constructRotationMatrix(const TVectorD &beamMomentumCMS) {
  // Rotation submatrix
  TMatrixD rotationSubmatrix(3, 3);
  {
    double cos_theta_x = beamMomentumCMS[3] / sqrt(beamMomentumCMS[1] * beamMomentumCMS[1] +
                                                   beamMomentumCMS[2] * beamMomentumCMS[2] +
                                                   beamMomentumCMS[3] * beamMomentumCMS[3]); // cos(Theta_x) = p_z / p_norm
    double sin_theta_x = sqrt(1 - cos_theta_x * cos_theta_x);
    double theta_z = atan2(beamMomentumCMS[1], beamMomentumCMS[2]); // theta_z = arctan ( p_x / p_y )
    double sin_theta_z = sin(theta_z);
    double cos_theta_z = cos(theta_z);

    // Row 1
    rotationSubmatrix[0][0] = cos_theta_z;
    rotationSubmatrix[0][1] = sin_theta_z * cos_theta_x;
    rotationSubmatrix[0][2] = sin_theta_z * sin_theta_x;

    // Row 2
    rotationSubmatrix[1][0] = - sin_theta_z;
    rotationSubmatrix[1][1] = cos_theta_z * cos_theta_x;
    rotationSubmatrix[1][2] = cos_theta_z * sin_theta_x;

    // Row 3
    rotationSubmatrix[2][0] = 0.;
    rotationSubmatrix[2][1] = - sin_theta_x;
    rotationSubmatrix[2][2] = cos_theta_x;
  }

  // K'' -> K' rotation
  (*m_rotationToCMSMatrix)[0][0] = 1.;
  StatusCode sc = copySubmatrixIntoMatrix(rotationSubmatrix, m_rotationToCMSMatrix);
  if (sc.isFailure())
    return sc;

  rotationSubmatrix.Invert();

  // K' -> K'' rotation
  (*m_rotationToGeneratorMatrix)[0][0] = 1.;
  sc = copySubmatrixIntoMatrix(rotationSubmatrix, m_rotationToGeneratorMatrix);
  if (sc.isFailure())
    return sc;

  return StatusCode::SUCCESS;
}

// Copy submatrix A into B
StatusCode CRMCProduction::copySubmatrixIntoMatrix(const TMatrixD &A, TMatrixD *B) {
  if (A.GetNrows() < B->GetNrows() && A.GetNcols() < B->GetNcols()) {
    for (int row = 0; row < A.GetNrows(); ++ row) {
      for (int col = 0; col < A.GetNcols(); ++ col) {
        (*B)[row + 1][col + 1] = A[row][col];
      }
    }
  } else {
    return Error("LbCRMC : copySubmatrixIntoMatrix : Can not copy the submatrix into the matrix: incompatible sizes!");
  }

  return StatusCode::SUCCESS;
}

// Apply boost and rotation to the simulated event
StatusCode CRMCProduction::applyBoostAndRotationToEvent(HepMC::GenEvent *theEvent) {
  if (!theEvent)
    return Error("LbCRMC : applyBoostAndRotationToEvent : Null pointer for the event passed to applyBoostAndRotationToEvent()!");

  // Loop over particle
  for (HepMC::GenEvent::particle_iterator particle = theEvent->particles_begin(); particle != theEvent->particles_end(); ++particle) {
    HepMC::FourVector tmp_particle4Vector = (*particle)->momentum();

    TVectorD tmp_particle4VectorGenerator(4);

    StatusCode sc = copyHepMCVecToTVecD(tmp_particle4Vector, tmp_particle4VectorGenerator);
    if (sc.isFailure())
      return sc;

    // Apply rotation and transform (K'' -> K' -> K)
    TVectorD tmp_particle4VectorLab = (*m_transformToLabMatrix) * ((*m_rotationToCMSMatrix) * tmp_particle4VectorGenerator);

    sc = copyTVecDToHepMCVec(tmp_particle4VectorLab, tmp_particle4Vector);
    if (sc.isFailure())
      return sc;

    // Apply changes to the event
    (*particle)->set_momentum(tmp_particle4Vector);
  }

  // Loop over vertices
  for (HepMC::GenEvent::vertex_iterator vertex = theEvent->vertices_begin(); vertex != theEvent->vertices_end(); ++vertex) {
    HepMC::FourVector tmp_vertex4Vector = (*vertex)->position();

    TVectorD tmp_vertex4VectorGenerator(4);

    StatusCode sc = copyHepMCVecToTVecD(tmp_vertex4Vector, tmp_vertex4VectorGenerator);
    if (sc.isFailure())
      return sc;

    // Apply rotation and transform (K'' -> K' -> K)
    TVectorD tmp_vertex4VectorLab = (*m_transformToLabMatrix) * ((*m_rotationToCMSMatrix) * tmp_vertex4VectorGenerator);

    sc = copyTVecDToHepMCVec(tmp_vertex4VectorLab, tmp_vertex4Vector);
    if (sc.isFailure())
      return sc;

    // Apply changes to the event
    (*vertex)->set_position(tmp_vertex4Vector);
  }

  return StatusCode::SUCCESS;
}

// Copy the values of the HepMC::FourVector to the TVectorD
StatusCode CRMCProduction::copyHepMCVecToTVecD(const HepMC::FourVector &hVec, TVectorD &tVec) {
  if (tVec.GetNrows() != 4) {
    return Error("LbCRMC : copyHepMCVecToTVecD : TVector is of a wrong size!");
  } else {
    tVec[0] = hVec.e();
    tVec[1] = hVec.px();
    tVec[2] = hVec.py();
    tVec[3] = hVec.pz();
  }

  return StatusCode::SUCCESS;
}

// Copy the values of the TVectorD to the HepMC::FourVector
StatusCode CRMCProduction::copyTVecDToHepMCVec(const TVectorD &tVec, HepMC::FourVector &hVec) {
  if (tVec.GetNrows() != 4) {
    return Error("LbCRMC : copyTVecDToHepMCVec : TVector is of a wrong size!");
  } else {
    hVec.setE(tVec[0]);
    hVec.setPx(tVec[1]);
    hVec.setPy(tVec[2]);
    hVec.setPz(tVec[3]);
  }

  return StatusCode::SUCCESS;
}

// Create default CRMC configuration file
void CRMCProduction::createDefaultCRMCConfiguratio() {
  m_defaultSettings.push_back("switch fusion on\n");
  m_defaultSettings.push_back("application hadron\n");
  m_defaultSettings.push_back("frame nucleon-nucleon\n");

  // Should CRMC decay particles or not?
  // if (m_switchOffDecayInEPOS)
  m_defaultSettings.push_back("switch decay off\n");



  // List of particles not to decay by the generator
  m_defaultSettings.push_back("nodecay  14\n");
  m_defaultSettings.push_back("nodecay -14\n");
  m_defaultSettings.push_back("nodecay  1120\n");
  m_defaultSettings.push_back("nodecay -1120\n");
  m_defaultSettings.push_back("nodecay  1220\n");
  m_defaultSettings.push_back("nodecay -1220\n");
  m_defaultSettings.push_back("nodecay  120\n");
  m_defaultSettings.push_back("nodecay -120\n");
  m_defaultSettings.push_back("nodecay  130\n");
  m_defaultSettings.push_back("nodecay -130\n");
  m_defaultSettings.push_back("nodecay -20\n");
  m_defaultSettings.push_back("nodecay  17\n");
  m_defaultSettings.push_back("nodecay -17\n");
  m_defaultSettings.push_back("nodecay  18\n");
  m_defaultSettings.push_back("nodecay -18\n");
  m_defaultSettings.push_back("nodecay  19\n");
  m_defaultSettings.push_back("nodecay -19\n");

  // Which particles to consider stable - remove (or set to -1) to decay everything in CRMC
  if (m_minDecayLength != 0) {
    std::ostringstream tmp_minDecayLengthStr;
    tmp_minDecayLengthStr << m_minDecayLength;
    m_defaultSettings.push_back("MinDecayLength  " + tmp_minDecayLengthStr.str() + "\n");
  }

  

  // Get the path to the CRMC tables from the environment
  std::string pathToGeneratorTables = System::getEnv("CRMC_TABS");
  if (pathToGeneratorTables.length() && pathToGeneratorTables != "UNKNOWN") {
    // Generator's includes
    m_defaultSettings.push_back("fdpmjetpho dat  " + pathToGeneratorTables + "/phojet_fitpar.dat\n");
    m_defaultSettings.push_back("fdpmjet dat     " + pathToGeneratorTables + "/dpmjet.dat\n");
    m_defaultSettings.push_back("fqgsjet dat     " + pathToGeneratorTables + "/qgsjet.dat\n");
    m_defaultSettings.push_back("fqgsjet ncs     " + pathToGeneratorTables + "/qgsjet.ncs\n");
    m_defaultSettings.push_back("fqgsjetII03 dat " + pathToGeneratorTables + "/qgsdat-II-03.lzma\n");
    m_defaultSettings.push_back("fqgsjetII03 ncs " + pathToGeneratorTables + "/sectnu-II-03\n");
    m_defaultSettings.push_back("fqgsjetII dat " + pathToGeneratorTables + "/qgsdat-II-04.lzma\n");
    m_defaultSettings.push_back("fqgsjetII ncs " + pathToGeneratorTables + "/sectnu-II-04\n");
    m_defaultSettings.push_back("fname check  none\n");
    m_defaultSettings.push_back("fname initl  " + pathToGeneratorTables + "/epos.initl\n");
    m_defaultSettings.push_back("fname iniev  " + pathToGeneratorTables + "/epos.iniev\n");
    m_defaultSettings.push_back("fname inirj  " + pathToGeneratorTables + "/epos.inirj\n");
    m_defaultSettings.push_back("fname inics  " + pathToGeneratorTables + "/epos.inics\n");
    m_defaultSettings.push_back("fname inihy  " + pathToGeneratorTables + "/epos.inihy\n");
  }

  // EPOS printouts (for debugging)
  //m_defaultSettings.push_back("print * 4\n");
  //m_defaultSettings.push_back("printcheck screen\n");

  // Mandatory 'EndOfInput' tag
  m_defaultSettings.push_back("EndEposInput\n");

  // Write CRMC parameters to a temporary file
  if(!writeTempGeneratorParamFiles(m_defaultSettings)) {
    m_paramFileName = m_tempParamFileName;
    m_tempParamFileName_backup = m_tempParamFileName;
  }
}

// Fill HepMCEvent directly from epos epout
HepMC::GenEvent* CRMCProduction::FillHepMC(HepMC::GenEvent *theEvent) {

    //prepare to store event information
    //Define Units in the HepMC file (be extremely carefull with this)
    theEvent->define_units(HepMC::Units::MEV, HepMC::Units::MM); //this is the unit we want at the end. But we Give GEV and MM
    //we then use a conversion from GeV to theEvent->momentum_unit() unit. This means hepmc should have been compiled with MeV default unit!!!!!!!!
    //by explicitely defining the unit to MEV here it should be always ok, whatever the default unit HepMC was compiled with. 

    //Set cross section information for this event
    HepMC::GenCrossSection cross_section;
    cross_section.set_cross_section((double)(Epos::hadr5().sigineaa())*1e9); //required in pB
    theEvent->set_cross_section(cross_section);
    //fill event information
    // Provide optional pdf set id numbers for CMSSW to work flavour of partons and stuff... hope it's optional
    HepMC::PdfInfo pdf(0, 0, 0, 0, 0, 0, 0);
    theEvent->set_pdf_info(pdf);

    // Setting heavy ion information
    // int   Ncoll_hard          // Number of hard scatterings
    // int   Npart_proj          // Number of projectile participants
    // int   Npart_targ          // Number of target participants
    // int   Ncoll               // Number of NN (nucleon-nucleon) collisions
    // int   spectator_neutrons           // Number of spectator neutrons
    // int   spectator_protons            // Number of spectator protons
    // int   N_Nwounded_collisions        // Number of N-Nwounded collisions (here Glauber number of participants with at least 1 interaction)
    // int   Nwounded_N_collisions        // Number of Nwounded-N collisons (here Glauber number of participants with at least 2 interaction2)
    // int   Nwounded_Nwounded_collisions // Number of Nwounded-Nwounded collisions (here GLauber number of collisions)
    // float impact_parameter        // Impact Parameter(fm) of collision
    // float event_plane_angle       // Azimuthal angle of event plane
    // float eccentricity            // eccentricity of participating nucleons
    //                                        in the transverse plane
    //                                        (as in phobos nucl-ex/0510031)
    // float sigma_inel_NN           // nucleon-nucleon inelastic
    //                                        (including diffractive) cross-section
	 

    //print some infos to check that the common block is properly written
    //print infos from common block Hadr4
    /*  printf("sigtot=%f\n",Epos::hadr5().sigtot());
    printf("sigcut=%f\n",Epos::hadr5().sigcut());
    printf("sigela=%f\n",Epos::hadr5().sigela());
    printf("sloela=%f\n",Epos::hadr5().sloela());
    printf("sigsd=%f\n",Epos::hadr5().sigsd());
    printf("sigine=%f\n",Epos::hadr5().sigine());
    printf("sigdif=%f\n",Epos::hadr5().sigdif());
    printf("sigineaa=%f\n",Epos::hadr5().sigineaa());
    printf("sigtotaa=%f\n",Epos::hadr5().sigtotaa());
    printf("sigelaaa=%f\n",Epos::hadr5().sigelaaa());
    printf("sigcutaa=%f\n",Epos::hadr5().sigcutaa());
    printf("sigdd=%f\n",Epos::hadr5().sigdd());
    //print infos from common block Cevt	 
    printf("phievt=%f\n",Epos::cevt().phievt());
    printf("nevt=%i\n",Epos::cevt().nevt());
    printf("bimevt=%f\n",Epos::cevt().bimevt());
    printf("kolevt=%i\n",Epos::cevt().kolevt());
    printf("koievt=%i\n",Epos::cevt().koievt());
    printf("pmxevt=%f\n",Epos::cevt().pmxevt());
    printf("egyevt=%f\n",Epos::cevt().egyevt());
    printf("npjevt=%i\n",Epos::cevt().npjevt());
    printf("ntgevt=%i\n",Epos::cevt().ntgevt());
    printf("npnevt=%i\n",Epos::cevt().npnevt());
    printf("nppevt=%i\n",Epos::cevt().nppevt());
    printf("ntnevt=%i\n",Epos::cevt().ntnevt());
    printf("ntpevt=%i\n",Epos::cevt().ntpevt());
    printf("jpnevt=%i\n",Epos::cevt().jpnevt());
    printf("jppevt=%i\n",Epos::cevt().jppevt());
    printf("jtnevt=%i\n",Epos::cevt().jtnevt());
    printf("jtpevt=%i\n",Epos::cevt().jtpevt());
    printf("jpnevt=%i\n",Epos::cevt().jpnevt());
    printf("xbjevt=%f\n",Epos::cevt().xbjevt());
    printf("qsqevt=%f\n",Epos::cevt().qsqevt());
    printf("nglevt=%i\n",Epos::cevt().nglevt());
    printf("zppevt=%f\n",Epos::cevt().zppevt());
    printf("zptevt=%f\n",Epos::cevt().zptevt());
    printf("minfra=%i\n",Epos::cevt().minfra());
    printf("maxfra=%i\n",Epos::cevt().maxfra());
    printf("kohevt=%i\n",Epos::cevt().kohevt());
    //print infos from common block C2evt
    printf("ng1evt=%i\n",Epos::c2evt().ng1evt());
    printf("ng2evt=%i\n",Epos::c2evt().ng2evt());
    printf("rglevt=%f\n",Epos::c2evt().rglevt());
    printf("sglevt=%f\n",Epos::c2evt().sglevt());
    printf("eglevt=%f\n",Epos::c2evt().eglevt());
    printf("fglevt=%f\n",Epos::c2evt().fglevt());
    printf("ikoevt=%i\n",Epos::c2evt().ikoevt());
    printf("typevt=%f\n",Epos::c2evt().typevt());
    //print infos from common block Accum
    printf("nrevt=%i\n",Epos::accum().nrevt());
    //print infos from common block Nucl1
    printf("maproj=%i\n",Epos::nucl1().maproj());
    printf("matarg=%i\n",Epos::nucl1().matarg());
    printf("laproj=%i\n",Epos::nucl1().laproj());
    printf("latarg=%i\n",Epos::nucl1().latarg());
    //print infos from common block Othe1
    printf("istmax=%i\n",Epos::othe1().istmax());
    //print infos from common block Appli
    printf("model=%i\n",Epos::appli().model());
    //print infos from common block Drop7
    printf("ioclude=%i\n",Epos::drop7().ioclude());
    //printf infos from common block Hadr25
    printf("idprojin=%i\n",Epos::hadr25().idprojin());*/
  
    // Fill the heavy ion informations of the event
    HepMC::HeavyIon ion(Epos::cevt().kohevt(),      //check it is properly filled
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
		        Epos::c2evt().fglevt(),  // defined only if phimin=phimax=0. // -1
		        Epos::hadr5().sigine()*1e9); //required in pB
    
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
   
     theEvent->set_signal_process_id(sig_id);  
     //set event number
     theEvent->set_event_number(Epos::accum().nrevt()); 
     //set default vertex
     HepMC::GenVertex* vertex = new HepMC::GenVertex(HepMC::FourVector(0,0,0,0));
     theEvent->add_vertex(vertex);

     //adapt the hepmcstore epos subroutine in a less complex way     
     double pprojin = 0.0;
     double ptargin = 0.0;
     int idpdg;
     int idprin;
     int idtgin;
     float amass;
     int id;

     //first loop over particles to prepare the beam particles
     for(int i = 1; i <= Epos::cptl().nptl(); i++){
         //prepare beam momenta
       if(i<=Epos::nucl1().maproj()){pprojin=pprojin + (double)(Epos::cptl().pptl(3,i));}
       else if(i<=(Epos::nucl1().maproj() + Epos::nucl1().matarg())){ptargin = ptargin + (double)(Epos::cptl().pptl(3,i));}
     }

     //beam particles
     HepMC::GenParticle* bpart[2]; 
     for(int a = 0; a < 2; a++ ){
     bpart[a] = new HepMC::GenParticle();
     }

     //other particles
     HepMC::GenParticle* gpart[200001]; //change not to have something hardcoded (should be nptl + the 2 beam particles)
     for(int a = 1; a < 200001; a++ ){
     gpart[a] = new HepMC::GenParticle();
     }

     //store initial target
     if(Epos::nucl1().maproj() > 1){
	  idprin = 1000000000 + (Epos::nucl1().maproj()*10) + (Epos::nucl1().laproj()*10000);
     } //end of if maproj > 1
     else{
	  idprin = Epos::hadr25().idprojin();
     }  //end of else maproj > 1
	
     id = idtrafo_((char *)"nxs", (char *)"pdg", &idprin,3,3); //needed ? should already be the pdg one?
     idmass_(&idprin,&amass);

     //store beam particle at the beggining of the HepMC file
     bpart[0]->set_pdg_id(id);
     bpart[0]->set_momentum(HepMC::FourVector(0.0, 0.0, pprojin,TMath::Sqrt((TMath::Power(pprojin,2))+(TMath::Power((double)(amass),2)))));  //here is GeV
     bpart[0]->set_generated_mass((double)(amass)); 
     bpart[0]->set_status(4); //in HepMC, beam particle status = 4
     bpart[0]->suggest_barcode(1);
     //main vertex is at (0,0,0)
     vertex->add_particle_in(bpart[0]);
    
     //target
     if(Epos::nucl1().matarg() > 1){
	  idtgin = 1000000000 + (Epos::nucl1().matarg()*10) + (Epos::nucl1().latarg()*10000);
     } //end of if TMath::nucl1().matarg > 1
     else{
	  idtgin = Epos::hadr25().idtargin();
     } //end of else TMath::nucl1().matarg() > 1
	 
     id =  idtrafo_((char *)"nxs", (char *)"pdg", &idtgin,3,3);
     idmass_(&idtgin,&amass);

     bpart[1]->set_pdg_id(id);
     bpart[1]->set_momentum(HepMC::FourVector(0.0, 0.0, ptargin,TMath::Sqrt((TMath::Power(ptargin,2))+(TMath::Power((double)(amass),2)))));  //here is GeV
     bpart[1]->set_generated_mass((double)(amass)); 
     bpart[1]->set_status(4); //in HepMC, beam particle status = 4
     bpart[1]->suggest_barcode(2);
     //main vertex is at (0,0,0)
     vertex->add_particle_in(bpart[1]);

     //before conversion of units
     //   bpart[0]->print();
     // bpart[1]->print();
       
     //set the beam particles inside the event (not sure it is needed since the particles were added to the default vertex)
     theEvent->set_beam_particles(bpart[0],bpart[1]);

     //give a new index to the particle in hepmc
     int barecode=3;
     // int countvertex = 1;
     //     HepMC::GenVertex* secondary_vertex[200000];


    
 
  
     //loop over particles

     for(int i = 1; i <= Epos::cptl().nptl(); i++){    	 

       //  printf("i=%i, fa=%i,mo=%i, dau1=%i, dau2=%i, stat=%i\n",i,Epos::cptl().iorptl(i),Epos::cptl().jorptl(i),Epos::cptl().ifrptl(1,i),Epos::cptl().ifrptl(2,i),Epos::cptl().istptl(i));

       //skip non final particles
       if((Epos::cptl().istptl(i) + 1) == 1){ //last generation has status 0 in Epos and +1 in HepMC 
       //convert the id of epos to the pdg id
       idpdg = idtrafo_((char *)"nxs", (char *)"pdg", &Epos::cptl().idptl(i),3,3); //convert the id of the particle from epos id to pdg id 
       gpart[i]->set_pdg_id(idpdg);
       gpart[i]->set_momentum(HepMC::FourVector((double)(Epos::cptl().pptl(1,i)), (double)(Epos::cptl().pptl(2,i)), (double)(Epos::cptl().pptl(3,i)), (double)(Epos::cptl().pptl(4,i))));  //here is GeV
       gpart[i]->set_generated_mass((double)(Epos::cptl().pptl(5,i))); //here is GeV
       gpart[i]->set_status(TMath::Min(2,Epos::cptl().istptl(i)+1)); //here is hepmc status
       //in the first loop we only set all particles with proper barecode
       gpart[i]->suggest_barcode(barecode);
       barecode++;
       vertex->add_particle_out(gpart[i]);
       } //end of if
     } //end of for

     
  //don't forget to rescale the units to MeV (if units where GeV)

  CRMCWrapper *wrap = new CRMCWrapper();
  wrap->convert_to_mev_and_mm(theEvent);

  //after conversion of units
  /*  printf("after conversion of units");
  bpart[0]->print();
  bpart[1]->print();
  gpart[1]->print();
  gpart[2]->print();*/

  return theEvent;

}

// EOF
