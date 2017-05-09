// $Id: BcChargedNumInLHCb.cpp,v 1.0 2017-05-04 21:28 Wenqian.Huang $
// Include files 

// local
#include "BcChargedNumInLHCb.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Kernel
#include "Kernel/ParticleID.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "GenEvent/HepMCUtils.h"
#include "MCInterfaces/IDecayTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BcChargedNumInLHCb
//
// 2017-04-11 : Wenqian Huang
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BcChargedNumInLHCb )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BcChargedNumInLHCb::BcChargedNumInLHCb( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_decayTool( 0 ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "ENumInLHCb" ,      m_enuminlhcb = 0 ) ;
  declareProperty( "MuNumInLHCb" ,     m_munuminlhcb = 0 ) ;
  declareProperty( "KNumInLHCb" ,      m_knuminlhcb = 0 ) ;
  declareProperty( "PiNumInLHCb" ,     m_pinuminlhcb = 0 ) ;
  declareProperty( "PNumInLHCb" ,      m_pnuminlhcb = 0 ) ;
  declareProperty( "HadNumInLHCb" ,    m_hadnuminlhcb = 0 ) ;
  declareProperty( "MuPt" ,            m_mupt = 0. * Gaudi::Units::MeV ) ;
  declareProperty( "HadPt" ,           m_hadpt = 0. * Gaudi::Units::MeV ) ;
  declareProperty( "DecayTool" ,       m_decayToolName = "EvtGenDecay") ;
  declareProperty( "BcPdgId" ,         m_sigBcPID = 541);
}

//=============================================================================
// Destructor 
//=============================================================================
BcChargedNumInLHCb::~BcChargedNumInLHCb( ) { ; }

//=============================================================================
// Initialize
//=============================================================================
StatusCode BcChargedNumInLHCb::initialize( ) {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize and retrieve "
                                      << m_decayToolName << " tool" << endmsg;

  if ( "" != m_decayToolName )
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;

  m_decayTool -> setSignal( m_sigBcPID ) ;

  return StatusCode::SUCCESS;

}

//=============================================================================
// Finalize
//=============================================================================
StatusCode BcChargedNumInLHCb::finalize( ) {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( 0 != m_decayTool ) release( m_decayTool );

  return GaudiTool::finalize(); 

}

