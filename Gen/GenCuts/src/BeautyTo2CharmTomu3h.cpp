// local
#include "BeautyTo2CharmTomu3h.h"
#include <iostream>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Kernel
#include "Kernel/ParticleID.h"
#include "GaudiKernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "GenEvent/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BeautyTo2CharmTomu3h
//
// 2017-03-10 : Stephen Ogilvy
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BeautyTo2CharmTomu3h )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeautyTo2CharmTomu3h::BeautyTo2CharmTomu3h( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
: GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin"  , m_chargedThetaMin = 10      * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax"  , m_chargedThetaMax = 400     * Gaudi::Units::mrad ) ;
  declareProperty( "MuonPtMin"        , m_muonptmin       = 0.0    * Gaudi::Units::GeV ) ;
  declareProperty( "MuonPMin"         , m_muonpmin        = 0.95   * Gaudi::Units::GeV ) ;
  declareProperty( "HadronPtMin"      , m_hadronptmin     = 0.2375 * Gaudi::Units::GeV ) ;
  declareProperty( "HadronPMin"       , m_hadronpmin      = 1.9    * Gaudi::Units::GeV ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
BeautyTo2CharmTomu3h::~BeautyTo2CharmTomu3h( ) { ; }

//=============================================================================
// Cut Function
//=============================================================================
bool BeautyTo2CharmTomu3h::applyCut( ParticleVector & theParticleVector,
                                     const HepMC::GenEvent * /* theEvent */ ,
                                     const LHCb::GenCollision * /* theHardInfo */ ) const {
  ParticleVector::iterator it ;
  int countPar = 0;
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ++it ) {
    
    // Initialise event filter conditions:
    // Count the numbers of charmed daughers looped over, so we don't accept
    // any B->D(mu3h) decay with only one charm.
    int numCharmDau=0;
    
    // These contain conditions for there being a charm daugher
    // with either a muon passing the cuts, or 3 hadrons passing the cuts.
    // Both must be true for the candidate to pass.
    bool hasMuon    = 0; // There is a charm hadron in the B daughters decaying to a muon + X, 
                         // with the muon passing selection.
    bool hasHadrons = 0; // There is a charm hadron in the B daughters with 3h in the decay products,
                         // with the hadrons passing selection.
    bool selection  = 0;
    
    // Check parent isn't stable:
    HepMC::GenVertex *EV = (*it) -> end_vertex(); 
    if ( EV == 0 ) continue;

    if(msgLevel(MSG::DEBUG)){
      countPar++;
      debug() << "=========================================\n" <<  endmsg ;
      debug() << "Parent number: " << countPar << " with ID = " << (*it)->pdg_id() << endmsg ;
    }

    // Only process particles which are beauty.
    LHCb::ParticleID parPID( (*it) -> pdg_id() ) ;
    if(!parPID.hasBottom()) continue;
    if(msgLevel(MSG::DEBUG)) debug() << "Parent contains a b quark" << endmsg ;

    // Find out if it would have passed with a simple LHCbAcceptance requirement.
    bool inAcc = 0;
    double angle = (*it)->momentum().theta();
    if (msgLevel(MSG::DEBUG)) debug() << "-- Beauty has angle = "<< angle/ Gaudi::Units::mrad << endmsg ;
    if ( ( fabs(sin(angle)) > fabs(sin(400*Gaudi::Units::mrad)) ))
    {
      if (msgLevel(MSG::DEBUG)) debug() << "-- Beauty is in LHCb acceptance." << endmsg;
      counter("#inAcc")++;
      inAcc=1;
    }
    else
    {
      if (msgLevel(MSG::DEBUG)) debug() << "-- Beauty NOT in LHCB acceptance." << endmsg;
    }

    // Loop through the B daughters
    HepMC::GenVertex::particle_iterator iterDAU;
    int countDau = 0;
    for ( iterDAU = EV->particles_begin( HepMC::children ); 
          iterDAU != EV->particles_end( HepMC::children ) ; ++iterDAU ){
      countDau++;
      if(msgLevel(MSG::DEBUG)) debug() << "   Daughter:   "
                                       << (*iterDAU) -> pdg_id()<< endmsg ;
      LHCb::ParticleID dauPID( (*iterDAU) -> pdg_id() ) ;
      
      if (!dauPID.hasCharm()) continue;
      numCharmDau++;
      if(msgLevel(MSG::DEBUG)){
        debug() << "   Daughter " << countDau << " contains a c quark." << endmsg ;
        debug() << "   Current Number of charm daughters =  " << numCharmDau << endmsg ;
        debug() << "   Looking at granddaughters of daughter: " << countDau << endmsg ;
      }
      
      // Track if any condition changes this loop.
      bool inIterHasMuon    = hasMuon;
      bool inIterHasHadrons = hasHadrons;
      // Check if the charm daughter passes the muon or hadron cuts: 
      // We're passing the immediate beauty daughter to this function, 
      // so we could be considering excited charm.
      passCuts(*iterDAU, hasMuon, hasHadrons);
      // If this didn't fulfil either criteria, maybe there is some charm further down 
      // the charm chain which can. In this case, run over the descendents and look for
      // charm. BUT only do this if neither condition changed when running the previous charm.
      if ( (inIterHasMuon != hasMuon) || (inIterHasHadrons != hasHadrons) ) continue;
      if(msgLevel(MSG::DEBUG)) debug() << "   -- Searching for charm in descendants of direct charm daughter..." 
                                       << endmsg;
      HepMC::GenVertex *CharmEV = (*iterDAU) -> end_vertex();
      HepMC::GenVertex::particle_iterator iterCharm ;
      int countCharmDesc = 0;
      for ( iterCharm = CharmEV->particles_begin( HepMC::descendants );
            iterCharm != CharmEV->particles_end( HepMC::descendants ) ; ++iterCharm )
      {
        countCharmDesc++;
        if(msgLevel(MSG::DEBUG)) debug() << "     -- Charm descendent number " << countCharmDesc
                                         << ":   "<< (*iterCharm) -> pdg_id()<< endmsg ;
        LHCb::ParticleID charmProdPID( (*iterCharm) -> pdg_id() );
        if (charmProdPID.hasCharm())
        {
          if(msgLevel(MSG::DEBUG)) debug() << "       -- Found a charm hadron. Now checking it." << endmsg;
          passCuts(*iterCharm, hasMuon, hasHadrons);
        }
      } // end of loop over charm daughter.
      
    } // end of loop over daughters.
    
    if ( hasMuon && hasHadrons ) selection = 1;
    if(msgLevel(MSG::DEBUG)){
      debug() << "\nEnd of parent summary:" << endmsg;
      debug() << "Num charm  = " << numCharmDau << endmsg;
      debug() << "hasMuon    = " << hasMuon << endmsg;
      debug() << "hasHadrons = " << hasHadrons << endmsg;
      debug() << "selection  = " << selection << endmsg;
    }

    // Before we move on, if either selection is now satisfied return true for the candidate.
    if ( selection && numCharmDau>=2 )
    {
      if(msgLevel(MSG::DEBUG)) debug() << ">>> Candidate passes selection!" << endmsg ;
      counter("#Selected")++;
      return true;
    }
    
  } // End of loop over parent particles.
  
  // If here return false:
  return false;
}


