// local
#include "BeautyTomuCharmTo3h.h"
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
// Implementation file for class : BeautyTomuCharmTo3h
//
// 2017-03-10 : Stephen Ogilvy
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BeautyTomuCharmTo3h)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeautyTomuCharmTo3h::BeautyTomuCharmTo3h( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
: GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin"  , m_chargedThetaMin = 10      * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax"  , m_chargedThetaMax = 400     * Gaudi::Units::mrad ) ;
  declareProperty( "MuonPtMin"       , m_muonptmin        = 0.0    * Gaudi::Units::GeV ) ;
  declareProperty( "MuonPMin"        , m_muonpmin         = 0.95   * Gaudi::Units::GeV ) ;
  declareProperty( "HadronPtMin"     , m_hadronptmin      = 0.2375 * Gaudi::Units::GeV ) ;
  declareProperty( "HadronPMin"      , m_hadronpmin       = 1.9    * Gaudi::Units::GeV ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
BeautyTomuCharmTo3h::~BeautyTomuCharmTo3h( ) { ; }

//=============================================================================
// Cut Function
//=============================================================================
bool BeautyTomuCharmTo3h::applyCut( ParticleVector & theParticleVector,
                                    const HepMC::GenEvent * /* theEvent */ ,
                                    const LHCb::GenCollision * /* theHardInfo */ ) const {
  ParticleVector::iterator it ;
  int countPar = 0;
  if(msgLevel(MSG::DEBUG))
  {
    debug() << "+++++++++++++++++++++++++++++++++" <<  endmsg ;
    debug() << "BeautyTomuCharmTo3h processing new ParticleVector." << endmsg;
  }
  
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ++it ) {
    
    // Initialise event filter conditions:
    // Some containers for numbers of types of daughter.
    int numCharmDau = 0; // number of charmed daughters
    int countDau    = 0; // number of Beauty HepMC::children
    
    // Now conditionals for the selections
    bool hasMuon    = 0; // There is a muon or tauon->muon in the Bs daughters in the acceptance,
                         // with the muon passing selection.
    bool hasHadrons = 0; // There is a charm hadron in the B daughters with 3h in the decay products,
                          // with the hadrons passing selection.
    // These conditions are the ANDs of the above muon and hadron conditions.
    // As soon as one becomes true, we want to pass the candidate and move on
    // to save time.
    bool selection  = 0;
    
    // Check parent isn't stable:
    HepMC::GenVertex *EV = (*it) -> end_vertex(); 
    if ( EV == 0 ) continue;

    if(msgLevel(MSG::DEBUG)){
      countPar++;
      debug() << "\n=========================================" <<  endmsg ;
      debug() << "-- Parent number: " << countPar << " with ID = " << (*it)->pdg_id() << endmsg ;
    }

    // Only process particles which contain beauty.
    LHCb::ParticleID parPID( (*it) -> pdg_id() ) ;
    if(!parPID.hasBottom()) continue;
    if(msgLevel(MSG::DEBUG)) debug() << "-- Parent has a b quark, processing daughters:" << endmsg ;

    // Find out if it would have passed with a simple LHCbAcceptance requirement.
    bool inAcc = 0;
    double angle = (*it)->momentum().theta();
    if (msgLevel(MSG::DEBUG)) debug() << "-- Beauty has angle = "<< angle/ Gaudi::Units::mrad << endmsg ;
    if ( ( fabs(sin(angle)) < fabs(sin(400*Gaudi::Units::mrad)) ))
    {
      if (msgLevel(MSG::DEBUG)) debug() << "-- Beauty is in LHCb acceptance." << endmsg;
      counter("#inAcc")++;
      inAcc=1;
    }
    else
    {
      if (msgLevel(MSG::DEBUG)) debug() << "-- Beauty not in LHCB acceptance." << endmsg;
    }
    
    // Loop through the beauty immediate daughters using HepMC::children
    // to find the lepton and the charm hadron.
    HepMC::GenVertex::particle_iterator iterDAU ;
    for ( iterDAU = EV->particles_begin( HepMC::children ); 
          iterDAU != EV->particles_end( HepMC::children ) ; ++iterDAU ){
      countDau++;
      if(msgLevel(MSG::DEBUG)) debug() << "   Daughter:   "<< (*iterDAU) -> pdg_id()<< endmsg ;
      LHCb::ParticleID dauPID( (*iterDAU) -> pdg_id() ) ;
      int dPID = dauPID.abspid();
      
      // If it's a muon, check the kinematics.
      if (dPID==13)
      {
        passMuonKinematics(*iterDAU, hasMuon);
      }
      // If it's a tauon, check there is a muon from it, and if so its kinematics.
      else if (dPID==15)
      {
        passMuonFromTauonKinematics(*iterDAU, hasMuon);
      }
      // Or if it's a charm hadron, first try passing the specific hadron itself.
      // If this doesn't work, search through the descendants to find any non-excited charm 
      // decaying weakly to 3h.
      else if (dauPID.hasCharm())
      {
        numCharmDau++;
        if(msgLevel(MSG::DEBUG))
        {
          debug() << "   -- Daughter " << countDau << " contains a c quark." << endmsg ;
          debug() << "   ---- current num of charm daughters =  " << numCharmDau << endmsg ;
        }
        // Try passing the charm hadron directly from the beauty itself.
        passCharmTo3hKinematics(*iterDAU, hasHadrons);
        
        // If neither selection is passed at this point try iterating through the 
        // descendants of the charm directly from the beauty.
        if ( !hasHadrons )
        {
          if(msgLevel(MSG::DEBUG)) debug() << "   -- Searching for charm in descendants of direct charm daughter..." 
                                           << endmsg;
          HepMC::GenVertex *CharmEV = (*iterDAU) -> end_vertex();
          HepMC::GenVertex::particle_iterator iterCharm ;
          int countCharmDesc = 0;
          for ( iterCharm = CharmEV->particles_begin( HepMC::descendants );
                iterCharm != CharmEV->particles_end( HepMC::descendants ) ; ++iterCharm )
          {
            // Skip if we've already found one.
            if ( hasHadrons ) continue;
            countCharmDesc++;
            if(msgLevel(MSG::DEBUG)) debug() << "     -- Charm descendent number " << countCharmDesc
                                             << ":   "<< (*iterCharm) -> pdg_id()<< endmsg ;
            LHCb::ParticleID charmProdPID( (*iterCharm) -> pdg_id() );
            if (charmProdPID.hasCharm())
            {
              if(msgLevel(MSG::DEBUG)) debug() << "       -- Found a charm hadron. Now checking it." << endmsg;
              passCharmTo3hKinematics(*iterCharm, hasHadrons);
            }
            
          } // end of loop over charm daughters
        } // end of descendent checking code
      } // end of charm checking code
    }
    
    // Finished loop over daughters. Evaluate selections.
    if (hasMuon && hasHadrons)
    {
      counter("#Selected")++;
      selection = 1;
    }
    
    if(msgLevel(MSG::DEBUG)){
      debug() << "--------------\nEnd of parent summary:" << endmsg;
      debug() << "Num charm daughters  = " << numCharmDau << endmsg;
      debug() << "Num beauty daughters = " << countDau << endmsg;
      debug() << "hasMuon              = " << hasMuon << endmsg;
      debug() << "hasHadrons           = " << hasHadrons << endmsg;
      debug() << "selection bool       = " << selection << endmsg;
    }
    
    if (selection)
    {
      if (!inAcc)
      {
        if(msgLevel(MSG::DEBUG)) debug() << "-- Candidate is reconstructible"
                                         << ", but would be excluded by LHCbAcceptance!" << endmsg ;
        counter("#SelectedWithBeautyNotInAcc")++;
      }
      if(msgLevel(MSG::DEBUG)) debug() << ">>> End of cut tool, candidate passes!\n" << endmsg;
      return true;
    }
  } // End of loop over parent particles.
  
  // If here return false:
  if(msgLevel(MSG::DEBUG)) debug() << ">>> End of cut tool, candidate does not pass!" << endmsg;
  counter("#FailedSelection")++;
  return false;
}

