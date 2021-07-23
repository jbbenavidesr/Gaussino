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
// $Id: Generation.cpp,v 1.33 2009-06-15 17:02:35 gcorti Exp $
// Include files 
#include <map>

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ThreadLocalContext.h"

// from Event
#include "Event/GenFSR.h"
#include "Event/GenFSRMTManager.h"
#include "Event/GenCountersFSR.h"
#include "Event/CrossSectionsFSR.h"

// from LHCb
#include "Kernel/ParticleID.h"

#include "GenInterfaces/IFullGenEventCutTool.h"
#include "GenInterfaces/IDecayTool.h" 
#include "GenInterfaces/ISampleGenerationTool.h"
#include "GenInterfaces/IPileUpTool.h"
#include "GenInterfaces/IVertexSmearingTool.h"
#include "GenInterfaces/ICounterLogFile.h"

// from Generators
#include "Generators/GenCounters.h"
#include "HepMCUtils/HepMCUtils.h"

// local
#include "Generators/Generation.h"

#include <iostream>
#include <stdlib.h>     /* getenv */
#include "TSystem.h"
#include "TUnixSystem.h"

// HepMC
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Attribute.h"

#include "Defaults/HepMCAttributes.h"
#include "range/v3/all.hpp"

#include "NewRnd/RndGlobal.h"
//-----------------------------------------------------------------------------
// Implementation file for class : Generation
//
// 2005-08-11 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory


//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Generation::initialize() {
  StatusCode sc = MultiTransformer::initialize();
  if ( sc.isFailure() ) return sc ;
  debug() << "==> Initialise" << endmsg ;

  // Retrieve pile up tool
  if ( "" == m_pileUpToolName ) {
    info() << "No Pile Up Tool is defined. Will generate no pile-up " 
           << endmsg ;
    info() << "and set luminosity in header to 2e32" << endmsg ;
  } else m_pileUpTool = tool< IPileUpTool >( m_pileUpToolName , this ) ;

  // Retrieve decay tool
  if ( "" != m_decayToolName ) m_decayTool = 
    tool< IDecayTool >( m_decayToolName ) ;

  // Retrieve xml log file tool for generator statistics
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  // Retrieve generation method tool
  if ( "" == m_sampleGenerationToolName ) 
    return Error( "No Sample Generation Tool is defined. This is mandatory" ) ;
  m_sampleGenerationTool = 
    tool< ISampleGenerationTool >( m_sampleGenerationToolName , this ) ;
  
  // Retrieve vertex smearing tool
  if ( "" == m_vertexSmearingToolName ) {
    info() << "No vertex smearing tool is defined. Will not smear anything." 
           << endmsg ;
  } else {
    m_vertexSmearingTool = 
      tool< IVertexSmearingTool >( m_vertexSmearingToolName , this ) ;
  }
  
  // Retrieve full gen event cut tool
  if ( "" != m_fullGenEventCutToolName ) m_fullGenEventCutTool =
    tool< IFullGenEventCutTool >( m_fullGenEventCutToolName , this ) ;

  // Message relative to event type
  always()
    << "=================================================================="
    << endmsg;
  always()
    << "Requested to generate EventType " << m_eventType << endmsg;
  always()
    << "=================================================================="
    << endmsg;

  // get the File Records service                                                                                                                                
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
std::tuple<std::vector<HepMC3::GenEventPtr>, LHCb::GenCollisions, LHCb::GenHeader> Generation::
operator()( const LHCb::GenHeader& old_gen_header) const {
  auto engine = createRndmEngine();
  // Set this as the global engine as some other tools will eventually need it.
  ThreadLocalEngine::Guard rnd_guard(engine);
  return callOperatorImplementation(old_gen_header, engine);
}

