// Gaudi.
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// Event.
#include "Event/CrossSectionsFSR.h"
#include "Event/GenCollision.h"
#include "Event/GenCountersFSR.h"
#include "Event/GenFSR.h"
#include "Event/GenFSRMTManager.h"

// Generators.
#include "GenInterfaces/IBeamTool.h"

// HepMC.
#include "HepMC3/GenEvent.h"
#include "HepMC3/HEPEVT_Wrapper.h"

// LbPythia8.
#include "LbPythia8/Pythia8ProductionMT.h"

// HepMC conversion
#include "Defaults/HepMCAttributes.h"
#include "HepMCUser/Status.h"
#include "HepMC3/Attribute.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "Pythia8HepMC3/Pythia8ToHepMC3.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandomEngine.h"

//-----------------------------------------------------------------------------
// Implementation file for class: Pythia8ProductionMT
//
// 2007-07-31 : Arthur de Gromard, Philip Ilten
//-----------------------------------------------------------------------------

std::mutex Pythia8ProductionMT::m_pythia_lock{};
//=============================================================================
// Default constructor.
//=============================================================================
Pythia8ProductionMT::Pythia8ProductionMT(const std::string& type,
				     const std::string& name,
                                     const IInterface* parent)
    : GaudiTool( type, name, parent )
    , m_pythia( 0 )
    , m_hooks( 0 )
    , m_lhaup( 0 )
    , m_beamTool( 0 )
    , m_pythiaBeamTool( 0 )
    , m_nEvents( 0 )
    , m_showBanner( false )
    , m_xmlLogTool( 0 )
{

  // Declare the tool properties.
  declareInterface<IProductionTool>( this );
  declareProperty( "Commands", m_userSettings, "List of commands to pass to Pythia 8." );
  declareProperty( "BeamToolName", m_beamToolName = "CollidingBeams", "The beam tool to use." );
  declareProperty( "ValidateHEPEVT", m_validate_HEPEVT = false, "Flag to validate the Pythia 8 event record." );
  declareProperty( "ListAllParticles", m_listAllParticles = false, "List all the particles being used by Pythia 8." );
  declareProperty( "CheckParticleProperties", m_checkParticleProperties = false,
                   "Check the particle properties for consistency." );
  declareProperty( "Tuning", m_tuningFile = "LHCbDefault.cmd", "Name of the tuning file to use." );
  declareProperty( "UserTuning", m_tuningUserFile = "",
                   "Name of the user tuning file to use. Using the tune subrun "
                   "will overwrite the default LHCb tune." );
  declareProperty( "ShowBanner", m_showBanner = false, "Flag to print the Pythia 8 banner at initialization." );
  declareProperty( "NThreads", m_nThreads = 1, "Delay the initialisation" );
  declareProperty ( "GenFSRLocation", m_FSRName =
                    LHCb::GenFSRLocation::Default);

  // Set the special particles.
  for ( int i = 1; i <= 8; ++i ) m_special.insert( i );
  for ( int i = 33; i <= 34; ++i ) m_special.insert( i );
  for ( int i = 41; i <= 44; ++i ) m_special.insert( i );
  for ( int i = 81; i <= 85; ++i ) m_special.insert( i );
  for ( int i = 88; i <= 99; ++i ) m_special.insert( i );
  m_special.insert( 17 );
  m_special.insert( 1103 );
  m_special.insert( 4301 );
  m_special.insert( 18 );
  m_special.insert( 2101 );
  m_special.insert( 4303 );
  m_special.insert( 21 );
  m_special.insert( 2103 );
  m_special.insert( 4401 );
  m_special.insert( 110 );
  m_special.insert( 2201 );
  m_special.insert( 4403 );
  m_special.insert( 990 );
  m_special.insert( 2203 );
  m_special.insert( 5101 );
  m_special.insert( 37 );
  m_special.insert( 3101 );
  m_special.insert( 5103 );
  m_special.insert( 39 );
  m_special.insert( 3103 );
  m_special.insert( 5201 );
  m_special.insert( 9900110 );
  m_special.insert( 3201 );
  m_special.insert( 5203 );
  m_special.insert( 9900210 );
  m_special.insert( 3203 );
  m_special.insert( 5301 );
  m_special.insert( 9900220 );
  m_special.insert( 3301 );
  m_special.insert( 5303 );
  m_special.insert( 9900330 );
  m_special.insert( 3303 );
  m_special.insert( 5401 );
  m_special.insert( 9900440 );
  m_special.insert( 4101 );
  m_special.insert( 5403 );
  m_special.insert( 9902110 );
  m_special.insert( 4103 );
  m_special.insert( 5501 );
  m_special.insert( 9902210 );
  m_special.insert( 4201 );
  m_special.insert( 5503 );
  m_special.insert( 1101 );
  m_special.insert( 4203 );
  m_special.insert( 1000022 );
  m_special.insert( 1000024 );
}