//=============================================================================
// Functions to test if Xc has a daughter muon in acceptance or if it goes to 
// >= 3 charged hadrons in acceptance
//=============================================================================
void BeautyTo2CharmTomu3h::passCuts( const HepMC::GenParticle * theSignal
                                     , bool &hasMuon, bool &hasHadrons) const 
{
  
  // Loop through descendants of charm particle:
  HepMC::GenVertex *charmEV = theSignal->end_vertex();
  HepMC::GenVertex::particle_iterator iterDESC ;
  int countDESC = 0;

  // Count the number of charged hadrons, and the subset of those passing selection.
  int numChargedHadron      = 0;
  int numChargedHadronInAcc = 0;

  for ( iterDESC = charmEV->particles_begin( HepMC::descendants ); 
        iterDESC != charmEV->particles_end( HepMC::descendants ) ; ++iterDESC ){
    countDESC++;
    if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"
                                     << countDESC << " has pid = "<< (*iterDESC)->pdg_id()<< endmsg ;
    LHCb::ParticleID descPID( (*iterDESC)->pdg_id() ) ;
    
    // First check the particles is the right type and in acceptance:
    int PID = descPID.abspid();
    if (!(PID==13 || PID==211 || PID==321 || PID==2212 )) continue;
    double angle = (*iterDESC)->momentum().theta();
    if (msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:" 
                                      << countDESC << " has angle = "<< angle/ Gaudi::Units::mrad << endmsg ;
    if ( ( fabs(sin(angle)) > fabs(sin(m_chargedThetaMax)) )
         || ( fabs(sin(angle)) < fabs(sin(m_chargedThetaMin)) )
         )  continue;
    if (msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter in in LHCb acceptance." << endmsg ;
    
    // Now here we have either a muon, proton, kaon or pion in the acceptance.
    // For muon, only skip this if we have a muon passing both, and we're just waiting
    // on the hadrons.
    if ( !(hasMuon) && PID==13){
      if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"<< countDESC 
                                       << " is a muon."<< endmsg ;
      double mpx, mpy, mpz, mpp, mpt;
      mpx = (*iterDESC)->momentum().px();
      mpy = (*iterDESC)->momentum().py();
      mpz = (*iterDESC)->momentum().pz();
      mpp = sqrt( mpx*mpx + mpy*mpy + mpz*mpz );
      mpt = sqrt( mpx*mpx + mpy*mpy);
      if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:" << countDESC 
                                       << " has P = " << mpp << "MeV, and Pt = " << mpt << "MeV." <<  endmsg ;
      if ( (mpt>m_muonptmin && mpp>m_muonpmin) )
      {
        if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"
                                         << countDESC << " has passed muon momentum cuts. \n"<< endmsg ;
        hasMuon=1;
      }
    } // end of muon search.

    // For hadrons:
    if ( !hasHadrons && ( PID==211 || PID==321 || PID == 2212) ){
      if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"<< countDESC << " is a pi or K."<< endmsg ;
      double hpx, hpy, hpz, hpp, hpt;
      hpx = (*iterDESC)->momentum().px();
      hpy = (*iterDESC)->momentum().py();
      hpz = (*iterDESC)->momentum().pz();
      hpp = sqrt( hpx*hpx + hpy*hpy + hpz*hpz );
      hpt = sqrt( hpx*hpx + hpy*hpy);
      if ( hpt>m_hadronptmin && hpp>m_hadronpmin) 
      {
        if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"
                                         << countDESC << " has passed hadron momentum cuts 1. \n"<< endmsg ;
        numChargedHadronInAcc++;
        if (numChargedHadronInAcc == 3 ) hasHadrons=1;
      }
    } // end of hadron search.
    
    /*    // Now check to see how the event flags should change.
    //if (hasMuon1 && hasHadrons1){
    //  if(msgLevel(MSG::DEBUG)) debug() << "Selection 1 passed.\n" << endmsg;
    //  selection1 = 1;
    }
    if (hasMuon2 && hasHadrons2){
      if(msgLevel(MSG::DEBUG)) debug() << "Selection 2 passed.\n" << endmsg;
      selection2 = 1;
      } */

  }

  return;
} 