std::tuple<std::vector<HepMC3::GenEventPtr>, LHCb::GenCollisions, LHCb::GenHeader>
Generation::callOperatorImplementation( const LHCb::GenHeader& old_gen_header, HepRandomEnginePtr & engine ) const
{
  auto m_genFSR = GenFSRMTManager::GetGenFSR(m_FSRName);

  debug() << "Processing event type " << m_eventType << endmsg;
  StatusCode sc = StatusCode::SUCCESS;
  setFilterPassed( true ) ;

  // Copy the old event header
  LHCb::GenHeader theGenHeader = old_gen_header;

  // Get the header and update the information
  if( !theGenHeader.evType() ){
    theGenHeader.setEvType( m_eventType );  
  }

  if(m_genFSR && m_genFSR->getSimulationInfo("evtType", 0) == 0)
  {
    std::string decFiles = "";
    int evtType = 0;
    std::string path = gSystem->Getenv("DECFILESROOT");
    std::string decFiles_file = "";
   
    if(path!=NULL)
    {
      decFiles_file = path;
    
      for(int i=decFiles_file.length()-1; i>= 0; i--)
      {
        char stop_char = '/';
        char ichar = decFiles_file[i];
        if(ichar!=stop_char)
          decFiles += ichar;
        else
          break; 
      }

      std::reverse(decFiles.begin(), decFiles.end());      
    }

    evtType = m_eventType;

    m_genFSR->addSimulationInfo("evtType", evtType);
    m_genFSR->addSimulationInfo("generationMethod",m_sampleGenerationTool->name());
    m_genFSR->addSimulationInfo("decFiles", decFiles);
    m_genFSR->incrementNJobs();
  }

  unsigned int  nPileUp( 0 ) ;

  // Create temporary containers for this event
  std::vector<HepMC3::GenEventPtr> theEvents;
  LHCb::GenCollisions theCollisions;

  interactionCounter theIntCounter ;

  // variables to store counters in FSR                                                              
  int key = 0;  
  std::string name = " ";

  // Generate a set of interaction until a good one is found
  bool goodEvent = false ;
  while ( ! goodEvent ) {
    theEvents.clear() ;
    theCollisions.clear() ;
    
    // Compute the number of pile-up interactions to generate 
    if ( 0 != m_pileUpTool ) 
      nPileUp = m_pileUpTool -> numberOfPileUp( engine ) ;
    else 
      // default set to 1 pile and 2.10^32 luminosity
      nPileUp = 1 ;

    theEvents.reserve(nPileUp);
    // generate a set of Pile up interactions according to the requested type
    // of event
    if ( 0 < nPileUp ) 
      goodEvent = m_sampleGenerationTool -> generate( nPileUp, theEvents, 
                                                      theCollisions, engine );
    else { 
      goodEvent = true ;
      setFilterPassed( false ) ;
    }
    
    // increase event and interactions counters
    //++m_nEvents ;    m_nInteractions += nPileUp ;

    // increase the generated events counter in the FSR                                                                                                          
    name = "EvtGenerated";
    key = LHCb::GenCountersFSR::CounterKeyToType(name);
    if(m_genFSR) m_genFSR->incrementGenCounter(key,1);
    // increase the generated interactions counter in the FSR                                                                                                    
    name = "IntGenerated";
    key = LHCb::GenCountersFSR::CounterKeyToType(name);    
    if(m_genFSR) m_genFSR->incrementGenCounter(key,nPileUp);

    // Update interaction counters
    if ( 0 < nPileUp ) { 
      //unsigned int val = 0;
      //for(auto & x: theIntCounter){
        //std::atomic_init<unsigned int>(&x, 0);
      //}
      for ( auto & evt : theEvents ){
        updateInteractionCounters( theIntCounter , evt.get() );
      }
    
      // Increse the generated interactions counters in FSR                                                                                                      
      updateFSRCounters(theIntCounter, m_genFSR, "Gen");

      //GenCounters::AddTo( m_intC , theIntCounter ) ;

      // Decay the event if it is a good event
      if ( goodEvent ) {
        unsigned short iPile( 0 ) ;
        for ( auto & evt : theEvents ) {
          if ( m_decayTool ) {
            sc = decayEvent( evt , engine ) ;
            if ( ! sc.isSuccess() ) goodEvent = false ;
          }
          evt->set_event_number( ++iPile ) ;
          if(m_vertexSmearingTool){
            if ( ( ! ( m_commonVertex.value() ) ) || ( 1 == iPile ) )
                sc = m_vertexSmearingTool -> smearVertex( evt , engine ) ;
            if ( ! sc.isSuccess() ) error() << "Smearing tool failed" << endmsg;
          }
        }
      }
      auto bla = *std::begin(theEvents);
      if ( ( m_commonVertex.value() ) && ( 1 < nPileUp ) ) {
        auto commonV = 
          (*std::begin((*std::begin(theEvents))->beams()))->end_vertex()->position();
        for ( auto & evt : theEvents ) {
          for ( auto & vtx : evt->vertices() ) {
            auto pos = vtx -> position() ;
            //FIXME: Shouldn't this shift by -pos + commonV to have the same vertex?
            vtx -> set_position( HepMC3::FourVector( pos.x() + commonV.x() , 
                                                    pos.y() + commonV.y() , 
                                                    pos.z() + commonV.z() , 
                                                    pos.t() + commonV.t() ) ) ;
          }
        } 
      }
      

      // Apply generator level cut on full event
      if ( m_fullGenEventCutTool ) {
        if ( goodEvent ) {
          //++m_nBeforeFullEvent ;
          // increase the counter of events before the full event generator level cut in the FSR                                                                 
          name = "BeforeFullEvt";
          key = LHCb::GenCountersFSR::CounterKeyToType(name);          
          if(m_genFSR) m_genFSR->incrementGenCounter(key,1);
          goodEvent = m_fullGenEventCutTool -> studyFullEvent( theEvents , 
                                                             theCollisions );
          if ( goodEvent ) {
            //++m_nAfterFullEvent ;
            // increase the counter of events after the full event generator level cut in the FSR                                                                
            name = "AfterFullEvt";
            key = LHCb::GenCountersFSR::CounterKeyToType(name);
            if(m_genFSR) m_genFSR->incrementGenCounter(key,1);            
          }
        }
      }
    }
  }  

  //++m_nAcceptedEvents ;
  //m_nAcceptedInteractions += nPileUp ;
  
  // increase the generated events counter in the FSR                                                                                                            
  name = "EvtAccepted";
  key = LHCb::GenCountersFSR::CounterKeyToType(name);
  if(m_genFSR) m_genFSR->incrementGenCounter(key,1);

  // increase the generated interactions counter in the FSR                                                                                                      
  name = "IntAccepted";
  key = LHCb::GenCountersFSR::CounterKeyToType(name);  
  if(m_genFSR) m_genFSR->incrementGenCounter(key,nPileUp);

  if ( 0 < nPileUp ) {
    //GenCounters::AddTo( m_intCAccepted , theIntCounter ) ;

    // Increse the accepted interactions counters in FSR                                                                                                         
    updateFSRCounters(theIntCounter, m_genFSR, "Acc");

  }
    
  // Copy the HepMCevents and Collisions from the temporary containers to 
  // those in TES and update the header information

  // Check that number of temporary HepMCEvents is the same as GenCollisions
  if( theEvents.size() != theCollisions.size() ) {
    error() << "Number of HepMCEvents and GenCollisions do not match" << endmsg;
  }

  if ( 0 < nPileUp ) {
    for( auto event_gencol : ranges::view::zip(theEvents, theCollisions)) {
      auto & evt = event_gencol.first;
      // GenFSR
      if(m_genFSR && m_genFSR->getSimulationInfo("hardGenerator", "") == "")
        m_genFSR->addSimulationInfo("hardGenerator",evt->attribute<HepMC3::StringAttribute>(Gaussino::HepMC::Attributes::GeneratorName)->value());
    }
  }

  //Just before writing, set the event and run number of the HepMC events so they are persisted.
  for(auto & evt : theEvents){
    evt->add_attribute(Gaussino::HepMC::Attributes::GaudiEventNumber, std::make_shared<HepMC3::IntAttribute>(old_gen_header.evtNumber()));
    evt->add_attribute(Gaussino::HepMC::Attributes::GaudiRunNumber, std::make_shared<HepMC3::IntAttribute>(old_gen_header.runNumber()));
  }

  return std::make_tuple(std::move(theEvents), std::move(theCollisions), std::move(theGenHeader));
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Generation::finalize() {
  using namespace GenCounters ;
  debug( ) << "==> Finalize" << endmsg ;
  // Add the event type number to the log file
  m_xmlLogTool -> addEventType( m_eventType ) ;
  
  // Print the various counters
  if ( 0 != m_pileUpTool ) { m_pileUpTool -> printPileUpCounters( ) ; }

  //printCounter( m_xmlLogTool , "generated events" , m_nEvents ) ;
  //printCounter( m_xmlLogTool , "generated interactions" , m_nInteractions ) ;
  
  //for ( unsigned int i = 0 ; i < m_intC.size() ; ++i )
    //printCounter( m_xmlLogTool , m_intCName[ i ] , m_intC[ i ] ) ;
  
  //printCounter( m_xmlLogTool , "accepted events" , m_nAcceptedEvents ) ;
  //printCounter( m_xmlLogTool , "interactions in accepted events" , 
                //m_nAcceptedInteractions ) ;
  
  //for ( unsigned int j = 0 ; j < m_intCAccepted.size() ; ++j ) 
    //printCounter( m_xmlLogTool , m_intCAcceptedName[ j ] , 
                  //m_intCAccepted[ j ] ) ;

  //printEfficiency( m_xmlLogTool , "full event cut" , m_nAfterFullEvent , 
                   //m_nBeforeFullEvent ) ;

  m_sampleGenerationTool -> printCounters() ;

  // create a new FSR and append to TDS                                                                                                                          
  auto m_genFSR = GenFSRMTManager::GetCombined(m_FSRName);

  // Now either create the info in the TES or add it to the existing one                                                                                         
  if(m_genFSR) put(m_fileRecordSvc, m_genFSR, m_FSRName);

  // check if the FSR can be retrieved from the TS                                                                                                               
  LHCb::GenFSR* readFSR = getIfExists<LHCb::GenFSR>(m_fileRecordSvc, m_FSRName);
  if(readFSR!=NULL)    // print the FSR just retrieved from TS                                                                                                   
    always() << "READ FSR: " << *readFSR << endmsg;

  if ( 0 != m_pileUpTool ) release( m_pileUpTool ).ignore() ;
  if ( 0 != m_decayTool ) release( m_decayTool ).ignore() ;
  if ( 0 != m_sampleGenerationTool ) release( m_sampleGenerationTool ).ignore() ;
  if ( 0 != m_vertexSmearingTool ) release( m_vertexSmearingTool ).ignore() ;
  if ( 0 != m_fullGenEventCutTool ) release( m_fullGenEventCutTool ).ignore() ;
  
  return GaudiAlgorithm::finalize( ) ; // Finalize base class
}

//=============================================================================
// Decay in the event all particles which have been left stable by the
// production generator
//=============================================================================
StatusCode Generation::decayEvent( HepMC3::GenEventPtr theEvent , HepRandomEnginePtr & engine ) const {
  m_decayTool -> disableFlip() ;
  StatusCode sc ;
  
  // Stuff the particles into a set. While this is not necessary to order, we have
  // cannot iterate over the list of particles themselves as this gets modified while
  // decaying the event as decay products are added
  HepMCUtils::ParticleSet tmp_set{std::begin( theEvent->particles() ), std::end( theEvent->particles() )};

  for ( auto & thePart : tmp_set) {
    
    unsigned int status = thePart -> status() ;
    
    if ( ( HepMC3::Status::StableInProdGen  == status ) || 
         ( ( HepMC3::Status::DecayedByDecayGenAndProducedByProdGen == status )
           && ( 0 == thePart -> end_vertex() ) ) ) {
      
      if ( m_decayTool -> isKnownToDecayTool( thePart -> pdg_id() ) ) {
        
        if ( HepMC3::Status::StableInProdGen == status ) 
          thePart -> 
            set_status( HepMC3::Status::DecayedByDecayGenAndProducedByProdGen ) ;
        else thePart -> set_status( HepMC3::Status::DecayedByDecayGen ) ;
        
        sc = m_decayTool -> generateDecay( thePart , engine ) ;
        if ( ! sc.isSuccess() ) return sc ;
      }
    } 
  }  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Interaction counters
//=============================================================================
void Generation::updateInteractionCounters( interactionCounter & theCounter ,
                                            const HepMC3::GenEvent* theEvent ) const
{
  unsigned int bQuark( 0 ) , bHadron( 0 ) , cQuark( 0 ) , cHadron( 0 ) ;
  int pdgId ;

  for ( auto & thePart : theEvent->particles() ) {
    if ( ( thePart -> status() == HepMC3::Status::DocumentationParticle ) ||
         ( thePart -> status() == HepMC3::Status::DecayedByDecayGen ) ||
         ( thePart -> status() == HepMC3::Status::StableInDecayGen ) ) 
      continue ;
    pdgId = abs( thePart -> pdg_id() ) ;
    LHCb::ParticleID thePid( pdgId ) ;

    if ( 5 == pdgId ) { 
      if (  thePart -> production_vertex() ) {
        if ( 1 != thePart -> production_vertex() -> particles_in().size() ) {
           bool containB = false;
           for(auto & part: thePart->production_vertex()->particles_in()){
             if (5==abs(part->pdg_id()))
               containB = true;
           }
           if (!containB) ++bQuark ;
        } else {
          auto & par = 
            *std::begin(thePart -> production_vertex() -> particles_in() ) ;
          if ( ( par -> status() == 
                 HepMC3::Status::DocumentationParticle ) ||
               ( par -> pdg_id() != thePart -> pdg_id() ) ) { 
            ++bQuark ;
          }
        }
      }
    }
    else if( 4 == pdgId ) {
      if ( thePart -> production_vertex() ) {
        if ( 1 != thePart->production_vertex() -> particles_in().size() ) {
          bool containC = false;
           for(auto & part: thePart->production_vertex()->particles_in()){
            if (4==abs(part->pdg_id()))
              containC = true;
          }
          if (!containC) ++cQuark ;
        } else {
          auto & par =
            *std::begin(thePart -> production_vertex() -> particles_in() ) ;
          if ( ( par -> status() ==
                 HepMC3::Status::DocumentationParticle ) ||
               ( par -> pdg_id() != thePart -> pdg_id() ) ) {
            ++cQuark ;
          }
        }
      }
    }
    else {
      if ( thePid.hasBottom() ) {
        // Count B from initial proton as a quark
        if ( thePart -> production_vertex() ) {
          if ( 0 != thePart -> production_vertex() -> particles_in().size() ) {
            auto & par = 
            *std::begin(thePart -> production_vertex() -> particles_in() ) ;
            if ( par -> production_vertex() ) {
              if ( 0 == par -> production_vertex() -> particles_in().size() ) {
                ++bQuark ;
              }
            } 
            else {
              ++bQuark ;
            }
          }
        }
        ++bHadron ;
      } else if ( thePid.hasCharm() ) {
        // Count D from initial proton as a quark
        if ( thePart -> production_vertex() ) {
          if ( 0 != thePart -> production_vertex() -> particles_in().size() ) {
            auto & par = 
            *std::begin(thePart -> production_vertex() -> particles_in() ) ;
            if ( par -> production_vertex() ) {
              if ( 0 == par -> production_vertex() -> particles_in().size() ) 
                ++cQuark ;
            } else ++cQuark ;
          }
        }
        ++cHadron ;
      }
    }
  }

  if ( bQuark >= 1 ) { 
    ++theCounter[ Oneb ] ;
    if ( bQuark >= 3 ) ++theCounter[ Threeb ] ;
  }
  if ( cQuark >= 1 ) {
    ++theCounter[ Onec ] ;
    if ( cQuark >= 3 ) ++theCounter[ Threec ] ;
    if ( bQuark >= 1 ) ++theCounter[ bAndc ] ;
  }
  if ( ( 0 == bQuark ) && ( bHadron > 0 ) ) ++theCounter[ PromptB ] ;
  if ( ( 0 == cQuark ) && ( 0 == bHadron ) && ( cHadron > 0 ) ) 
    ++theCounter[ PromptC ];
}


//=============================================================================
// Interaction counters in FSR                                              
//=============================================================================                   

void Generation::updateFSRCounters( interactionCounter & theCounter,
                                    LHCb::GenFSR* m_genFSR,
                                    const std::string option) const
{
  int key = 0; 
  longlong count = 0;
  std::string name[7]= {"Oneb","Threeb","PromptB","Onec","Threec","PromptC","bAndc"};  
  std::string cname = "";

  for(int i=0; i<7; i++)
  {
    cname = name[i]+option;
    key = LHCb::GenCountersFSR::CounterKeyToType(cname);
    count = theCounter[i];
    if(m_genFSR) m_genFSR->incrementGenCounter(key,count); 
  } 
}
