// Include files

// local
#include "DaughtersInLHCbAndCutsForLambdacD.h"

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

// from Generators
#include "GenEvent/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaughtersInLHCbAndCutsForLambdacD
//
// 2017-01-09 : Victor Renaudin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndCutsForLambdacD )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndCutsForLambdacD::DaughtersInLHCbAndCutsForLambdacD( const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin"   , m_chargedThetaMin   = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax"   , m_chargedThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMin"   , m_neutralThetaMin   = 5 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMax"   , m_neutralThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "LambdacPtCuts"     , m_lcptCut           = 1500 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPtMinCut" , m_daughtersptminCut = 150 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPtMaxCut" , m_daughtersptmaxCut = 150 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPMinCut"  , m_daughterspminCut  = 1000 * Gaudi::Units::MeV ) ;
  declareProperty( "LambdacCTauCut"    , m_lcctauCut         = -1. ) ;
}

//=============================================================================
// Destructor
//=============================================================================
DaughtersInLHCbAndCutsForLambdacD::~DaughtersInLHCbAndCutsForLambdacD( ) { ; }

//=============================================================================
// AndWithMinP function
//=============================================================================
bool DaughtersInLHCbAndCutsForLambdacD::applyCut( ParticleVector & theParticleVector ,
                                               const HepMC::GenEvent * /* theEvent */ ,
                                               const LHCb::GenCollision * /* theHardInfo */ )
  const {
  ParticleVector::iterator it ;

  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {
      if( msgLevel (MSG::DEBUG))debug()<<" particle type "<<(*it)->pdg_id()<<" vtx "<<(*it)->end_vertex()<<endmsg;
      // Check that the signal is the Lambda_c sig
      if ( abs( (*it) -> pdg_id() ) !=4122  ) {
          Exception( "The signal is not our dear Lambdac" ) ;
          }

      if ( ! passCuts( *it ) ) {
          it = theParticleVector.erase( it ) ;
      } else ++it ;
  }
  return ( ! theParticleVector.empty() ) ;
  }

