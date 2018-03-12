// $Id: GenCounters.cpp,v 1.5 2007-09-11 17:48:32 robbep Exp $
// Include files

// local
#include "Generators/GenCounters.h"

// Generators
#include "GenEvent/HepMCUtils.h"

// Boost
#include <boost/array.hpp>

// HepMC
#include "HepMC/GenEvent.h"
#include "HepMC/VertexAttribute.h"

// Defaults for attribute names
#include "Defaults/HepMCAttributes.h"

// Event
#include "Event/HepMCEvent.h"
#include "Event/GenFSR.h"
#include "Event/GenCountersFSR.h"

// LHCb
#include "Kernel/ParticleID.h"

//=============================================================================
// Function to test if a HepMC::GenParticle is a B hadron at root of decay
//=============================================================================
struct isRootB : std::unary_function< const HepMC::GenParticlePtr &, bool > {

  /// test operator, returns true if it is a root B
  bool operator() ( const HepMC::GenParticlePtr & part ) const {

    // Do not consider documentation and special particles
    if ( part -> status() == LHCb::HepMCEvent::DocumentationParticle ) 
      return false ;
    
    // Check if particle has a b quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasBottom() ) return false ;

    // Check if particle has a mother
    if ( ! part -> production_vertex() ) return true ;

    // Check all parents of the B 
    auto & thePV = part -> production_vertex() ;
    for ( auto & parent : thePV->particles(HepMC::parents) ) {
      LHCb::ParticleID parentID( parent -> pdg_id() ) ;
      if ( parentID.hasBottom() && (thePid.abspid()==5 || parentID.abspid()!=5)) return false ;
    }

    // If no parent is a B, then it is a root B
    return true ;
  }
};

//=============================================================================
// Function to test if a HepMC::GenParticle is a D hadron at root of decay
//=============================================================================
struct isRootD : std::unary_function< const HepMC::GenParticlePtr &, bool > {

  /// test operator, returns true if it is a root D
  bool operator() ( const HepMC::GenParticlePtr & part ) const {

    // Do not consider documentation and special particles
    if ( part -> status() == LHCb::HepMCEvent::DocumentationParticle ) 
      return false ;

    // Check if particle has a c quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasCharm() ) return false ;
    if ( thePid.hasBottom() ) return false ;

    // Check if particle has a mother
    if ( ! part -> production_vertex() ) return true ;

    // Check all parents of the D
    HepMC::GenVertex::particles_in_const_iterator parent ;
    auto & thePV = part -> production_vertex() ;
    for ( auto & parent : thePV->particles(HepMC::parents) ) {
      LHCb::ParticleID parentID( parent -> pdg_id() ) ;
      if ( parentID.hasCharm()  && (parentID.abspid()!=4 || thePid.abspid()==4)) return false ;
    }

    // If no parent is a D, then it is a root D
    return true ;
  }
};

//=============================================================================
// Function to test if a HepMC::GenParticle is a B hadron at end of decay tree
//=============================================================================
struct isEndB : std::unary_function< const HepMC::GenParticlePtr &, bool > {

  /// Test operator. Returns true if particle is the last B
  bool operator() ( const HepMC::GenParticlePtr & part ) const {

    // Do not look at special particles
    if ( part -> status() == LHCb::HepMCEvent::DocumentationParticle ) 
      return false ;

    // Test if particle has a b quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasBottom() ) return false ;

    // test oscillation
    if ( ! HepMCUtils::IsBAtProduction( part ) ) return false ;

    // Test if the B has daughters (here we are sure it has not oscillated)
    if ( ! part -> end_vertex() ) return true ;
    
    // Loop over daughters to check if they are B hadrons
    HepMC::GenVertex::particles_out_const_iterator children ;
    auto & theEV = part -> end_vertex() ;
    for ( auto & child : theEV->particles(HepMC::children) ){
      LHCb::ParticleID childID( child -> pdg_id() ) ;
      if ( childID.hasBottom() ) {
        if ( child -> pdg_id() == - part -> pdg_id() ) return true ;
        return false ;
      }
    }

    // If not, then it is a end B
    return true ;
  }
};

