/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// $Id: Inclusive.cpp,v 1.14 2008-07-04 08:51:29 robbep Exp $
// Include files 

// local
#include "Inclusive.h"

// from Gaudi
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// from Kernel
#include "GenInterfaces/IGenCutTool.h"

// from Generators
#include "GenInterfaces/IProductionTool.h"
#include "Generators/GenCounters.h"
#include "GenInterfaces/ICounterLogFile.h"

// from Event                                                                                                                                                    
#include "Event/GenFSR.h"
#include "Event/GenFSRMTManager.h"
#include "Event/GenCountersFSR.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Inclusive
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT( Inclusive )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Inclusive::Inclusive( const std::string& type, const std::string& name,
                      const IInterface* parent )
  : ExternalGenerator  ( type, name , parent ) {
    declareProperty( "InclusivePIDList" , m_pidVector ) ;

    GenCounters::setupBHadronCountersNames( m_bHadCNames , m_antibHadCNames ) ;
    GenCounters::setupDHadronCountersNames( m_cHadCNames , m_anticHadCNames ) ;
    
    GenCounters::setupExcitedCountersNames( m_bExcitedCNames , "B" ) ;
    GenCounters::setupExcitedCountersNames( m_cExcitedCNames , "D" ) ;
  }

//=============================================================================
// Destructor
//=============================================================================
Inclusive::~Inclusive( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode Inclusive::initialize( ) {
  StatusCode sc = ExternalGenerator::initialize( ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize base class !" ) ;

  if ( m_pidVector.empty() ) 
    return Error( "InclusivePIDList property is not set" ) ;

  // Transform vector into set
  for ( std::vector<int>::iterator it = m_pidVector.begin() ; 
        it != m_pidVector.end() ; ++it ) m_pids.insert( *it ) ;
  
  LHCb::IParticlePropertySvc * ppSvc =
    svc< LHCb::IParticlePropertySvc >( "LHCb::ParticlePropertySvc" ) ;
  
  info() << "Generating Inclusive events of " ;
  PIDs::const_iterator it ;
  bool bottom( false ) , charm( false ) ;
  
  for ( it = m_pids.begin() ; it != m_pids.end() ; ++it ) {
    const LHCb::ParticleProperty * prop = ppSvc -> find( LHCb::ParticleID( *it ) ) ;
    if ( 0 == prop ) 
      warning() << "PDG Code " << (*it) << " does not exist." << endmsg ;
    else {
      info() << prop -> particle() << " " ;      
      if ( LHCb::ParticleID( prop -> pdgID() ).hasBottom() ) bottom = true ;
      else if ( LHCb::ParticleID( prop -> pdgID() ).hasCharm()  ) charm  = true ;
    }
  }

  if ( bottom && ! charm ) m_lightestQuark = LHCb::ParticleID::bottom ;
  else if ( charm ) m_lightestQuark = LHCb::ParticleID::charm ;
  else if (m_pids.size() > 2) return Error( "This case is not yet implemented" ) ;

  info() << endmsg ;

  // XML Log file
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  return sc ;
}

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool Inclusive::generate( const unsigned int nPileUp , 
                          HepMC3::GenEventPtrs & theEvents , 
                          LHCb::GenCollisions & theCollisions ,
                          HepRandomEnginePtr & engine ) const {
  StatusCode sc ;
  bool result = false ;

  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC3::GenEventPtr theGenEvent( 0 ) ;

  // Moved into conditional statement for now
  //GenCounters::BHadronCounter thebHadC , theantibHadC ;
  //GenCounters::DHadronCounter thecHadC , theanticHadC ;
  //GenCounters::ExcitedCounter thebExcitedC , thecExcitedC ;
  //unsigned int theccCounter , thebbCounter ;
  
  auto genFSR = GenFSRMTManager::GetGenFSR(m_FSRName);
  int key = 0;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( &theEvents , &theCollisions , theGenEvent, 
                        theGenCollision ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision , engine ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! result ) {
      // Decay particles heavier than the particles to look at
      // If N hadrons <= 2, we assume it is (h,hbar)
      // Passing m_pids[0] needed for incl. charmless bottomless hadron production
      sc &= decayHeavyParticles( theGenEvent , m_lightestQuark , m_pids.size() > 2 ? 0 : *(m_pids.begin()), engine ) ;
      
      // Check if one particle of the requested list is present in event
      ParticleVector theParticleList ;
      if ( checkPresence( m_pids , theGenEvent , theParticleList ) ) {
        // Update counters
        //thebHadC.assign( 0 )     ;    theantibHadC.assign( 0 ) ;
        //thecHadC.assign( 0 )     ;    theanticHadC.assign( 0 ) ;
        //thebExcitedC.assign( 0 ) ;    thecExcitedC.assign( 0 ) ;
        // FIXME: Counters recreated in the conditional as no simple way to reset atomic counters.
        // Should declare the counters non-atomic as they are thread local anyway and provide
        // templated update... functions in GenCounters that accept both the atomic and non-atomic arrays of counters
        GenCounters::BHadronCounter thebHadC{} , theantibHadC{} ;
        GenCounters::DHadronCounter thecHadC{} , theanticHadC{} ;
        GenCounters::ExcitedCounter thebExcitedC{} , thecExcitedC{} ;
        std::atomic_uint thebbCounter{}, theccCounter{};
          
        GenCounters::updateHadronCounters( theGenEvent.get() , thebHadC , 
                                           theantibHadC , thecHadC , 
                                           theanticHadC , thebbCounter , 
                                           theccCounter ) ;
        GenCounters::updateExcitedStatesCounters( theGenEvent.get() , thebExcitedC , 
                                                  thecExcitedC ) ;

        // Accumulate counters
        GenCounters::AddTo( m_bHadC , thebHadC ) ;
        GenCounters::AddTo( m_antibHadC , theantibHadC ) ;
        GenCounters::AddTo( m_cHadC , thecHadC ) ;
        GenCounters::AddTo( m_anticHadC , theanticHadC ) ;

        m_bbCounter += thebbCounter ;  m_ccCounter += theccCounter ;

        GenCounters::AddTo( m_bExcitedC , thebExcitedC ) ;
        GenCounters::AddTo( m_cExcitedC , thecExcitedC ) ;

        if(genFSR) GenCounters::updateHadronFSR( theGenEvent.get(), genFSR, "Gen");

        ++m_nEventsBeforeCut ;
        key = LHCb::GenCountersFSR::CounterKeyToType("BeforeLevelCut");
        if(genFSR) genFSR->incrementGenCounter(key, 1);
        bool passCut = true ;
        if ( 0 != m_cutTool ) 
          passCut = m_cutTool -> applyCut( theParticleList , theGenEvent.get() , 
                                           theGenCollision ) ;
        
        if ( passCut && ( ! theParticleList.empty() ) ) {
          ++m_nEventsAfterCut ;
          result = true ;

          theGenCollision -> setIsSignal( true ) ;

          if ( 0 == nPositivePz( theParticleList ) ) {
            revertEvent( theGenEvent.get() ) ;
            ++m_nInvertedEvents ;
            key = LHCb::GenCountersFSR::CounterKeyToType("EvtInverted");
            if(genFSR) genFSR->incrementGenCounter(key, 1);
          }
          else
          {
            key = LHCb::GenCountersFSR::CounterKeyToType("AfterLevelCut");
            if(genFSR) genFSR->incrementGenCounter(key, 1); 
          }

          GenCounters::AddTo( m_bHadCAccepted , thebHadC ) ;
          GenCounters::AddTo( m_antibHadCAccepted , theantibHadC ) ;
          GenCounters::AddTo( m_cHadCAccepted , thecHadC ) ;
          GenCounters::AddTo( m_anticHadCAccepted , theanticHadC ) ;

          GenCounters::AddTo( m_bExcitedCAccepted , thebExcitedC ) ;
          GenCounters::AddTo( m_cExcitedCAccepted , thecExcitedC ) ;          

          if(genFSR) GenCounters::updateHadronFSR( theGenEvent.get(), genFSR, "Acc");
        }
      }
    }
  }  
  
  return result ;
}

//=============================================================================
// Print the counters
//=============================================================================
void Inclusive::printCounters( ) const {
  using namespace GenCounters ;
  
  printEfficiency( m_xmlLogTool , "generator level cut" , 
                   m_nEventsAfterCut - m_nInvertedEvents , 
                   m_nEventsBeforeCut ) ;
  printCounter( m_xmlLogTool , "z-inverted events" , m_nInvertedEvents ) ;

  printArray( m_xmlLogTool , m_bHadC , m_bHadCNames , "generated" ) ;
  printArray( m_xmlLogTool , m_antibHadC , m_antibHadCNames , "generated" ) ;
  printCounter( m_xmlLogTool , "generated (bb)" , m_bbCounter ) ;
  
  printArray( m_xmlLogTool , m_cHadC , m_cHadCNames , "generated" ) ;
  printArray( m_xmlLogTool , m_anticHadC , m_anticHadCNames , "generated" ) ;
  printCounter( m_xmlLogTool , "generated (cc)" , m_ccCounter ) ;

  printArray( m_xmlLogTool , m_bHadCAccepted , m_bHadCNames , "accepted" ) ;
  printArray( m_xmlLogTool , m_antibHadCAccepted , m_antibHadCNames , "accepted" ) ;
  printCounter( m_xmlLogTool , "accepted (bb)" , m_bbCounterAccepted ) ;
  
  printArray( m_xmlLogTool , m_cHadCAccepted , m_cHadCNames , "accepted" ) ;
  printArray( m_xmlLogTool , m_anticHadCAccepted , m_anticHadCNames , "accepted" ) ;
  printCounter( m_xmlLogTool , "accepted (cc)" , m_ccCounterAccepted ) ;

  printArray( m_xmlLogTool , m_bExcitedC , m_bExcitedCNames , "generated" ) ;
  printArray( m_xmlLogTool , m_bExcitedCAccepted , m_bExcitedCNames , "accepted" ) ;

  printArray( m_xmlLogTool , m_cExcitedC , m_cExcitedCNames , "generated" ) ;
  printArray( m_xmlLogTool , m_cExcitedCAccepted , m_cExcitedCNames , "accepted" ) ;
}