// Function to check the muon passes the acceptance cuts.
void BeautyTomuCharmTo3h::passMuonKinematics(const HepMC::GenParticle * theMuon,
                                             bool &hasMuon) const
{
  // At this point we know we have a muon.
  // Check it passes the angular cuts.
  double angle = (theMuon)->momentum().theta();
  if (msgLevel(MSG::DEBUG)) debug() << "   -- Muon has angle = "<< angle/ Gaudi::Units::mrad << endmsg ;
  if ( ( fabs(sin(angle)) > fabs(sin(m_chargedThetaMax)) )
       || ( fabs(sin(angle)) < fabs(sin(m_chargedThetaMin)) )
       ){
    if (msgLevel(MSG::DEBUG)) debug() << "     -- Muon is NOT in LHCb acceptance." << endmsg ;
    return;
  }
  if (msgLevel(MSG::DEBUG)) debug() << "   -- Muon is in LHCb acceptance." << endmsg ;
  
  // Now check it passes the momentum cuts.
  double mpx, mpy, mpz, mpp, mpt;
  mpx = (theMuon)->momentum().px();
  mpy = (theMuon)->momentum().py();
  mpz = (theMuon)->momentum().pz();
  mpp = sqrt( mpx*mpx + mpy*mpy + mpz*mpz );
  mpt = sqrt( mpx*mpx + mpy*mpy);
  if(msgLevel(MSG::DEBUG)) debug() << "   -- Muon has P = " << mpp << "MeV, and Pt = " 
                                   << mpt << "MeV." <<  endmsg ;
  // If the muon passes the cuts for the appropriate selection, set the flag to true.
  if ( mpt>m_muonptmin && mpp>m_muonpmin )
  {
    if(msgLevel(MSG::DEBUG)) debug() << "   -- Muon has passed kinematic cuts."<< endmsg ;
    hasMuon=1;
  }
  return;
}