//=============================================================================
// Functions to test if the Lambda_c and 3 pi from the daughters are in acceptance
//=============================================================================
bool DaughtersInLHCbAndCutsForLambdacD::passCuts( const HepMC::GenParticle * theSignal ) const {
    //theSignal = Lambda_c
    HepMC::GenVertex * EV = theSignal -> end_vertex() ;
    if ( 0 == EV ) return true ;

    typedef std::vector< HepMC::GenParticle * > Particles ;
    Particles unstables, stables ;
    Particles lcparents , lcdaughters ;

    // first check on the Lambda_c origin : it should come from a B decay
    HepMC::GenVertex::particle_iterator iter ;
    int bOK=0 ;
    double zB=-999. ;
    double zB2=-999. ;

    if( msgLevel (MSG::DEBUG))debug() << "Check the Lambda_c ancestor " <<endmsg;
    for ( iter = EV -> particles_begin( HepMC::ancestors ) ;
            iter != EV -> particles_end( HepMC::ancestors ) ; ++iter ) {
        lcparents.push_back( *iter );

        // is there a B
        if ( int(abs( (*iter) -> pdg_id() )) % 10000 > 500 && int(abs( (*iter) -> pdg_id() )) % 10000 < 600 )bOK++;
        if ( int(abs( (*iter) -> pdg_id() )) % 10000 > 5000 && int(abs( (*iter) -> pdg_id() )) % 10000 < 6000 )bOK++;
    }

    if( msgLevel (MSG::DEBUG))debug() <<"start of Lambda_c parent loop with "<<bOK<<" B ancestors"<<endmsg;
    if(bOK==0) return false;

    for ( Particles::const_iterator it = lcparents.begin() ; it != lcparents.end() ;
        ++it ) {

        if( msgLevel (MSG::DEBUG))debug()<<" particle type "<<(*it)->pdg_id()<<" vtx "<<(*it)->end_vertex()<<endmsg;

        if((*it)->end_vertex()==0)continue;

        if( msgLevel (MSG::DEBUG))debug()<<"vtx "<<(*it)->end_vertex()->position().z()<<endmsg;

        if ( ( ( int(abs( (*it) -> pdg_id() )) % 10000 > 500 && int(abs( (*it) -> pdg_id() )) % 10000 < 600 ) ||
                    ( int(abs( (*it) -> pdg_id() )) % 10000 > 5000 && int(abs( (*it) -> pdg_id() )) % 10000 < 6000 ) )
                && ( zB < (*it)->end_vertex()->position().z() ) ){
            zB = (*it)->end_vertex()->position().z();
        }
    }

    if( msgLevel (MSG::DEBUG))debug() << "N beauty in Lambda_c ancestors "<<bOK
        <<" vertex position "<<zB<<endmsg;

    // daughters of Lambda_c
    int npLc=0;
    int nKLc=0;
    int npiLc=0;

    //ask all Lambda_c daughters in acceptance
    for ( iter = EV -> particles_begin( HepMC::descendants ) ;
            iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {

        // Fill all daughters
        if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;

        if ( 0 == (*iter) -> end_vertex() )
        if ( 22 != (*iter) -> pdg_id() ) lcdaughters.push_back( *iter ) ;
        if ( 2212 == abs(int((*iter) -> pdg_id())) )npLc++;
        if ( 321 == abs(int((*iter) -> pdg_id() )))nKLc++;
        if ( 211 == abs(int((*iter) -> pdg_id() )))npiLc++;
    }

    if ( stables.empty() )
        Exception( "Signal has no stable daughters !" ) ;

    if ( lcdaughters.empty() )
        Exception( "No Lc daughters in signal chain !" );
    //check that the Lambda_c goes into p K pi
    if( msgLevel (MSG::DEBUG))debug() << "Check Lc daughters N(p) = "
        << npLc<<" N(K) = "<< nKLc<<" N(pi) = "<<npiLc<<endmsg;
    if(npLc*nKLc*npiLc!=1) return false;

    double angle( 0. ) ;
    double firstpz = stables.front() -> momentum().pz() ;

    if( msgLevel (MSG::DEBUG))debug() << "New event" << endmsg ;

    for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
            ++it ) {
        if( msgLevel (MSG::DEBUG))debug() << "Check particle " << (*it) -> pdg_id() << " with angle "
            << (*it) -> momentum().theta() / Gaudi::Units::mrad
            << " mrad." << endmsg ;

        // All particles in same direction
        if ( 0 > ( firstpz * ( (*it) -> momentum().pz() ) ) ) return false ;

        angle = (*it) -> momentum().theta() ;
        LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
        //only charged tracks to be considered
        if ( 0 != pid.threeCharge() ) {
            if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) )
                return false ;
            if ( fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) )
                return false ;
        }
    }
    // Now check other cuts
    if( msgLevel (MSG::DEBUG))debug() << "Check other cuts" << endmsg ;

    if( msgLevel (MSG::DEBUG))debug() << "Lc pT = " << theSignal -> momentum().perp() << endmsg ;
    if ( theSignal -> momentum().perp() < m_lcptCut ) return false ;

    double minpt = 14.*Gaudi::Units::TeV ;
    double maxpt = 0. ;
    double minp  = 14.*Gaudi::Units::TeV ;

    for ( Particles::const_iterator it = lcdaughters.begin() ; it != lcdaughters.end() ;
            ++it ) {
        if( msgLevel (MSG::DEBUG))debug() << "Daughter pT = " << (*it) -> momentum().perp()
            << " p = " << (*it) -> momentum().rho() << endmsg ;

        if ( (*it) -> momentum().perp() > maxpt )
            maxpt = (*it) -> momentum().perp() ;
        if ( (*it) -> momentum().perp() < minpt )
            minpt = (*it) -> momentum().perp() ;
        if ( (*it) -> momentum().rho() < minp )
            minp = (*it) -> momentum().rho() ;
    }

    if( msgLevel (MSG::DEBUG))debug() << "Min Pt = " << minpt
        << " Max Pt = " << maxpt
        << " Min P = " << minp << endmsg ;

    if ( minpt < m_daughtersptminCut ) return false ;
    if ( maxpt < m_daughtersptmaxCut ) return false ;
    if ( minp  < m_daughterspminCut  ) return false ;

    // now look for 3 pions in the Lambda_c family
    if( msgLevel (MSG::DEBUG))debug() << "Check for 3 pions in addition to Lambda_c " <<zB<<endmsg;
    int Npions_inacc=0;

    for ( iter = EV -> particles_begin( HepMC::relatives) ;
            iter != EV -> particles_end( HepMC::relatives ) ; ++iter ) {

        if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
        if ( 0 != (*iter) -> end_vertex() ) unstables.push_back( *iter ) ;

        // identify z of second B
        if ( int(abs( (*iter) -> pdg_id() ))%10000> 500&& int(abs( (*iter) -> pdg_id() ))%10000<600 ){
            if ( zB2<= (*iter)->end_vertex()->position().z() && zB != (*iter)->end_vertex()->position().z() ) zB2 = (*iter)->end_vertex()->position().z();
        }
        if ( int(abs( (*iter) -> pdg_id() ))%10000> 5000&& int(abs( (*iter) -> pdg_id() ))%10000<6000 ){
            if ( zB2<= (*iter)->end_vertex()->position().z() && zB != (*iter)->end_vertex()->position().z() ) zB2 = (*iter)->end_vertex()->position().z();
        }
    }
    //now loop on the unstable particles
    //we  look at all the children of the B ancestor by requiring z>zB
    //we ask a double charm event but no Ds
    int nD = 0;

    //the family contains both B.. we have to request that the second D comes from the same B
    for ( Particles::const_iterator it = unstables.begin() ; it != unstables.end() ;
            ++it ) {
        if ((*it)->production_vertex()==0) continue;
        if( msgLevel (MSG::DEBUG))debug()<<"particle relative "<<(*it) -> pdg_id()<<" z "
            << (*it)->production_vertex()->position().z()<<endmsg;

        if( fabs((*it)->production_vertex()->position().z()-zB)>0.01) continue;
        if ( 421 == abs((*it) -> pdg_id() ))nD++;
        if ( 411 == abs((*it) -> pdg_id() ))nD++;
    }
    if (nD<1) return false;

    //now loop on the stable particles
    //we  look at all the children of the B ancestor by requiring z>zB or z>zB2
    //this will include the 3 Lambda_c daughters so we ask 4 pions in the acceptance
    for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
            ++it ) {
        if ((*it)->production_vertex()==0) continue;

        if( msgLevel (MSG::DEBUG))debug()<<"particle relative "<<(*it) -> pdg_id()<<" z "
            << (*it)->production_vertex()->position().z()<<endmsg;

        if( (*it)->production_vertex()->position().z()<zB && (*it)->production_vertex()->position().z()<zB2 ) continue;

        if( (*it)->production_vertex()->position().z()>200.) continue;
        if ( 211 != abs((*it) -> pdg_id() )) continue;

        angle = (*it) -> momentum().theta() ;

        if ( fabs( sin( angle ) ) < fabs(  sin( m_chargedThetaMin ) ) ) continue;
        if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) continue;

        Npions_inacc++;
    }

    if( msgLevel (MSG::DEBUG)) debug() << "Number of  pions in the Lambda_c family that passed "
    <<Npions_inacc<<endmsg;

    if (Npions_inacc<4) return false ;
    if( msgLevel (MSG::DEBUG)) debug() << "Event passed !" << endmsg ;

    return true ;
}