//=============================================================================
// Acceptance function
//=============================================================================
bool BcChargedNumInLHCb::applyCut( ParticleVector & theParticleVector ,
                                  const HepMC::GenEvent * theEvent  ,
                                  const LHCb::GenCollision */* theHardInfo */ )
  const {
  
  // To see whether the B_c is in the Event or not
  //--------------------------------------------------------------------
  theParticleVector.clear( ) ;
  HepMC::GenEvent::particle_const_iterator it ;
  for ( it = theEvent -> particles_begin() ; it != theEvent -> particles_end() ; ++it )
  {
    if ( ( abs( (*it) -> pdg_id() ) == m_sigBcPID ) &&
         ( LHCb::HepMCEvent::DocumentationParticle != (*it) -> status() ) &&
         ( HepMCUtils::IsBAtProduction( *it ) ) )
        theParticleVector.push_back( *it ) ;
  }

  std::sort( theParticleVector.begin() , theParticleVector.end() , 
             HepMCUtils::compareHepMCParticles ) ;
  
  if ( theParticleVector.empty() ) return false ;

  // To decay the signal particle
  //--------------------------------------------------------------------  
  bool hasFlipped = false ;
  HepMC::GenParticle * theSignal ;
  theSignal = theParticleVector.front() ;  
  m_decayTool -> generateSignalDecay( theSignal , hasFlipped ) ;
  
  // To do the cut
  //--------------------------------------------------------------------  
  ParticleVector::iterator itp ;
  for ( itp = theParticleVector.begin() ; 
        itp != theParticleVector.end() ; ) {    
    if ( ! passCuts( *itp ) ) {
      itp = theParticleVector.erase( itp ) ;
    } else ++itp ;
  }
  
  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if all daughters are in acceptance
//=============================================================================
bool BcChargedNumInLHCb::passCuts( const HepMC::GenParticle * theSignal ) 
  const 
  {
    HepMC::GenVertex * EV = theSignal -> end_vertex() ;
    if ( 0 == EV ) return false ;

    // check if pz of the Bc is positive
    if ( theSignal -> momentum().pz() < 0. ) return false ;
  
    typedef std::vector< HepMC::GenParticle * > Particles ;
    Particles stables ;
    HepMC::GenVertex::particle_iterator iter ;
  
    for ( iter = EV -> particles_begin( HepMC::descendants ) ; iter != EV -> particles_end( HepMC::descendants ) ; ++iter )
    {
      if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
    }  
  
    if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;
  
    double angle( 0. ) ;
    double firstpz = stables.front() -> momentum().pz() ;
  
    debug() << "New event" << endmsg ;

    int ecount = 0;
    int mucount = 0;
    int kcount = 0;
    int picount = 0;
    int pcount = 0;

    for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ; ++it ) 
    {
    
      debug() << "Check particle " << (*it) -> pdg_id() << " with angle " 
            << (*it) -> momentum().theta() / Gaudi::Units::mrad << " mrad." << endmsg ;
    
      // Remove neutrinos
      if ( ( 12 == abs( (*it) -> pdg_id() ) ) ||  ( 14 == abs( (*it) -> pdg_id() ) ) || ( 16 == abs( (*it) -> pdg_id() ) ) ) continue ;
    
      // Don't use daughters of Lambda and KS:
      HepMC::GenParticle * theParent ;
      theParent =  *( (*it) -> production_vertex() -> particles_in_const_begin() ) ;
      if ( 3122 == abs( theParent -> pdg_id() ) ) continue ;
      if ( 310 == theParent -> pdg_id() ) continue ;
    
      // Consider only gammas from pi0 and eta
      if ( 22 == (*it) -> pdg_id() ) 
      {
        if ( ( 111 != theParent -> pdg_id() ) && ( 221 != theParent -> pdg_id() ) ) continue ;
      }
    
      // All particles in same direction
      if ( 0 > ( firstpz * ( (*it) -> momentum().pz() ) ) ) return false ;
    
      angle = (*it) -> momentum().theta() ;
    
      LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
      if ( 0 == pid.threeCharge() ) continue ; 
      else 
      {
        if ( ( fabs( angle ) <= fabs( m_chargedThetaMax ) ) && ( fabs( angle ) >= fabs( m_chargedThetaMin ) ) )
        {
          switch ( abs( (*it) -> pdg_id() ) )
          {
            case 11 : { ecount++ ; break ; }
            case 13 : 
            {
              if ( (*it) -> momentum().perp() >= m_mupt )
              mucount++ ;
              break ;
            }
            case 321 : { kcount++ ; break ; }
            case 211 : 
            {
              if ( (*it) -> momentum().perp() >= m_hadpt )
              picount++ ;
              break ;
            }
            case 2212 : { pcount++ ; break ; }
            default : { break ; }
	  }
        }
        else continue ; 
      }
    }
//    debug() << "e:" << ecount << " mu:" << mucount << " K:"<< kcount << " pi:" << picount << "p:" << pcount << endmsg ;
//    if (( ecount >= m_enuminlhcb ) &&	( mucount >= m_munuminlhcb ) &&	( kcount >= m_knuminlhcb ) && ( picount >= m_pinuminlhcb ) && ( pcount >= m_pnuminlhcb ) )
      if (( ecount >= m_enuminlhcb ) &&   ( mucount >= m_munuminlhcb ) && ( kcount + picount + pcount >= m_hadnuminlhcb ) )
    {
      debug() << "Event passed !" << endmsg ;
      return true ;
    }
    else return false ;
  }