// Function to check a tauon particle decays to mu nu nu, and then to pass the muon kinematics to the 
// dedicated function for muons.
void BeautyTomuCharmTo3h::passMuonFromTauonKinematics(const HepMC::GenParticle * theTauon,
                                                      bool &hasMuon) const
{
  // Iterate over the tauon daughters. If we find a pion at all, exit the function immediately.
  HepMC::GenVertex *tauEV = theTauon->end_vertex();
  HepMC::GenVertex::particle_iterator iterDESC ;
  for ( iterDESC = tauEV->particles_begin( HepMC::descendants );
        iterDESC != tauEV->particles_end( HepMC::descendants ) ; ++iterDESC )
  {
    LHCb::ParticleID descPID( (*iterDESC)->pdg_id() );
    int dPID = descPID.abspid();    
    if(msgLevel(MSG::DEBUG)) debug() << "     --- tauon daughter has PID: " << dPID << endmsg;
    if (dPID==211)
    {
      if(msgLevel(MSG::DEBUG)) debug() << "    --- tauon decays via pions, skipping." << endmsg;
      return;
    }
    else if (dPID==13)
    {
      passMuonKinematics(*iterDESC, hasMuon);
    }
  }
  return;
}
 
// Function to examine the Ds->KKpi hadrons and check they are in the acceptance.
// Accepts a Ds, or any 
void BeautyTomuCharmTo3h::passCharmTo3hKinematics(const HepMC::GenParticle * theCharm,
                                                  bool &hasHadrons) const
{
  // These track the numbers of Ds daughters passing each selection.
  // The first is the numbers of stable charged daughters.
  // The second is the subset of these passing acceptance and kinematics.
  int numDaugh      = 0;
  int numDaughInAcc = 0;
  bool charmInAcc   = 0;
  
  HepMC::GenVertex *CharmEV = theCharm->end_vertex();
  HepMC::GenVertex::particle_iterator iterDESC ;
  for ( iterDESC = CharmEV->particles_begin( HepMC::children );
        iterDESC != CharmEV->particles_end( HepMC::children ) ; ++iterDESC )
  {
    LHCb::ParticleID descPID( (*iterDESC)->pdg_id() );
    int dPID = descPID.abspid();
    if(msgLevel(MSG::DEBUG)) debug() << "       -- Charm daughter has PID: " << dPID << endmsg;
    if (dPID==211 || dPID==321 || dPID==2212)
    {
      numDaugh++;
      double angle = (*iterDESC)->momentum().theta();
      if (msgLevel(MSG::DEBUG)) debug() << "       -- Hadron has angle = "<< angle/ Gaudi::Units::mrad << endmsg ;
      if ( ( fabs(sin(angle)) > fabs(sin(m_chargedThetaMax)) )
           || ( fabs(sin(angle)) < fabs(sin(m_chargedThetaMin)) )
           )
      {
        if (msgLevel(MSG::DEBUG)) debug() << "         -- Hadron is NOT in LHCb acceptance." << endmsg ;
        continue;
      }
      else
      {
        if (msgLevel(MSG::DEBUG)) debug() << "       -- Hadron is in LHCb acceptance." << endmsg ;
      }
      
      // Now check it passes the momentum cuts.
      double mpx, mpy, mpz, mpp, mpt;
      mpx = (*iterDESC)->momentum().px();
      mpy = (*iterDESC)->momentum().py();
      mpz = (*iterDESC)->momentum().pz();
      mpp = sqrt( mpx*mpx + mpy*mpy + mpz*mpz );
      mpt = sqrt( mpx*mpx + mpy*mpy);
      if(msgLevel(MSG::DEBUG)) debug() << "       -- Hadron has P = " << mpp << "MeV, and Pt = "
                                       << mpt << "MeV." <<  endmsg ;
      
      // If the hadron passes the cuts for the appropriate selection, increment the number passing counter.
      if ( mpt>m_hadronptmin && mpp>m_hadronpmin )
      {
        if(msgLevel(MSG::DEBUG)) debug() << "       -- Hadron has passed momentum cuts 1."<< endmsg ;
        numDaughInAcc++;
      }
    }
    else
    {
      if(msgLevel(MSG::DEBUG)) debug() << "       -- Charm daughter is not a K/pi/p. Skipping." << endmsg;
    }
  }
  // Finished looping over daughters.
  if (numDaugh==3)
  {
    if(msgLevel(MSG::DEBUG)) debug() << "   -- Charm decays to 3 hadrons." << endmsg;
    double charmTheta = theCharm->momentum().theta();
    if (msgLevel(MSG::DEBUG)) debug() << "      -- Charm has angle = "<< charmTheta/Gaudi::Units::mrad 
                                      << endmsg;
    if ( (fabs(sin(charmTheta)) < fabs(sin(m_chargedThetaMax)))
         && (fabs(sin(charmTheta)) > fabs(sin(m_chargedThetaMin)) ) )
    {
      charmInAcc = 1;
    }
    if (numDaughInAcc!=3)
    {
      if (msgLevel(MSG::DEBUG)) debug() << "     -- Charm in acc but daughters are not!" << endmsg;
      counter("#CharmInAccButNotSelected")++;
      return;
    }
  }
  if (numDaughInAcc==3){
    hasHadrons=1;
    if(msgLevel(MSG::DEBUG)) debug() << "  -- Candidate charm has three hadrons in acceptance passing cuts."
                                     << endmsg;
    if (!charmInAcc)
    {
      if (msgLevel(MSG::DEBUG)) debug() << "      -- Charm is not in charged acceptance, but its daughters are."
                                        << endmsg;
      counter("#SelectedWithCharmNotInAcc")++;
    }
  }
  
  return;
}

