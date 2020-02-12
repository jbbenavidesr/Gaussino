// $Id: SignalForcedFragmentation.cpp,v 1.15 2008-07-24 22:05:38 robbep Exp $
// Include files

// from Gaudi
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

// from HepMC
#include "HepMC3/GenEvent.h"
#include "HepMCUser/VertexAttribute.h"
#include "Defaults/HepMCAttributes.h"

// from Kernel
#include "GenInterfaces/IGenCutTool.h"
#include "GenInterfaces/IDecayTool.h"

// from Generators
#include "GenInterfaces/IProductionTool.h"
#include "HepMCUtils/HepMCUtils.h"

// from Event                                                                                                                                                    
#include "Event/GenFSR.h"
#include "Event/GenFSRMTManager.h"
#include "Event/GenCountersFSR.h"

#include "HepMCUser/Status.h"

// local
#include "SignalForcedFragmentation.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

#include "HepMC3/Relatives.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalForcedFragmentation
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( SignalForcedFragmentation )


//=============================================================================
// Initialize method
//=============================================================================
StatusCode SignalForcedFragmentation::initialize( ) {
  StatusCode sc = Signal::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  LHCb::IParticlePropertySvc * ppSvc = 
    svc< LHCb::IParticlePropertySvc >( "LHCb::ParticlePropertySvc" ) ;
  const LHCb::ParticleProperty * prop = ppSvc -> find( LHCb::ParticleID( *m_pids.begin() ) ) ;
  m_signalMass = prop -> mass() ;

  release( ppSvc ) ;
  
  return sc ;
}