//=============================================================================
// Function to test if a HepMC::GenParticle is a D hadron at end of decay tree
//=============================================================================
struct isEndD : std::unary_function< const HepMC::GenParticlePtr &, bool > {

  /// Test operator. Returns true if it is the last D
  bool operator() ( const HepMC::GenParticlePtr & part ) const {

    // Do not look at special particles
    if ( part -> status() == LHCb::HepMCEvent::DocumentationParticle ) 
      return false ;

    // Check if it has a c quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasCharm() ) return false ;

    // Check if it has daughters
    if ( ! part -> end_vertex() ) return true ;

    // Loop over the daughters to find a D hadron
    HepMC::GenVertex::particles_out_const_iterator children ;
    auto & theEV = part -> end_vertex() ;
    for ( auto & child : theEV->particles(HepMC::children) ) {
      LHCb::ParticleID childID( child -> pdg_id() ) ;
      if ( childID.hasCharm() ) return false ;
    }

    // If not, then it is a End D
    return true ;
  }
};

//-----------------------------------------------------------------------------
// Implementation file for namespace : GenCounters
//
// 2006-02-06 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Setup names of B hadron counters
//=============================================================================
void GenCounters::setupBHadronCountersNames( BHadronCNames & BC , 
                                             BHadronCNames & antiBC ) {
    BC[ Bd ] = "B0" ;
    BC[ Bu ] = "B+" ;
    BC[ Bs ] = "Bs0" ;
    BC[ Bc ] = "Bc+" ;
    BC[ bBaryon ] = "b-Baryon" ;

    antiBC[ Bd ] = "anti-B0" ;
    antiBC[ Bu ] = "B-" ;
    antiBC[ Bs ] = "anti-Bs0" ;
    antiBC[ Bc ] = "Bc-" ;
    antiBC[ bBaryon ] = "anti-b-Baryon" ;
}

//=============================================================================
// Setup names of D hadron counters
//=============================================================================
void GenCounters::setupDHadronCountersNames( DHadronCNames & DC , 
                                             DHadronCNames & antiDC ) {
    DC[ D0 ] = "D0" ;
    DC[ Dch ] = "D+" ;
    DC[ Ds ] = "Ds+" ;
    DC[ cBaryon ] = "c-Baryon" ;

    antiDC[ D0 ] = "anti-D0" ;
    antiDC[ Dch ] = "D-" ;
    antiDC[ Ds ] = "Ds-" ;
    antiDC[ cBaryon ] = "anti-c-Baryon" ;
}

//=============================================================================
// Setup names of B excited states
//=============================================================================
void GenCounters::setupExcitedCountersNames( ExcitedCNames & B , 
                                             const std::string & root ) {
  B[ _0star ] = root + "(L=0,J=0)";
    B[ _1star ] = root + "* (L=0, J=1)" ;
    B[ _2star ] = root + "** (L=1, J=0,1,2)" ;
}

