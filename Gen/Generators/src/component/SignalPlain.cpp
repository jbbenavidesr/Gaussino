// $Id: SignalPlain.cpp,v 1.16 2007-02-22 13:30:24 robbep Exp $
// Include files 

// local
#include "SignalPlain.h"

// from Gaudi

// Event 
#include "Event/GenFSR.h"
#include "Event/GenCountersFSR.h"

// Kernel
#include "GenInterfaces/IGenCutTool.h"
#include "GenInterfaces/IDecayTool.h"

// from Generators
#include "GenInterfaces/IProductionTool.h"
#include "GenEvent/HepMCUtils.h"

#include "HepMC/VertexAttribute.h"
#include "Defaults/HepMCAttributes.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalPlain
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( SignalPlain )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SignalPlain::SignalPlain( const std::string& type, const std::string& name,
                          const IInterface* parent )
  : Signal( type, name , parent ) { ; }

//=============================================================================
// Destructor
//=============================================================================
SignalPlain::~SignalPlain( ) { ; }

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool SignalPlain::generate( const unsigned int nPileUp , 
                            std::vector<HepMC::GenEvent> & theEvents , 
                            LHCb::GenCollisions & theCollisions ) {
  StatusCode sc ;
  bool result = false ;
  // Memorize if the particle is inverted
  bool isInverted = false ;
  bool hasFlipped = false ;
  bool hasFailed = false ;
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;
  
  IDataProviderSvc* fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);
  std::string FSRName = LHCb::GenFSRLocation::Default;
  LHCb::GenFSR* genFSR = getIfExists<LHCb::GenFSR>(fileRecordSvc, FSRName);
  int key = 0;  

  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( &theEvents , &theCollisions , theGenEvent, 
                        theGenCollision ) ;
    
    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! result ) {
      // Decay particles heavier than the particles to look at
      decayHeavyParticles( theGenEvent , m_signalQuark , m_signalPID ) ;
      
      // Check if one particle of the requested list is present in event
      ParticleVector theParticleList ;
      if ( checkPresence( m_pids , theGenEvent , theParticleList ) ) {

        // establish correct multiplicity of signal
        if ( ensureMultiplicity( theParticleList.size() ) ) {

          // choose randomly one particle and force the decay
          hasFlipped = false ;
          isInverted = false ;
          hasFailed  = false ;
          HepMC::GenParticlePtr theSignal =
            chooseAndRevert( theParticleList , isInverted , hasFlipped , hasFailed ) ;
          if ( hasFailed ) {
            HepMCUtils::RemoveDaughters( theSignal ) ;
            Error( "Skip event" ) ;
            return false ;
          }

          theParticleList.clear() ;
          theParticleList.push_back( theSignal ) ;

          if ( ! hasFlipped ) {

            m_nEventsBeforeCut++ ;
            key = LHCb::GenCountersFSR::CounterKeyToType("BeforeLevelCut");
            genFSR->incrementGenCounter(key, 1);

            // count particles in 4pi
            updateCounters( theParticleList , m_nParticlesBeforeCut , 
                            m_nAntiParticlesBeforeCut , false , false ) ;
            
            bool passCut = true ;
            if ( 0 != m_cutTool ) 
              passCut = m_cutTool -> applyCut( theParticleList , theGenEvent ,
                                               theGenCollision ) ;
            
            if ( passCut && ( ! theParticleList.empty() ) ) {
              if ( ! isInverted ) {
                m_nEventsAfterCut++ ;
                key = LHCb::GenCountersFSR::CounterKeyToType("AfterLevelCut");
                genFSR->incrementGenCounter(key, 1);              
              }

              if ( isInverted ) {
                ++m_nInvertedEvents ;
                key = LHCb::GenCountersFSR::CounterKeyToType("EvtInverted");
                genFSR->incrementGenCounter(key, 1);                
              }

              // Count particles passing the generator level cut with pz > 0     
              updateCounters( theParticleList , m_nParticlesAfterCut , 
                              m_nAntiParticlesAfterCut , true , isInverted ) ;              
              
              if ( m_cleanEvents ) {
                sc = isolateSignal( theSignal ) ;
                if ( ! sc.isSuccess() ) Exception( "Cannot isolate signal" ) ;
              }
              theGenEvent->add_attribute(Gaussino::HepMC::Attributes::SignalProcessVertex, std::make_shared<HepMC::VertexAttribute>(theSignal->end_vertex()));
              
              theGenCollision -> setIsSignal( true ) ;
              
              // Count signal B and signal Bbar
              if ( theSignal -> pdg_id() > 0 ) {
                ++m_nSig ;
                key = LHCb::GenCountersFSR::CounterKeyToType("EvtSignal");
                genFSR->incrementGenCounter(key, 1);
              }
              else {
                ++m_nSigBar ;
                key = LHCb::GenCountersFSR::CounterKeyToType("EvtantiSignal");                
                genFSR->incrementGenCounter(key, 1);
              }
              

              // Update counters
              GenCounters::updateHadronCounters( theGenEvent , m_bHadC , 
                                                 m_antibHadC , m_cHadC , 
                                                 m_anticHadC , m_bbCounter ,
                                                 m_ccCounter ) ;
              GenCounters::updateExcitedStatesCounters( theGenEvent , 
                                                        m_bExcitedC , 
                                                        m_cExcitedC ) ;
              
              GenCounters::updateHadronFSR( theGenEvent, genFSR, "Acc");
              

              result = true ;
            } else {
              // event does not pass cuts
              HepMCUtils::RemoveDaughters( theSignal ) ;
            }
          } else {
            // has flipped:
            HepMCUtils::RemoveDaughters( theSignal ) ;
            theSignal -> set_pdg_id( - ( theSignal -> pdg_id() ) ) ;
          }
        }
      }
    }
  }  
  
  return result ;
}

