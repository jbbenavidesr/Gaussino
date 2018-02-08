// Include files 

// local
#include "DaughtersInLHCbAndCutsForDstar3pi.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Transform4DTypes.h"

// from Kernel
#include "Kernel/ParticleID.h"
#include "GaudiKernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenRanges.h"

// from Generators
#include "GenEvent/HepMCUtils.h"

// from STL
#include <algorithm>

//-----------------------------------------------------------------------------
// Implementation file for class : DaughtersInLHCbAndCutsForDstar3pi
//
// 2015-04-23 : Guy Wormser
// 2017-12-14 : Dawid Gerstel, Olivier Leroy, Guy Wormser, Adam Morris
// Update as discussed in the simulation meeting:
// https://indico.cern.ch/event/577949/contributions/2823100/attachments/1574160/2485093/gerstel_Sim2.pdf
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndCutsForDstar3pi )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndCutsForDstar3pi::DaughtersInLHCbAndCutsForDstar3pi( const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin"   , m_chargedThetaMin   = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax"   , m_chargedThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "D0PtCuts"          , m_d0ptCut           = 1500 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPtMinCut" , m_daughtersptminCut = 150 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPMinCut"  , m_daughterspminCut  = 1000 * Gaudi::Units::MeV ) ;
  declareProperty( "SoftPiPtCut"       , m_softpiptCut       = 100 * Gaudi::Units::MeV ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCbAndCutsForDstar3pi::~DaughtersInLHCbAndCutsForDstar3pi( ) { ; }

//=============================================================================
// AndWithMinP function
//=============================================================================
bool DaughtersInLHCbAndCutsForDstar3pi::applyCut( ParticleVector & theParticleVector, 
						  const HepMC::GenEvent * /* theEvent */ ,
						  const LHCb::GenCollision * /* theHardInfo */ ) const {
  ParticleVector::iterator it ;
  
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {    
    // Check that the signal is the D* sig
    if ( abs( (*it) -> pdg_id() ) !=413  ) 
      Exception( "The signal is not the D*" ) ;
    
    if ( ! passCuts( *it ) ) {
      it = theParticleVector.erase( it ) ;
    } else ++it ;
  }

  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if the D* and 3 pi from the daughters are in acceptance
//=============================================================================
bool DaughtersInLHCbAndCutsForDstar3pi::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( EV == nullptr) return true ;

  typedef HepMC::GenParticle * Particle;
  typedef std::vector<Particle> Particles ;
  HepMC::GenVertex::particle_iterator iter ;


  /************************************************************
   * Check if D* has a b-hadron in its ancestors
   ************************************************************/

  // Helper to check pid
  const auto pid_short = [](const Particle & part) { return abs((*part).pdg_id())%10000; };

  // Helper to check if a particle is a b-hadron
  const auto is_it_b_had = [pid_short](const Particle & part) {
    const int pid = pid_short(part);
    return ((pid > 500 && pid < 600) || 
	    (pid > 5000 && pid < 6000));
  };

  // If none of the D*'s ancestors is a b-hadron, reject the event
  if (std::none_of(EV->particles_begin(HepMC::ancestors), 
		   EV->particles_end(HepMC::ancestors),
		   is_it_b_had))
    return false;


  /**************************************************
   * Scan the D* daughters in the acceptance:
   * look for D0 and pi+ in D*+ -> D0 pi+.
   **************************************************/
  Particle theSoftPion( 0 ) ;
  Particle theD0( 0 ) ;
  Particles dst_descendants_stable;
  HepMC::GenVertex::particles_out_const_iterator it_out ;
  for (const auto dst_child : EV->particles(HepMC::children)) {

    // Get the id
    const int dst_daughter_id = abs(dst_child -> pdg_id());

    // Skip photons
    if (dst_daughter_id == 22) continue;

    // Fill all daughters to D* stable descendants
    dst_descendants_stable.push_back(dst_child) ;

    // Get the D0
    if ( dst_daughter_id == 421 ) theD0 = dst_child ;

    // Get the soft pion
    else if ( dst_daughter_id == 211) theSoftPion = dst_child ;
  }

  if ( theD0 == nullptr )
    Exception( "No D0 in the signal decay chain !" ) ;

  if ( theSoftPion == nullptr ) 
    Exception( "No soft pion in the decay chain !" ) ;  


  /****************************************************
   * Scan the daughters of D0:
   * look for K- pi+ in D0 -> K- pi+
   ***************************************************/
   HepMC::GenVertex * EVD0 = theD0 -> end_vertex() ;
  if (EVD0 == nullptr) Exception( "The D0 has no daughters" ) ;
  
  int nKD0 = 0;
  int npiD0 = 0;
  Particles d0daughters ;
  for (const auto d0_child : EVD0->particles(HepMC::children)) {

    const int d0_daughter_id = abs(d0_child -> pdg_id());

    // Skip photons
    if ( d0_daughter_id == 22) continue;

    d0daughters.push_back(d0_child) ;

    // Get the pi
    if (d0_daughter_id == 211) npiD0++;
    // Get the K
    else if (d0_daughter_id == 321) nKD0++;
  }

  // Append D0 children to D* descendants (for convenience)
  dst_descendants_stable.insert(dst_descendants_stable.end(), d0daughters.begin(), d0daughters.end());

  // Check that the D0 goes into K- pi+
  if ( nKD0 != 1) Exception( "No single kaon from the D0!" ) ;  
  if ( npiD0 != 1) Exception( "No single pion from the D0!" ) ;  

  /************************************************************************************
   * Start the cuts 
   ***********************************************************************************/
  if (msgLevel(MSG::DEBUG))
    debug() << "New event. Starting the cuts" << endmsg ;

  // Check if particles have same direction and lie in the angular acceptance
  double firstpz = dst_descendants_stable.front() -> momentum().pz() ;
  for (const auto dst_descendant : dst_descendants_stable) {

    // Check if all particles are in the same direction
    if ((firstpz * ( dst_descendant -> momentum().pz() ) ) < 0 ) return false ;


    // Check if the particles lie within the angular acceptance
    double angle = dst_descendant -> momentum().theta() ;

    if (msgLevel(MSG::DEBUG)) {
      debug() << "Check particle " << dst_descendant -> pdg_id() << " with angle " 
	      << angle / Gaudi::Units::mrad 
	      << " mrad." << endmsg ;
    }

    LHCb::ParticleID pid( dst_descendant -> pdg_id() ) ;
    // Only charged tracks to be considered
    if (pid.threeCharge() != 0) {
      if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) || 
	   fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) ) 
	return false ;
    }
  }

  // Apply pT cut for D0
  if (msgLevel(MSG::DEBUG)) 
    debug() << "D0 pT = " << theD0 -> momentum().perp() << endmsg ;
  if ( theD0 -> momentum().perp() < m_d0ptCut ) return false ;
  
  // Apply pT cut for the soft pion
  if (msgLevel(MSG::DEBUG)) 
    debug() << "Soft pion pT = " << theSoftPion -> momentum().perp() << endmsg ;
  if ( theSoftPion -> momentum().perp() < m_softpiptCut ) return false ;


  // Compute the minpt and minp. Start with higher-than-possible values.  
  double minpt = 14.*Gaudi::Units::TeV ;
  double minp  = 14.*Gaudi::Units::TeV ; 

  for ( const auto d0_child : d0daughters) {

    if (msgLevel(MSG::DEBUG)) {
      debug() << "Daughter pT = " << d0_child -> momentum().perp() 
	      << " p = " << d0_child -> momentum().rho() << endmsg ;
    }

  if ( d0_child -> momentum().perp() < minpt ) minpt = d0_child -> momentum().perp() ;
  if ( d0_child -> momentum().rho() < minp )   minp  = d0_child -> momentum().rho() ;
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "Min Pt = " << minpt
	    << " Min P = " << minp << endmsg ;
  }


  if ( minpt < m_daughtersptminCut || 
       minp  < m_daughterspminCut ) 
    return false ;
  

  /**************************************************************************************
   * Loop over the stable particles in the full decay chain (i.e. all D* relatives).
   * Count all the pions coming from a non-excited b-hadron that pass the acceptance cut.
   * This will include (at least) 3pi in addition to 2pi in D* descendants, 
   * so we ask for >= 5 pions in the acceptance.
   * Reject "long" pions, i.e. coming from K_S^0 -> pi+pi- or Lambda_0 -> p pi.
   *************************************************************************************/
  int pi_from_dst = 0;
  int pi_from_d0 = 0;
  int Npions_inacc=0;

  // List of all stable b-hadrons
  std::vector<int> stable_b_hadrons =  { 511, 521, 531, 5122, 541, 5112, 5212, 5222, 
					 5132, 5232, 5312, 5322, 5332, 5142, 5242,
					 5412, 5342, 5442, 5512, 5522, 5532, 5542, 5554};

  for (const auto relative : EV->particles(HepMC::relatives)) {
    // Pick only stable particles
    if (relative -> end_vertex()) continue;

    // Is it a pion?
    if ( 211 != abs(relative -> pdg_id() )) continue;

    // Has it origin vertex?
    HepMC::GenVertex * OV = relative -> production_vertex() ;
    if (OV == nullptr) continue;

    if (msgLevel(MSG::DEBUG))
      debug()<<"Relative pion with z_vtx == " << OV->position().z()<<endmsg;

    // Go over pi's ancestors and check if there's a stable b-hadron
    // Helper to check if a particle is a b-hadron
    const auto is_it_stable_b_had = [pid_short, stable_b_hadrons](const Particle & part) {
      const int pid = pid_short(part);
      // Check if a valid stable b-hadron has been found
      return (std::find(stable_b_hadrons.begin(), stable_b_hadrons.end(), pid) 
	      != stable_b_hadrons.end());
    };
    
    // If none of the pi's ancestors is a b-hadron, reject this pion
    if (std::none_of(OV->particles_begin(HepMC::ancestors), 
		     OV->particles_end(HepMC::ancestors),
		     is_it_stable_b_had))
      continue;


    // Cut off "long" pions, e.g. from K_S^0 -> pi+pi- or Lambda_0 -> p pi
    const auto PV_z = EV->parent_event()->beam_particles().first->end_vertex()->position().z();
    if( (relative->production_vertex()->position().z() - PV_z)>200.) continue;

    // Check angular acceptance
    const double angle = relative -> momentum().theta() ;
    if ( fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) ||
	 fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) 
      continue;

    Npions_inacc++;

    // Count pions coming from D*(2010)+- and (~)D0 to debug (duplication issue)
    Particle theParent = *(OV -> particles_in_const_begin()) ;
    if(abs(theParent->pdg_id()) == 413) pi_from_dst++;
    if(abs(theParent->pdg_id()) == 421) pi_from_d0++;
  }

  if (msgLevel(MSG::DEBUG))
    debug() << "Number of  pions in the D* family that passed cuts " <<Npions_inacc<<endmsg;
  if (Npions_inacc<5) return false ;

  if (msgLevel(MSG::DEBUG))
    debug() << "Event passed !" << endmsg ;

  return true ;
}