//=============================================================================
// Count excited states counters
//=============================================================================
void GenCounters::updateExcitedStatesCounters
( const HepMC::GenEvent * theEvent , ExcitedCounter & thebExcitedC ,
  ExcitedCounter & thecExcitedC ) {

  // Signal Vertex
  // In HepMC3 now stored as an event attribute. If it does not exist, the default
  // constructor is used to construct the attribute. This will yield a GenVertexPtr
  // pointing to nullptr.
  auto signal_process_vertex =
      theEvent->attribute<HepMC::VertexAttribute>( Gaussino::HepMC::Attributes::SignalProcessVertex )->value();

  // Count B :
  std::vector< HepMC::GenParticlePtr > rootB ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( rootB ) , isRootB() ) ;

  std::vector< HepMC::GenParticlePtr >::const_iterator iter ;

  for ( iter = rootB.begin() ; iter != rootB.end() ; ++iter ) {
    if ( signal_process_vertex ) {
      if ( ! HepMCUtils::commonTrees( signal_process_vertex,
                                      (*iter) -> end_vertex() ) ) 
        continue ;
    }
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;

    if ( thePid.isMeson() ) {
      if ( 0 == thePid.lSpin() ) {
        if ( 1 == thePid.jSpin() ) ++thebExcitedC[ _0star ] ;
        else ++thebExcitedC[ _1star ] ;
      } else ++thebExcitedC[ _2star ] ;
    }
  }

  // Count D :
  std::vector< HepMC::GenParticlePtr > rootD ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( rootD ) , isRootD() ) ;

  for ( iter = rootD.begin() ; iter != rootD.end() ; ++iter ) {
    if ( signal_process_vertex) {
      if ( ! HepMCUtils::commonTrees( signal_process_vertex, 
                                      (*iter) -> end_vertex() ) ) 
        continue ;
    }
    
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;
    if ( thePid.isMeson() ) {
      if ( 0 == thePid.lSpin() ) {
        if ( 1 == thePid.jSpin() ) ++thecExcitedC[ _0star ] ;
        else ++thecExcitedC[ _1star ] ;
        } else ++thecExcitedC[ _2star ] ;
    }
  }       
}

//=============================================================================
// Update the counters of number of different hadrons in selected events
//=============================================================================
void GenCounters::updateHadronCounters( const HepMC::GenEvent * theEvent ,
                                        BHadronCounter & thebHadC , 
                                        BHadronCounter & theantibHadC ,
                                        DHadronCounter & thecHadC ,
                                        DHadronCounter & theanticHadC ,
                                        unsigned int & thebbCounter ,
                                        unsigned int & theccCounter ) {
  // Signal vertex
  // In HepMC3 now stored as an event attribute. If it does not exist, the default
  // constructor is used to construct the attribute. This will yield a GenVertexPtr
  // pointing to nullptr.
  auto signal_process_vertex =
      theEvent->attribute<HepMC::VertexAttribute>( Gaussino::HepMC::Attributes::SignalProcessVertex )->value();

  // Count B:
  std::vector< HepMC::GenParticlePtr > endB ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( endB ) , isEndB() ) ;
  std::vector< HepMC::GenParticlePtr >::const_iterator iter ;
  
  for ( iter = endB.begin() ; iter != endB.end() ; ++iter ) {
    if ( signal_process_vertex ) {
      if ( HepMCUtils::commonTrees( signal_process_vertex, 
                                    (*iter) -> end_vertex() ) )
        continue ;
    }
    
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;
    
    if ( thePid.isMeson() ) {
      if ( thePid.pid() > 0 ) {
        if ( thePid.hasUp() ) ++thebHadC[ Bu ] ;
        else if ( thePid.hasDown() ) ++thebHadC[ Bd ] ;
        else if ( thePid.hasStrange() ) ++thebHadC[ Bs ] ;
        else if ( thePid.hasCharm() ) ++thebHadC[ Bc ] ;
        else ++thebbCounter ;
      } else {
        if ( thePid.hasUp() ) ++theantibHadC[ Bu ] ;
        else if ( thePid.hasDown() ) ++theantibHadC[ Bd ] ;
        else if ( thePid.hasStrange() ) ++theantibHadC[ Bs ] ;
        else if ( thePid.hasCharm() ) ++theantibHadC[ Bc ] ;
        else ++thebbCounter ;
      }
    } else if ( thePid.isBaryon() ) {
      if ( thePid.pid() < 0 ) ++thebHadC[ bBaryon ] ;
      else ++theantibHadC[ bBaryon ] ;
    }
  }
  
  std::vector< HepMC::GenParticlePtr > endD ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( endD ) , isEndD() ) ;
  
  for ( iter = endD.begin() ; iter != endD.end() ; ++iter ) {
    if ( signal_process_vertex ) {
      if ( HepMCUtils::commonTrees( signal_process_vertex,
                                    (*iter) -> end_vertex() ) ) 
        continue ;
    }
    
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;
    
    if ( thePid.isMeson() ) {
      if ( thePid.pid() > 0 ) {
        if ( thePid.hasUp() ) ++thecHadC[ D0 ] ;
        else if ( thePid.hasDown() ) ++thecHadC[ Dch ] ;
        else if ( thePid.hasStrange() ) ++thecHadC[ Ds ] ;
        else ++theccCounter ;
      } else {
        if ( thePid.hasUp() ) ++theanticHadC[ D0 ] ;
        else if ( thePid.hasDown() ) ++theanticHadC[ Dch ] ;
        else if ( thePid.hasStrange() ) ++theanticHadC[ Ds ] ;
        else ++theccCounter ;
      }
    } else if ( thePid.isBaryon() ) {
      if ( thePid.pid() > 0 ) ++thecHadC[ cBaryon ] ;
      else ++theanticHadC[ cBaryon ] ;
    }
  } 
}