//=============================================================================
// Generate set of events with repeated hadronization
//=============================================================================
bool SignalForcedFragmentation::generate( const unsigned int nPileUp ,
                                          HepMC3::GenEventPtrs & theEvents ,
                                          LHCb::GenCollisions & theCollisions ,
                                          HepRandomEnginePtr & engine ) const
{
  StatusCode sc ;
  CLHEP::RandFlat flatGenerator{engine.getref(), 0, 1};

  // first decay signal particle
  HepMC3::GenEventPtr theSignalHepMCEvent = std::make_shared<HepMC3::GenEvent>( ) ;
  HepMC3::GenParticlePtr theSignalAtRest{new HepMC3::GenParticle( )};
  theSignalAtRest -> 
    set_momentum( HepMC3::FourVector( 0., 0., 0., m_signalMass ) ) ;

  // Memorize if signal has been inverted (not used here)
  bool isInverted = false ;
  bool dummyHasFlipped = false ;
  bool hasFailed = false ;

  // Create an origin vertex at (0,0,0,0) for the signal particle at rest
  HepMC3::GenVertex * theVertex =  
    new HepMC3::GenVertex( HepMC3::FourVector( 0., 0., 0., 0. ) ) ;
  theSignalHepMCEvent -> add_vertex( theVertex ) ;
  theVertex -> add_particle_out( theSignalAtRest ) ;
  
  bool flip ;
  int theSignalPID = *m_pids.begin() ;

  auto genFSR = GenFSRMTManager::GetGenFSR(m_FSRName);
  int key = 0;

  if ( m_cpMixture ) {
    // decide which flavour to generate : 
    // if flavour < 0.5, b flavour
    // if flavour >= 0.5, bbar flavour
    double flavour = flatGenerator() ;

    m_decayTool -> enableFlip() ;
    
    if ( flavour < 0.5 ) 
      theSignalAtRest -> set_pdg_id( +abs( theSignalPID ) ) ;
    else
      theSignalAtRest -> set_pdg_id( -abs( theSignalPID ) ) ;
  } else {
    // generate only one flavour
    m_decayTool -> disableFlip() ;
    theSignalAtRest -> set_pdg_id( theSignalPID ) ;
  }

  sc = m_decayTool -> generateSignalDecay( theSignalAtRest , flip , engine ) ;
  if ( ! sc.isSuccess() ) return false ;

  bool result = false ;  

  PIDs signalPid ;
  signalPid.insert( theSignalAtRest -> pdg_id() ) ;

  sc = m_productionTool -> 
    setupForcedFragmentation( theSignalAtRest -> pdg_id() ) ;
  if ( sc.isFailure() ) error() << "Could not force fragmentation" << endmsg ;

  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC3::GenEventPtr theGenEvent( 0 ) ;

  // TODO: fix problem when 2 consecutive B events. The 2 B events both have
  // signal in them !

  // Then generate set of pile-up events    
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( &theEvents , &theCollisions , theGenEvent ,
                        theGenCollision ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision , engine ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! result ) {
      ParticleVector theParticleList ;
      if ( checkPresence( signalPid , theGenEvent , theParticleList ) ) {
        m_nEventsBeforeCut++ ;
        key = LHCb::GenCountersFSR::CounterKeyToType("BeforeLevelCut");
        if(genFSR) genFSR->incrementGenCounter(key, 1);
        
        updateCounters( theParticleList , m_nParticlesBeforeCut , 
                        m_nAntiParticlesBeforeCut , false , false ) ;

        HepMC3::GenParticlePtr theSignal = chooseAndRevert( theParticleList , 
                                                           isInverted ,
                                                           dummyHasFlipped , 
							  hasFailed , engine ) ;

        // Erase daughters of signal particle
        HepMCUtils::RemoveDaughters( theSignal ) ;
	
	if ( hasFailed ) {
	  Error( "Skip event" ) ;
	  return false  ;
	}
        
        theParticleList.clear() ;
        theParticleList.push_back( theSignal ) ;

        // Now boost signal at rest to frame of signal produced by 
        // production generator
        Gaudi::LorentzVector mom( theSignal -> momentum() ) ;
        ROOT::Math::Boost theBoost( -mom.BoostToCM() ) ;
        
        // Give signal status
        theSignal -> set_status( HepMC3::Status::SignalInLabFrame ) ;
        
        sc = boostTree( theSignal , theSignalAtRest , theBoost ) ;
        if ( ! sc.isSuccess() ) Exception( "Cannot boost signal tree" ) ;

        bool passCut = true ;
        if ( 0 != m_cutTool ) 
          passCut = m_cutTool -> applyCut( theParticleList , theGenEvent.get() ,
                                           theGenCollision ) ;
        
        if ( passCut && ( ! theParticleList.empty() ) ) {          
          m_nEventsAfterCut++ ;
          
          updateCounters( theParticleList , m_nParticlesAfterCut , 
                          m_nAntiParticlesAfterCut , true , false ) ;
          
          if ( isInverted ) {
            ++m_nInvertedEvents ;
            key = LHCb::GenCountersFSR::CounterKeyToType("EvtInverted");
            if(genFSR) genFSR->incrementGenCounter(key, 1); 
          }
          else
          {
            key = LHCb::GenCountersFSR::CounterKeyToType("AfterLevelCut");
            if(genFSR) genFSR->incrementGenCounter(key, 1);            
          }

          if ( m_cleanEvents ) { 
            sc = isolateSignal( theSignal ) ;
            if ( ! sc.isSuccess() ) Exception( "Cannot isolate signal" ) ;
          }
          
          theGenEvent->add_attribute(Gaussino::HepMC::Attributes::SignalProcessVertex,
              std::make_shared<HepMC3::VertexAttribute>(theSignal->end_vertex()));
          theGenCollision -> setIsSignal( true ) ;
          
          // Count signal B and signal Bbar
          if ( theSignal -> pdg_id() > 0 ) {
            ++m_nSig ;
            key = LHCb::GenCountersFSR::CounterKeyToType("EvtSignal");
            if(genFSR) genFSR->incrementGenCounter(key, 1);
          }
          else
          {
            ++m_nSigBar ;
            key = LHCb::GenCountersFSR::CounterKeyToType("EvtantiSignal");
            if(genFSR) genFSR->incrementGenCounter(key, 1);            
          }

          // Update counters
          GenCounters::updateHadronCounters( theGenEvent.get() , m_bHadC ,
                                             m_antibHadC , m_cHadC ,
                                             m_anticHadC , m_bbCounter ,
                                             m_ccCounter ) ;
          GenCounters::updateExcitedStatesCounters( theGenEvent.get() ,
                                                    m_bExcitedC ,
                                                    m_cExcitedC ) ;

          if(genFSR) GenCounters::updateHadronFSR( theGenEvent.get(), genFSR, "Acc");

          result = true ;
        } 
      }   
    }
  }

  // Now a smart pointer so no explicit deletion necessary
  //delete theSignalHepMCEvent ;
  return result ;
}