//=============================================================================
// Default destructor.
//=============================================================================
Pythia8ProductionMT::~Pythia8ProductionMT() {}

//=============================================================================
// Initialize the tool.
//=============================================================================
StatusCode Pythia8ProductionMT::initialize()
{

  // Print the initialization banner.
  always() << "============================================================="
           << "=====" << endmsg;
  always() << "Using as production engine " << this->type() << endmsg;
  always() << "============================================================="
           << "=====" << endmsg;

  // Initialize the Gaudi tool.
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) Exception( "Failed to initialize the Gaudi tool." );

  // Initialize the beam tool.
  m_beamTool = tool<IBeamTool>( m_beamToolName, this );
  if ( !m_beamTool ) Exception( "Failed to initialize the IBeamTool." );

  // Initialze the XML log file.
  m_xmlLogTool = tool<ICounterLogFile>( "XmlCounterLogFile" );
  m_manager    = new Pythia8ThreadManager{};

  return sc;
}

//=============================================================================
// Initialize the Pythia 8 generator.
//=============================================================================
StatusCode Pythia8ProductionMT::initializeGenerator()
{
  if ( !m_pythia() ) {
    debug() << "Skipping generator initialization for this thread" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Initialize the external pointers.
  m_pythia->setBeamShapePtr( m_pythiaBeamTool() );
  if ( m_hooks() ) m_pythia->setUserHooksPtr( m_hooks() );
  if ( m_lhaup() ) m_pythia->setLHAupPtr( m_lhaup() );

  // Set the beam configuration.
  Gaudi::XYZVector beamA, beamB;
  m_beamTool->getMeanBeams( beamA, beamB );
  beamA /= Gaudi::Units::GeV;
  beamB /= Gaudi::Units::GeV;
  m_pythia->settings.parm( "Beams:pxA", beamA.X() );
  m_pythia->settings.parm( "Beams:pyA", beamA.Y() );
  m_pythia->settings.parm( "Beams:pzA", beamA.Z() );
  m_pythia->settings.parm( "Beams:pxB", beamB.X() );
  m_pythia->settings.parm( "Beams:pyB", beamB.Y() );
  m_pythia->settings.parm( "Beams:pzB", beamB.Z() );

  // Read the tuning files.
  if ( "UNKNOWN" != System::getEnv( "LBPYTHIA8ROOT" ) &&
       m_pythia->readFile( System::getEnv( "LBPYTHIA8ROOT" ) + "/options/" + m_tuningFile ) )
    ;
  else
    Warning( "Failed to find $LBPYTHIA8ROOT/options/" + m_tuningFile + ", using default options." );
  if ( m_tuningUserFile != "" && !m_pythia->readFile( m_tuningUserFile ) )
    Warning( "Failed to find " + m_tuningUserFile + "." );

  // Turn off minimum bias if using LHAup.
  if ( m_lhaup() ) {
    std::vector<string> procs;
    procs.push_back( "SoftQCD:" );
    procs.push_back( "HardQCD:" );
    procs.push_back( "Onia:" );
    procs.push_back( "Charmonium:" );
    procs.push_back( "Bottomonium:" );
    for ( unsigned int proc = 0; proc < procs.size(); ++proc ) {
      std::map<string, Pythia8::FVec> fvecs = m_pythia->settings.getFVecMap( procs[proc] );
      std::map<string, Pythia8::Flag> flags = m_pythia->settings.getFlagMap( procs[proc] );
      for ( std::map<string, Pythia8::FVec>::iterator itr = fvecs.begin(); itr != fvecs.end(); ++itr )
        m_pythia->settings.fvec( itr->first, std::vector<bool>( itr->second.valNow.size(), false ) );
      for ( std::map<string, Pythia8::Flag>::iterator itr = flags.begin(); itr != flags.end(); ++itr )
        m_pythia->settings.flag( itr->first, false );
    }
  }

  // Read user settings.
  for ( unsigned int setting = 0; setting < m_userSettings.size(); ++setting ) {
    debug() << m_userSettings[setting] << endmsg;
    if ( !m_pythia->readString( m_userSettings[setting] ) )
      Warning( "Failed to read the command " + m_userSettings[setting] + "." );
  }

  // Check particle properties if requested.
  if ( m_checkParticleProperties ) {
    int id = m_pythia->particleData.nextId( 0 );
    while ( id != 0 ) {
      if ( !m_pythia->particleData.hasChanged( id ) )
        warning() << "Data for particle with ID " << id << " and name " << m_pythia->particleData.name( id )
                  << " has not been changed." << endmsg;
      id = m_pythia->particleData.nextId( id );
    }
  }
  
  // Check the Breit-Wigner mass thresholds.
  for (std::set<int>::iterator id = m_bws().begin(); id != m_bws().end(); ++id) {
    Pythia8::ParticleDataEntry *pde = 
      m_pythia->particleData.particleDataEntryPtr(*id);
    if (pde->isResonance()) continue;
    pde->initBWmass();
    if (pde->useBreitWigner()) continue;
    double mThr(0), bRatSum(0), mThrSum(0);
    for (int i = 0; i < int(pde->sizeChannels()); ++i)
      if (pde->channel(i).onMode() > 0) {
	bRatSum += pde->channel(i).bRatio();
	double mChannelSum = 0.;
	for (int j = 0; j < pde->channel(i).multiplicity(); ++j)
	  mChannelSum += m_pythia->particleData.m0(pde->channel(i).product(j));
	mThrSum += pde->channel(i).bRatio() * mChannelSum;
      }
    mThr = (bRatSum == 0.) ? 0. : mThrSum / bRatSum;
    if (mThr > pde->m0()) {
      warning() << "The threshold mass for particle with ID " << *id << " is "
		<< mThr << " GeV but its nominal mass is " << pde->m0() 
		<< " GeV; clearing its decay channels." << endmsg;
      pde->clearChannels();
    }
  }

  // Initialize.
  if (m_lhaup.get()) m_pythia->settings.mode("Beams:frameType", 5);
  if ( m_pythia->init() )
    return StatusCode::SUCCESS;
  else
    return Error( "Failed to initialize Pythia 8." );
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode Pythia8ProductionMT::generateEvent( HepMC3::GenEvent* theEvent, LHCb::GenCollision* theCollision,
                                               HepRandomEnginePtr& engine ) const
{
  if ( !m_pythia() ) {
    debug() << "Initializing Pythia8 in thread!" << endmsg;
    // This is supposed to only affect thread-local variables so while
    // not technically constant it is marked as such to be called here
    auto sc = InitializeThread();
    if(sc.isFailure()){
      return sc;
    }
  }

  class RndForPythia : public Pythia8::RndmEngine
  {
  public:
    RndForPythia( CLHEP::HepRandomEngine& engine ) : m_gen( engine, 0, 1 ) {}
    virtual double flat() { return m_gen(); }

  private:
    CLHEP::RandFlat m_gen;
  };

  auto pythia = m_pythia();
  RndForPythia rnd_generator{engine.getref()};
  pythia->setRndmEnginePtr( &rnd_generator );
  // Generate the event (make 10 attempts).
  int tries( 0 );
  while ( !pythia->next() && tries < 10 ) ++tries;
  if ( tries == 10 ) return Error( "Pythia 8 event generation failed 10 times." );
  if ( !m_pythia->flag( "HadronLevel:all" ) ) m_event = pythia->event;
  ++m_nEvents;

  LHCb::GenFSR* genFSR{nullptr};
  if(m_FSRName != ""){
    genFSR = GenFSRMTManager::GetGenFSR();
  }

  // Store the minimum bias cross-section in the GenFSR.
  std::vector<int> codes = m_pythia->info.codesHard();
  int key = LHCb::CrossSectionsFSR::MBCrossSection;
  if (genFSR && genFSR->hasGenCounter(key+100)) {
    longlong count = genFSR->getGenCounterInfo(100 + key).second;
    count = m_pythia->info.nAccepted(key) - count;
    if (count > 0) genFSR->incrementGenCounter(key + 100, count); 
  } else if (m_pythia->info.nAccepted(key) != 0 && genFSR)
    genFSR->addGenCounter(100 + key, m_pythia->info.nAccepted(key));
  if (genFSR && genFSR->hasCrossSection(key)) genFSR->eraseCrossSection(key);
  if(genFSR) genFSR->addCrossSection
    (key, LHCb::GenFSR::CrossValues("Total cross-section", 
				    m_pythia->info.sigmaGen(key)));

  // Store the others cross-sections in the GenFSR.
  for (unsigned int code = 0; code < codes.size(); ++code) {
    key = codes[code];
    if (genFSR && genFSR->hasGenCounter(key + 100)) {
      longlong count = genFSR->getGenCounterInfo(100 + key).second;
      count = m_pythia->info.nAccepted(key) - count;
      if (count > 0) genFSR->incrementGenCounter(key + 100, count);
    } else if (m_pythia->info.nAccepted(key) != 0 && genFSR)
      genFSR->addGenCounter(100 + key, m_pythia->info.nAccepted(key));
    if (genFSR && genFSR->hasCrossSection(key)) genFSR->eraseCrossSection(key);
    if(genFSR) genFSR->addCrossSection
      (key, LHCb::GenFSR::CrossValues(m_pythia->info.nameProc(key), 
				      m_pythia->info.sigmaGen(key)));    
  }

  // Convert the event to HepMC and return.
  if ( theCollision->isSignal() || pythia->flag( "HadronLevel:all" ) )
    return toHepMC( theEvent, theCollision );
  else
    return StatusCode::SUCCESS;
}

//=============================================================================
// Convert the Pythia 8 event to HepMC format.
//=============================================================================
StatusCode Pythia8ProductionMT::toHepMC(HepMC3::GenEvent* theEvent, 
				      LHCb::GenCollision* theCollision) const {

  // Convert to HepMC.
  HepMC3::Pythia8ToHepMC3 conversion;
  conversion.set_print_inconsistency(m_validate_HEPEVT);
  if (!(conversion.fill_next_event(*m_pythia(), theEvent))) 
    return Error("Failed to convert Pythia 8 event to HepMC.");
  // Convert status codes and IDs.
  for ( auto& p : theEvent->particles() ) {
    int status = p->status();
    int pid    = p->pdg_id();
    if ( status > 3 ) {
      if ( status == 21 ) {
        //(*p)->set_status(LHCb::HepMCEvent::PythiaIncomingParton);
        p->set_status( 21 );
      } else if ( ( status > 21 && status < 30 )    // part of hard process
                  || ( status > 40 && status < 50 ) // ISR
                  || ( status > 50 && status < 60 ) // FSR
      ) {
        // p->set_status(LHCb::HepMCEvent::PythiaHardProcess);
        p->set_status( 22 );
      } else if ( ( status == 71 ) || ( status == 72 ) ||
                  ( ( status == 62 ) && ( abs( pid ) >= 22 ) && ( abs( pid ) <= 37 ) ) )
        p->set_status( HepMC3::Status::DecayedByProdGen );
      else
        p->set_status( HepMC3::Status::DocumentationParticle );
    } else if ( status != HepMC3::Status::DecayedByProdGen && status != HepMC3::Status::StableInProdGen &&
                status != HepMC3::Status::DocumentationParticle )
      warning() << "Unknown status rule " << status << " for particle" << pid << endmsg;
  }

  // Convert to LHCb units.
  for (auto & v:theEvent->vertices())
    v->set_position(HepMC3::FourVector
		       (v->position().x(), v->position().y(),
			v->position().z(), 
			(v->position().t() * Gaudi::Units::mm) 
			/ Gaudi::Units::c_light));

  // Set the process and collision info.
  int code(m_pythia->info.hasSub() ? m_pythia->info.codeSub() : 
	   m_pythia->info.code());
  theEvent->add_attribute(Gaussino::HepMC::Attributes::SignalProcessID, std::make_shared<HepMC3::IntAttribute>(code));
  theCollision->setProcessType(code);
  theCollision->setSHat(m_pythia->info.sHat());
  theCollision->setTHat(m_pythia->info.tHat());
  theCollision->setUHat(m_pythia->info.uHat());
  theCollision->setPtHat(m_pythia->info.pTHat());
  theCollision->setX1Bjorken(m_pythia->info.x1());
  theCollision->setX2Bjorken(m_pythia->info.x2());
  return StatusCode::SUCCESS;
}

//=============================================================================
// Set a particle stable.
//=============================================================================
void Pythia8ProductionMT::setStable( const LHCb::ParticleProperty* thePP ) { m_stable_pp.push_back( thePP ); }

void Pythia8ProductionMT::setStableImpl( const LHCb::ParticleProperty* thePP ) const
{
  m_pythia->particleData.mayDecay( pythia8Id( thePP ), false );
}

//=============================================================================
// Update a particle.
//=============================================================================
void Pythia8ProductionMT::updateParticleProperties( const LHCb::ParticleProperty* thePP )
{
  m_update_pp.push_back( thePP );
}
void Pythia8ProductionMT::updateParticlePropertiesImpl( const LHCb::ParticleProperty* thePP ) const
{

  // Create the particle if needed.
  int id                    = pythia8Id( thePP );
  string name               = thePP->name();
  Pythia8::ParticleData& pd = m_pythia->particleData;
  if ( id == 0 ) {
    const LHCb::ParticleID pid = thePP->pid();
    id                         = pid.pid();
    if ( id < 0 )
      if ( pd.isParticle( abs( id ) ) )
        pd.names( abs( id ), pd.name( abs( id ) ), name );
      else
        pd.addParticle( id, "void", name, pid.sSpin(), -3 * thePP->charge() );
    else
      pd.addParticle( id, name, pid.sSpin(), 3 * thePP->charge() );
    id = pythia8Id( thePP );
    if ( id == 0 ) return;
  }
  if ( pd.name( id ) == "void" ) pd.name( id, name );

  // Set the mass, width and lifetime (only non-resonant).
  pd.m0(id, thePP->mass() / Gaudi::Units::GeV);
  if (id == 6 || (id >= 23 && id <= 37)) return;
  double lifetime = thePP->lifetime()*Gaudi::Units::c_light;
  if (lifetime >= 1.e16 * Gaudi::Units::mm) lifetime = 0;
  double width = lifetime == 0 ? 0 : Gaudi::Units::hbarc / lifetime;
  double min_mass = (thePP->mass() - (thePP->maxWidth() ? thePP->maxWidth() : 
                15*width)) / Gaudi::Units::GeV;
  // Ensure that minimal mass is not negative
  if(min_mass < 0){
    min_mass = 0;
  }
  pd.mMin(id, min_mass); 
  pd.mMax(id, (thePP->mass() + 15*width) / Gaudi::Units::GeV);
  pd.mWidth(id, width / Gaudi::Units::GeV);
  pd.tau0(id, lifetime / Gaudi::Units::mm);
}

//=============================================================================
// Turn on and off fragmentation.
//=============================================================================
void Pythia8ProductionMT::turnOnFragmentation() { m_pythia->settings.flag( "HadronLevel:Hadronize", true ); }

void Pythia8ProductionMT::turnOffFragmentation() { m_pythia->settings.flag( "HadronLevel:Hadronize", false ); }

//=============================================================================
// Hadronize an event.
//=============================================================================
StatusCode Pythia8ProductionMT::hadronize( HepMC3::GenEvent* theEvent, LHCb::GenCollision* theCollision )
{
  if ( !m_pythia->forceHadronLevel() ) return StatusCode::FAILURE;
  return toHepMC( theEvent, theCollision );
}

//=============================================================================
// Save the Pythia 8 event record.
//=============================================================================
void Pythia8ProductionMT::savePartonEvent( HepMC3::GenEvent* /*theEvent*/ ) { m_event = m_pythia->event; }

//=============================================================================
// Retrieve the Pythia 8 event record.
//=============================================================================
void Pythia8ProductionMT::retrievePartonEvent( HepMC3::GenEvent* /*theEvent*/ ) { m_pythia->event = m_event(); }

//=============================================================================
// Print the running conditions.
//=============================================================================
void Pythia8ProductionMT::printRunningConditions() const
{
  if ( !m_pythia() ) return;
  if ( m_nEvents == 0 && m_listAllParticles == true && msgLevel( MSG::DEBUG ) ) m_pythia->particleData.listAll();
  if ( msgLevel( MSG::VERBOSE ) )
    m_pythia->settings.listAll();
  else if ( msgLevel( MSG::DEBUG ) )
    m_pythia->settings.listChanged();
}

//=============================================================================
// Return whether a particle has special status.
//=============================================================================
bool Pythia8ProductionMT::isSpecialParticle( const LHCb::ParticleProperty* thePP ) const
{
  return m_special.find( thePP->pid().abspid() ) != m_special.end();
}

//=============================================================================
// Setup forced fragmentation.
//=============================================================================
StatusCode Pythia8ProductionMT::setupForcedFragmentation( const int /*thePdgId*/ )
{
  m_pythia->settings.flag( "PartonLevel:all", false );
  return StatusCode::SUCCESS;
}

//=============================================================================
// Return the Pythia 8 ID.
//=============================================================================
int Pythia8ProductionMT::pythia8Id( const LHCb::ParticleProperty* thePP ) const
{
  int id( thePP->pid().pid() );
  if ( abs( id ) == 30221 ) return id > 0 ? 10221 : -10221;
  if ( abs( id ) == 104124 ) return id > 0 ? 4124 : -4124;
  if ( m_pythia->particleData.isParticle( id ) ) return id;
  return 0;
}

StatusCode Pythia8ProductionMT::InitializeThread() const
{
  debug() << "Initializing Pythia8 in thread" << endmsg;
  // Initialize the user hooks.
  if ( !m_hooks.get() ) m_hooks = new Pythia8::LhcbHooks();

  // Create the Pythia 8 generator.
  string xmlpath( "UNKNOWN" != System::getEnv( "PYTHIA8XML" ) ? System::getEnv( "PYTHIA8XML" ) : "" );
  m_pythia = new Pythia8::Pythia( xmlpath, m_showBanner );
  if ( !m_pythia.get() ) return StatusCode::FAILURE;

  // Create the Breit-Wigner map for checking particle widths later.
  m_bws().clear();
  int id = m_pythia->particleData.nextId(1);
  while (id != 0) {
    if (!m_pythia->particleData.isResonance(id)) {
      m_pythia->particleData.particleDataEntryPtr(id)->initBWmass();
      if (m_pythia->particleData.useBreitWigner(id)) m_bws().insert(id);
    }
    id = m_pythia->particleData.nextId(id);
  }

  // Add LhcbHooks parameters.
  Pythia8::Settings& set = m_pythia->settings;
  string sm( "StandardModel:" ), mpi( "MultiPartonInteractions:" ), pre( "LhcbHooks:" ), parm( "pT0Ref" );
  set.addParm( pre + parm, set.parm( mpi + parm ), false, false, 0, 0 );
  parm = "ecmRef";
  set.addParm( pre + parm, set.parm( mpi + parm ), false, false, 0, 0 );
  parm = "ecmPow";
  set.addParm( pre + parm, set.parm( mpi + parm ), false, false, 0, 0 );
  parm = "alphaSvalue";
  set.addParm( pre + parm, set.parm( mpi + parm ), false, false, 0, 0 );
  parm = "alphaSorder";
  set.addMode( pre + parm, set.mode( mpi + parm ), false, false, 0, 0 );
  parm = "alphaSnfmax";
  set.addMode( pre + parm, set.mode( sm + parm ), false, false, 0, 0 );

  StatusCode sc;
  // Initialize the Pythia beam tool.
  m_pythiaBeamTool = new BeamToolForPythia8( m_beamTool, m_pythia->settings, sc );
  if ( !sc.isSuccess() ) return Error( "Failed to initialize the BeamToolForPythia8." );

  // Now the normal tools update settings via the provided interface
  // As we need to do this per pythia instance once created within
  // the thread we have cached the arguments for the function call
  // and will now do it ourselfs
  for ( auto pp : m_update_pp ) {
    updateParticlePropertiesImpl( pp );
  }
  for ( auto pp : m_stable_pp ) {
    setStableImpl( pp );
  }

  // Now initialize the generator and hope for the best!
  const_cast<Pythia8ProductionMT*>(this)->initializeGenerator();
  if ( m_first_init ) {
    printRunningConditions();
    m_first_init = false;
  } else {
    GetInitBarrier( m_nThreads - 1 ).wait();
    std::call_once( m_init_flag, [&]() { info() << "All Pythia8 instances initialised" << endmsg; } );
  }

  // This is just a dumb hack to clean up after the threads
  // terminate...
  // The Garbage bins hold a static store which should be
  // destroyed at the end of the execution.
  // GarbageBin<Pythia8::Pythia*>::Add( m_pythia() );
  // GarbageBin<Pythia8::UserHooks*>::Add( m_hooks() );
  // GarbageBin<Pythia8::LHAup*>::Add( m_lhaup() );
  // GarbageBin<BeamToolForPythia8*>::Add( m_pythiaBeamTool() );

  // Push this into the manager for later merging and cleanup of the used pythia instances
  std::lock_guard<std::mutex> l{m_pythia_lock};
  m_manager->store.emplace_back( m_pythia(), m_hooks(), m_lhaup(), m_pythiaBeamTool() );
  return StatusCode::SUCCESS;
}

StatusCode Pythia8ProductionMT::finalize()
{
  // Print the statistics.
  for ( auto[pythia, hooks, lhaup, beam] : m_manager->store ) {
    pythia->stat();

    // Write the cross-sections to the XML log.
    std::vector<int> codes = pythia->info.codesHard();
    for ( unsigned int code = 0; code < codes.size(); ++code )
      m_xmlLogTool->addCrossSection( pythia->info.nameProc( codes[code] ), codes[code],
                                     pythia->info.nAccepted( codes[code] ), pythia->info.sigmaGen( codes[code] ) );
  }

  // Clean up.
  delete m_manager;
  return GaudiTool::finalize();
}

//=============================================================================
// The END.
//=============================================================================