//=============================================================================                    
// Update the genFRS in selected events                                                            
//=============================================================================                    
void GenCounters::updateHadronFSR( const HepMC::GenEvent * theEvent ,
                                   LHCb::GenFSR* genFSR,
                                   const std::string option)
{  
  // Signal Vertex                                                                        
  // In HepMC3 now stored as an event attribute. If it does not exist, the default
  // constructor is used to construct the attribute. This will yield a GenVertexPtr
  // pointing to nullptr.
  auto signal_process_vertex =
      theEvent->attribute<HepMC::VertexAttribute>( Gaussino::HepMC::Attributes::SignalProcessVertex )->value();
  int key = 0;
  
  // Count B :                                                                                    
  std::vector< HepMC::GenParticlePtr > rootB ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( rootB ) , isRootB() ) ;

  std::vector< HepMC::GenParticlePtr >::const_iterator iter ;

  for ( iter = rootB.begin() ; iter != rootB.end() ; ++iter )
  {
    if ( signal_process_vertex )
    {  
      if ( ! HepMCUtils::commonTrees( signal_process_vertex ,
                                      (*iter) -> end_vertex() ) )
        continue ; 
    }    
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;

    if (thePid.isMeson())
    {
      if (0 == thePid.lSpin())
      {
        if (1 == thePid.jSpin())
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("B"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Bstar"+option);
          genFSR->incrementGenCounter(key, 1); 
        } 
      }
      else
      {
        key = LHCb::GenCountersFSR::CounterKeyToType("B2star"+option);
        genFSR->incrementGenCounter(key, 1);                                                                                        
      } 
    } 
  }

  // Count D :                                                                                     
  std::vector< HepMC::GenParticlePtr > rootD ;  
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( rootD ) , isRootD() ) ;

  for ( iter = rootD.begin() ; iter != rootD.end() ; ++iter )
  {
    if ( signal_process_vertex)
    {
      if ( ! HepMCUtils::commonTrees( signal_process_vertex ,
                                      (*iter) -> end_vertex() ) )
        continue ; 
    }

    LHCb::ParticleID thePid( (*iter) -> pdg_id() );

    if (thePid.isMeson())
    {
      if (0 == thePid.lSpin())
      {
        if (1 == thePid.jSpin())
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("D"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Dstar"+option);
          genFSR->incrementGenCounter(key, 1); 
        } 
      }
      else
      {
        key = LHCb::GenCountersFSR::CounterKeyToType("D2star"+option);
        genFSR->incrementGenCounter(key, 1); 
      } 
    } 
  }

  // Count B:                                                                                     
  std::vector< HepMC::GenParticlePtr > endB ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( endB ) , isEndB() ) ;

  for ( iter = endB.begin() ; iter != endB.end() ; ++iter )
  {
    if ( signal_process_vertex )
    {
      if ( HepMCUtils::commonTrees( signal_process_vertex ,
                                    (*iter) -> end_vertex() ) )
        continue ;      
    }

    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;

    if ( thePid.isMeson() )                                                                                                               
    {                                                                                                                                  
      if ( thePid.pid() > 0 )
      {                                                                                                                              
        if ( thePid.hasUp() )                                                                                                         
        {                                                              
          key = LHCb::GenCountersFSR::CounterKeyToType("Bplus"+option);
          genFSR->incrementGenCounter(key, 1);                                                                                       
        }                                                                                                                          
        else if ( thePid.hasDown() )                                                                                                  
        {                                                                 
          key = LHCb::GenCountersFSR::CounterKeyToType("B0"+option);
          genFSR->incrementGenCounter(key, 1);                                                                                       
        }                                                                                                                          
        else if ( thePid.hasStrange() )                                                                                               
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Bs0"+option); 
          genFSR->incrementGenCounter(key, 1);
        }
        else if ( thePid.hasCharm() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Bcplus"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("bb"+option);
          genFSR->incrementGenCounter(key, 1); 
        } 
      }
      else
      {
        if ( thePid.hasUp() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Bminus"+option);
          genFSR->incrementGenCounter(key, 1);
        }
        else if ( thePid.hasDown() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("antiB0"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else if ( thePid.hasStrange() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("antiBs0"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else if ( thePid.hasCharm() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Bcminus"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("bb"+option);
          genFSR->incrementGenCounter(key, 1); 
        } 
      } 
    }
    else if ( thePid.isBaryon() )
    {
      if ( thePid.pid() < 0 )
      {
        key = LHCb::GenCountersFSR::CounterKeyToType("bBaryon"+option);
        genFSR->incrementGenCounter(key, 1); 
      }
      else
      {
        key = LHCb::GenCountersFSR::CounterKeyToType("antibBaryon"+option);
        genFSR->incrementGenCounter(key, 1); 
      } 
    } 
  }

  // Count D:                                                                                   
  std::vector< HepMC::GenParticlePtr > endD ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( endD ) , isEndD() ) ;

  for ( iter = endD.begin() ; iter != endD.end() ; ++iter )
  {
    if ( signal_process_vertex )
    {
      if ( HepMCUtils::commonTrees( signal_process_vertex ,
                                    (*iter) -> end_vertex() ) )
        continue ;      
    }

    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;

    if ( thePid.isMeson() )
    {
      if ( thePid.pid() > 0 )
      {
        if ( thePid.hasUp() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("D0"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else if ( thePid.hasDown() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Dplus"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else if ( thePid.hasStrange() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Dsplus"+option);
          genFSR->incrementGenCounter(key, 1);
        }
        else
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("cc"+option);
          genFSR->incrementGenCounter(key, 1); 
        } 
      }
      else
      {
        if ( thePid.hasUp() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("antiD0"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else if ( thePid.hasDown() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Dminus"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else if ( thePid.hasStrange() )
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("Dsminus"+option);
          genFSR->incrementGenCounter(key, 1); 
        }
        else
        {
          key = LHCb::GenCountersFSR::CounterKeyToType("cc"+option);
          genFSR->incrementGenCounter(key, 1); 
        } 
      } 
    }
    else if ( thePid.isBaryon() )
    {
      if ( thePid.pid() > 0 )
      {
        key = LHCb::GenCountersFSR::CounterKeyToType("cBaryon"+option);
        genFSR->incrementGenCounter(key, 1); 
      }
      else
      {
        key = LHCb::GenCountersFSR::CounterKeyToType("anticBaryon"+option);
        genFSR->incrementGenCounter(key, 1); 
      } 
    } 
  } 
}
