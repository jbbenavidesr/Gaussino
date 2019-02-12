// $Id: ParticleGun.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
// Include files

// local
#include "ParticleGun.h"

// from SEAL
#include "boost/tokenizer.hpp"

// from Event
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"

// from LHCb
#include "Kernel/ParticleID.h"

// from Generators
#include "GenInterfaces/IPileUpTool.h"
#include "LbPGuns/IParticleGunTool.h"
#include "GenInterfaces/IDecayTool.h"
#include "GenInterfaces/IVertexSmearingTool.h"
#include "GenInterfaces/IFullGenEventCutTool.h"
#include "GenInterfaces/IGenCutTool.h"
#include "Generators/GenCounters.h"
#include "GenEvent/HepMCUtils.h"

#include "HepMCUser/VertexAttribute.h"
#include "Defaults/HepMCAttributes.h"

#include "CLHEP/Random/RandFlat.h"
#include "NewRnd/RndGlobal.h"

#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/EventContext.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleGun
//
// 2008-05-18 : Patrick Robbe
// 2016-03-01 : Dan Johnson [adding mass-sampling]
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_COMPONENT( ParticleGun )


//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ParticleGun::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize( ) ; // Initialize base class
  if ( sc.isFailure() ) return sc ;

  debug() << "==> Initialise" << endmsg ;

  // Retrieve pile up tool
  if ( "" != m_numberOfParticlesToolName )
    m_numberOfParticlesTool = tool< IPileUpTool >( m_numberOfParticlesToolName , this ) ;

  // Retrieve decay tool
  if ( "" != m_decayToolName ) {
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;
    if ( m_decayTool && m_sigPdgCode!=0 ) m_decayTool -> setSignal( m_sigPdgCode ) ;
  }

  // If trying to sample mass but meaningless mass range, throw an error
  if (m_sampleMass) {
    if (m_MassRange_min<0. || m_MassRange_max<0. || m_MassRange_min > m_MassRange_max )  {
      debug() << "==> Min: " << m_MassRange_min << endmsg ;
      debug() << "==> Max: " << m_MassRange_max << endmsg ;
      return Error( "You asked to sample the particle mass, but defined a meaningless mass range" ) ;
    }
  }  

  // Retrieve generation method tool
  if ( "" == m_particleGunToolName )
    return Error( "No ParticleGun Generation Tool is defined. This is mandatory" ) ;
  m_particleGunTool =
    tool< IParticleGunTool >( m_particleGunToolName , this ) ;

  boost::char_separator<char> sep(".");
  boost::tokenizer< boost::char_separator<char> >
    strList( m_particleGunTool -> name() , sep ) ;

  std::string result = "" ;
  for ( boost::tokenizer< boost::char_separator<char> >::iterator
          tok_iter = strList.begin();
        tok_iter != strList.end(); ++tok_iter)
    result = (*tok_iter) ;
  m_particleGunName = result ;

  // Retrieve generation method tool
  if ( "" != m_vertexSmearingToolName )
    m_vertexSmearingTool =
      tool< IVertexSmearingTool >( m_vertexSmearingToolName , this ) ;

  // Retrieve full gen event cut tool
  if ( "" != m_fullGenEventCutToolName ) m_fullGenEventCutTool =
                                           tool< IFullGenEventCutTool >( m_fullGenEventCutToolName , this ) ;

  // Retrieve gen cut tool
  if ( "" != m_genCutToolName ) m_genCutTool =
                                           tool< IGenCutTool >( m_genCutToolName , this ) ;

  // Message relative to event type
  always()
    << "=================================================================="
    << endmsg;
  always()
    << "Requested to generate EventType " << m_eventType << endmsg;
  always()
    << "=================================================================="
    << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