//=============================================================================
// Boost the Tree theSignal to theVector frame and attach it to theMother
//=============================================================================
StatusCode SignalForcedFragmentation::boostTree( HepMC3::GenParticlePtr
                                                 theSignal ,
                                                 HepMC3::ConstGenParticlePtr
                                                 theSignalAtRest ,
                                                 const ROOT::Math::Boost& 
                                                 theBoost )
  const {
  if ( ! theSignalAtRest -> end_vertex() ) return StatusCode::SUCCESS ;
    
  if ( theSignal -> end_vertex() ) 
    return Error( "The particle has already a decay vertex !" ) ;

  if ( ! theSignalAtRest -> production_vertex() )
    return Error( "The particle has no production vertex !" ) ;
  
  // Displacement in original frame
  Gaudi::LorentzVector positionEnd , positionBegin ;
  positionEnd.SetXYZT( theSignalAtRest -> end_vertex() -> position() . x() ,
                       theSignalAtRest -> end_vertex() -> position() . y() ,
                       theSignalAtRest -> end_vertex() -> position() . z() ,
                       theSignalAtRest -> end_vertex() -> position() . t() *
                       Gaudi::Units::c_light ) ;
  
  positionBegin.SetXYZT(theSignalAtRest->production_vertex()->position().x() ,
                        theSignalAtRest->production_vertex()->position().y() ,
                        theSignalAtRest->production_vertex()->position().z() ,
                        theSignalAtRest->production_vertex()->position().t() 
                        * Gaudi::Units::c_light ) ;
  
  Gaudi::LorentzVector position = positionEnd - positionBegin ;
  
  // Displacement in new frame after boost.
  Gaudi::LorentzVector newPosition = theBoost( position ) ;

  // LHCb Units
  Gaudi::LorentzVector newP ;
  newP.SetXYZT( newPosition.X() , newPosition.Y() , newPosition.Z() ,
                newPosition.T() / Gaudi::Units::c_light ) ;
  
  // Add original position
  Gaudi::LorentzVector 
    originalPosition( theSignal -> production_vertex() -> position() ) ;
  newP += originalPosition ;

  // Create new HepMC vertex after boost and add it to the current event    
  HepMC3::GenVertex * newVertex = 
    new HepMC3::GenVertex( HepMC3::FourVector(newP.X(), newP.Y() , newP.Z() , 
                                            newP.T()));
  
  theSignal -> parent_event() -> add_vertex( newVertex ) ;
  newVertex -> add_particle_in( theSignal ) ;

  
  for ( auto child : HepMC3::Relatives::CHILDREN(theSignalAtRest) ) {
    // Boost all daughter particles and create a new HepMC particle
    // for each daughter
    Gaudi::LorentzVector momentum( child -> momentum() ) ;
    Gaudi::LorentzVector newMomentum = theBoost( momentum ) ;
    int id                           = child -> pdg_id() ;
    int status                       = child -> status() ;
    
    HepMC3::GenParticlePtr newPart{
      new HepMC3::GenParticle( HepMC3::FourVector( newMomentum.Px() , 
                                                 newMomentum.Py() , 
                                                 newMomentum.Pz() , 
                                                 newMomentum.E()   ) , 
                              id , status )} ;
    
    newVertex -> add_particle_out( newPart ) ;
    
    HepMC3::GenParticlePtr theNewSignal             = newPart ;
    
    // Recursive call to boostTree for each daughter
    boostTree( theNewSignal , child, theBoost ) ;
  }

  return StatusCode::SUCCESS ;
}