std::tuple<std::vector<HepMC::GenEvent>, LHCb::GenCollisions, LHCb::GenHeader>
ParticleGun::operator()( const LHCb::GenHeader& theOldGenHeader ) const {

  debug() << "Processing event type " << m_eventType << endmsg ;
  auto engine = createRndmEngine();
  ThreadLocalEngine::Guard guard(engine);
  // Create a flat random generator to replace RandomForGenerator singleton
  CLHEP::RandFlat flatGenerator{*engine.get(), 0, 1};
  StatusCode sc = StatusCode::SUCCESS ;

  // Get the header and update the information
  LHCb::GenHeader theGenHeader = theOldGenHeader;
  if( !theGenHeader.evType() ){
    theGenHeader.setEvType( m_eventType );
  }

  unsigned int  nParticles( 0 ) ;

  // Create temporary containers for this event
  std::vector<HepMC::GenEvent> theEvents{};
  LHCb::GenCollisions theCollisions{};

  // Working set of pointers
  LHCb::GenCollision * theGenCollision{nullptr};
  HepMC::GenEvent * theGenEvent{nullptr};

  Gaudi::LorentzVector theFourMomentum{};
  Gaudi::LorentzVector origin{};
  int thePdgId ;

  // Generate sets of particles until a good one is found
  bool goodEvent = false ;

  while ( ! goodEvent ) {
    theEvents.clear() ;
    theCollisions.clear() ;

    // Compute the number of pile-up interactions to generate
    if ( 0 != m_numberOfParticlesTool )
      nParticles = m_numberOfParticlesTool -> numberOfPileUp( engine ) ;
    // default set to 1 pile and 0 luminosity
    else nParticles = 1 ;

    // generate a set of particles according to the requested type
    // of particle gun
    theEvents.reserve(nParticles);
    for ( unsigned int i = 0 ; i < nParticles ; ++i ) {
      // Prepare event container
      prepareInteraction( &theEvents , &theCollisions , theGenEvent , theGenCollision ) ;
      theGenEvent->add_attribute(Gaussino::HepMC::Attributes::GaudiEventNumber, std::make_shared<HepMC::IntAttribute>(Gaudi::Hive::currentContext().evt()));
      theGenEvent->add_attribute(Gaussino::HepMC::Attributes::GaudiRunNumber, std::make_shared<HepMC::IntAttribute>(Gaudi::Hive::currentContext().eventID().run_number()));

      // If sampling the mass, change the energy of the particle appropriately
      if (m_sampleMass) {
        double massToGenerate = m_MassRange_min + flatGenerator() * (m_MassRange_max-m_MassRange_min) ;
        double energy = sqrt( massToGenerate * massToGenerate + theFourMomentum.P() * theFourMomentum.P() ) ;
        theFourMomentum.SetE( energy ) ;
      }

      // generate one particle
      m_particleGunTool -> generateParticle( theFourMomentum , origin , thePdgId , engine );

      // create HepMC Vertex
      HepMC::GenVertex * v =
        new HepMC::GenVertex( HepMC::FourVector( origin.X() ,
                                                 origin.Y() ,
                                                 origin.Z() ,
                                                 origin.T() ) ) ;
      // create HepMC particle
      HepMC::GenParticle * p =
        new HepMC::GenParticle( HepMC::FourVector( theFourMomentum.Px() ,
                                                   theFourMomentum.Py() ,
                                                   theFourMomentum.Pz() ,
                                                   theFourMomentum.E()  ) ,
                                thePdgId ,
                                LHCb::HepMCEvent::StableInProdGen ) ;

      v -> add_particle_out( p ) ;
      theGenEvent->add_vertex( v ) ;
      theGenEvent->add_attribute(Gaussino::HepMC::Attributes::SignalProcessID,
          std::make_shared<HepMC::IntAttribute>(nParticles));
      theGenEvent->add_attribute(Gaussino::HepMC::Attributes::SignalProcessVertex,
          std::make_shared<HepMC::VertexAttribute>(v));
    }

    goodEvent = true ;
    // increase event and interactions counters
    ++m_nEvents ;    m_nParticles += nParticles ;

    // Decay the event if it is a good event
    if ( 0 != m_decayTool ) {
      unsigned short iPart{0};
      for ( auto & event : theEvents ) {
        ParticleVector theParticleList ;
        theParticleList.clear();

        auto theSignal = decayEvent( &event, theParticleList, engine, sc) ;
        if ( ! sc.isSuccess() ) error() << "Failed to decay event" << endmsg;

        event.set_event_number(++iPart);

        // Add Cut tool
        bool passCut(true);
        if ( m_genCutTool && theSignal ) {
          ++m_nBeforeCut;
          passCut = m_genCutTool -> applyCut( theParticleList , theGenEvent ,
                                              theGenCollision ) ;
          // event does not pass cuts
          if ( !passCut || theParticleList.empty() ){
            
            HepMCUtils::RemoveDaughters( theSignal ) ;
            goodEvent = false;
          }
          else ++m_nAfterCut;
        }
      }
    }

    // Apply smearing of primary vertex
    if ( 0 != m_vertexSmearingTool ) {
      for ( auto & event : theEvents ) {
        sc = m_vertexSmearingTool -> smearVertex( &event , engine ) ;
        if ( ! sc.isSuccess() ) error() << "Failed to smear event" << endmsg;
      }
    }

    // Apply generator level cut on full event
    if ( m_fullGenEventCutTool ) {
      if ( goodEvent ) {
        ++m_nBeforeFullEvent ;
        goodEvent = m_fullGenEventCutTool -> studyFullEvent( theEvents ,
                                                             theCollisions );
        if ( goodEvent ) ++m_nAfterFullEvent ;
      }
    }
  }

  // Check that number of HepMC events is the same as GenCollisions
  if( theEvents.size() != theCollisions.size() ) {
    error() << "Number of HepMCEvents and GenCollisions do not match" << endmsg;
  }

  return std::make_tuple(std::move(theEvents), std::move(theCollisions), std::move(theGenHeader));
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ParticleGun::finalize() {
  using namespace GenCounters ;
  debug( ) << "==> Finalize" << endmsg ;
  // Print the various counters
  info() << "**************************************************" << endmsg ;
  if ( 0 != m_numberOfParticlesTool )
    m_numberOfParticlesTool -> printPileUpCounters( ) ;
  info() << "***********   Generation counters   **************" << std::endl ;
  printCounter( info() , "generated events" , m_nEvents ) ;
  printCounter( info() , "generated particles" , m_nParticles ) ;

  printCounter( info() , "accepted events" , m_nAcceptedEvents ) ;
  printCounter( info() , "particles in accepted events" ,
                m_nAcceptedParticles ) ;

  printEfficiency( info() , "full event cut" , m_nAfterFullEvent ,
                   m_nBeforeFullEvent ) ;
  printEfficiency( info() , "signal cut" , m_nAfterCut ,
                   m_nBeforeCut ) ;
  info() << endmsg ;

  m_particleGunTool -> printCounters() ;

  if ( 0 != m_numberOfParticlesTool ) release( m_numberOfParticlesTool ) ;
  if ( 0 != m_decayTool ) release( m_decayTool ) ;
  if ( 0 != m_particleGunTool ) release( m_particleGunTool ) ;
  if ( 0 != m_vertexSmearingTool ) release( m_vertexSmearingTool ) ;
  if ( 0 != m_fullGenEventCutTool ) release( m_fullGenEventCutTool ) ;
  if ( 0 != m_genCutTool ) release( m_genCutTool ) ;

  return GaudiAlgorithm::finalize( ) ; // Finalize base class
}

//=============================================================================
// Decay in the event all particles which have been left stable by the
// production generator
//=============================================================================
HepMC::GenParticlePtr ParticleGun::decayEvent( HepMC::GenEvent * theEvent,
                                             ParticleVector & theParticleList,
                                             HepRandomEnginePtr & engine,
                                             StatusCode & sc) const {
  m_decayTool -> disableFlip() ;
  sc = StatusCode::SUCCESS ;
  HepMC::GenParticle *theSignal(0);

  // We must use particles_begin to obtain an ordered iterator of GenParticles
  // according to the barcode: this allows to reproduce events !
  HepMCUtils::ParticleSet pSet( theEvent -> particles_begin() ,
                                theEvent -> particles_end() ) ;

  HepMCUtils::ParticleSet::iterator itp ;

  for ( itp = pSet.begin() ; itp != pSet.end() ; ++itp ) {

    HepMC::GenParticle * thePart = (*itp) ;
    unsigned int status = thePart -> status() ;

    if ( ( LHCb::HepMCEvent::StableInProdGen  == status ) ||
         ( ( LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen == status )
           && ( 0 == thePart -> end_vertex() ) ) ) {

      if ( m_decayTool -> isKnownToDecayTool( thePart -> pdg_id() ) ) {

        if ( LHCb::HepMCEvent::StableInProdGen == status )
          thePart ->
            set_status( LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen ) ;
        else thePart -> set_status( LHCb::HepMCEvent::DecayedByDecayGen ) ;

        if ( abs(m_sigPdgCode) == abs(thePart->pdg_id()) ) {
          bool hasFlipped(false);
          sc = m_decayTool -> generateSignalDecay( thePart, hasFlipped , engine) ;
          theSignal = thePart;
        } else
          sc = m_decayTool -> generateDecay( thePart , engine ) ;

        theParticleList.push_back( thePart );

        if ( ! sc.isSuccess() ) return 0 ;
      }
    }
  }
  return theSignal ;
}

//=============================================================================
// Set up event
//=============================================================================
void ParticleGun::prepareInteraction( std::vector<HepMC::GenEvent> * theEvents ,
    LHCb::GenCollisions * theCollisions , HepMC::GenEvent * & theGenEvent ,  
    LHCb::GenCollision * & theGenCollision ) const {
  theEvents->emplace_back(HepMC::Units::MEV, HepMC::Units::MM);
  theGenEvent = &theEvents->back();
  theGenEvent->add_attribute( Gaussino::HepMC::Attributes::GeneratorName,
                              std::make_shared<HepMC::StringAttribute>( m_particleGunName) );
  // Little hack to make it thread-safe when reading later
  theGenEvent->attribute<HepMC::StringAttribute>(Gaussino::HepMC::Attributes::GeneratorName);

  //FIXME: Still need fix this, see header
  theGenCollision = new LHCb::GenCollision();
  theCollisions->insert(theGenCollision);
  //theGenCollision = &theCollisions->back();
  // FIXME: Need to modify new GenCollisions class for persistency
  //theGenCollision->setEvent( theHepMCEvent );
}
