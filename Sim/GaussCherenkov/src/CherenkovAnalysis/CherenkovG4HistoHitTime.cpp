// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"

#include <TFile.h>
#include <TTree.h>


#include "Geant4/G4EventManager.hh"
#include "Geant4/G4TrajectoryContainer.hh"
#include "Geant4/G4Trajectory.hh"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4VHitsCollection.hh"
#include "Geant4/G4SDManager.hh"
#include "GaussCherenkov/CkvG4Hit.h"
#include "Geant4/G4TrajectoryPoint.hh"


#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaTrajectoryPoint.h"


// local
#include "CherenkovG4HistoHitTime.h"

CherenkovG4HistoHitTime* CherenkovG4HistoHitTime::CherenkovG4HistoHitTimeInstance=0;


//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4HistoHitTime
//
// 2016-07-10 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4HistoHitTime::CherenkovG4HistoHitTime(  )
  :m_CkvHitTimeNtupleBooked(false),
   m_CkvAnaNtupHitTimeFileName("DummyHitTimeNtupleFileName"),
   m_RichAnaHitTimeNtupFile(0),
   m_RichAnaHitTimeTree(0),
   m_aHitTime(0.0),
   m_aRichDetNum(0),
   m_aRichHitCollection(0),
   m_aChParticleType(0),
   m_aChTrackID(0),
   m_aChPartOriginX(0.0),
   m_aChPartOriginY(0.0),
   m_aChPartOriginZ(0.0),
   m_aChPartTotMom(0.0),
   m_aHitCoordX(0.0),
   m_aHitCoordY(0.0),
   m_aHitCoordZ(0.0),
   m_aChOriginTime(0.0){

  ResetCkvTimeParam();
  

 }

//=============================================================================
// Initialize
//=============================================================================

void CherenkovG4HistoHitTime::InitCherenkovG4NtupHitTime() 
{

  // G4cout<<" Now in InitCherenkovG4NtupHitTime "<< m_CkvAnaNtupHitTimeFileName <<G4endl;
  
  m_RichAnaHitTimeNtupFile  = new TFile(m_CkvAnaNtupHitTimeFileName,"RECREATE");
  
  if(m_RichAnaHitTimeNtupFile) {

    //G4cout<<" Now book the Time Ntuple "<<G4endl;
    
    m_RichAnaHitTimeTree = new TTree("20050","CkvG4AnalysisHitTimeNtup");
    m_RichAnaHitTimeTree ->Branch("HitTime", &m_aHitTime,"HitTime/D");
    m_RichAnaHitTimeTree ->Branch("DetNum",  &m_aRichDetNum, "DetNum/I");
    m_RichAnaHitTimeTree ->Branch("HitCollection", &m_aRichHitCollection,
                                  "HitCollection/I");
    m_RichAnaHitTimeTree ->Branch("PartType", &m_aChParticleType,"PartType/I");
    m_RichAnaHitTimeTree ->Branch("PartID",  &m_aChTrackID,  "PartID/I");
    m_RichAnaHitTimeTree ->Branch("ChOriginX", &m_aChPartOriginX,"ChOriginX/D");
    m_RichAnaHitTimeTree ->Branch("ChOriginY", &m_aChPartOriginY,"ChOriginY/D");
    m_RichAnaHitTimeTree ->Branch("ChOriginZ", &m_aChPartOriginZ,"ChOriginZ/D");
    m_RichAnaHitTimeTree ->Branch("ChOriginTime", &m_aChOriginTime, "ChOriginTime/D");
    m_RichAnaHitTimeTree ->Branch("ChTotMom", &m_aChPartTotMom, "ChTotMom/D");
    m_RichAnaHitTimeTree ->Branch("HitX",   &m_aHitCoordX, "HitX/D");
    m_RichAnaHitTimeTree ->Branch("HitY",   &m_aHitCoordY, "HitY/D");
    m_RichAnaHitTimeTree ->Branch("HitZ",   &m_aHitCoordZ, "HitZ/D");

    

    m_CkvHitTimeNtupleBooked =true;
    ResetCkvTimeParam();
    
        
  }

}
//=============================================================================
// reset in event
//=============================================================================
void CherenkovG4HistoHitTime::ResetCkvTimeParam(){
  
  m_RichTrajIDInitPosMap.clear();
  m_RichTrajIDInitTimeMap.clear();
  

}

//=============================================================================
// Fill ntuple
//=============================================================================
void CherenkovG4HistoHitTime::FillG4NtupHitTime(const G4Event* anEvent, int  NumRichColl,
                        const std::vector<int> & RichG4CollectionID   ){

  if( ! m_CkvHitTimeNtupleBooked ) return;

  
  StoreTrajectInfo(anEvent);
  
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4HitReconlog( msgSvc,"CherenkovG4HistoHitTime");


  G4HCofThisEvent * HCE;
  G4int NumRichCollection= NumRichColl;    
  int Current_RichG4CollectionID=0;

  //G4cout<<" Now fill the Hit time Ntuple "<<NumRichCollection<<   G4endl;
  
  for (int ihcol=0; ihcol<NumRichCollection; ihcol++) {

    Current_RichG4CollectionID =RichG4CollectionID[ihcol];
    if(Current_RichG4CollectionID >=0 ) {
      HCE = anEvent->GetHCofThisEvent();
      CkvG4HitsCollection* RHC=NULL;
      if(HCE){
        RHC = (CkvG4HitsCollection*)(HCE->
                                      GetHC( Current_RichG4CollectionID));
      }

     if(RHC ){
        G4int nHitInCurColl = RHC->entries();
 
        for (G4int iha=0; iha<nHitInCurColl ; iha++ ) {
          CkvG4Hit* aHit = (*RHC)[iha];
          bool aTimeNtSelectThisHit=true;
          m_aHitTime  =  aHit ->  RichHitGlobalTime() ;
          m_aRichDetNum  = aHit ->  GetCurRichDetNum();
          m_aRichHitCollection = ihcol;
          m_aChParticleType =aHit -> GetChTrackPDG();
          m_aChTrackID= aHit -> GetChTrackID() ;
          std::pair<std::multimap<G4int,G4ThreeVector>::iterator, std::multimap<G4int,G4ThreeVector>::iterator> ppp = 
                         m_RichTrajIDInitPosMap.equal_range(m_aChTrackID);
          std::multimap<G4int,G4ThreeVector>::iterator ipm = ppp.first;
          G4ThreeVector aTrackOrigin(-1000.0,-1000.0, -100000.0)  ;
          G4bool aFoundOrigin= false;
            while( ( ipm != ppp.second ) && ( ! aFoundOrigin)  ) {
            aTrackOrigin= (*ipm).second;
            //G4cout<<" ih Track Origin pos "<< iha <<"   "<<aTrackOrigin <<G4endl;
            
            aFoundOrigin = true;
            ++ipm;
            
          }
          m_aChPartOriginX = aTrackOrigin.x();
          m_aChPartOriginY = aTrackOrigin.y();
          m_aChPartOriginZ = aTrackOrigin.z();
          

          std::pair<std::multimap<G4int,G4double>::iterator, std::multimap<G4int,G4double>::iterator> qpp = 
                         m_RichTrajIDInitTimeMap.equal_range(m_aChTrackID);
          std::multimap<G4int,G4double>::iterator iqm = qpp.first;
          G4double aTrackStartTime=-1000.0;
          
          G4bool aFoundTrackStartTime= false;
            while( ( iqm != qpp.second ) && ( ! aFoundTrackStartTime)  ) {
            aTrackStartTime= (*iqm).second;

            // G4cout<<" ih Track Origin StartTime "<< iha <<"   "<<aTrackStartTime <<G4endl;
            
            aFoundTrackStartTime = true;
            ++iqm;
            
          }


          m_aChOriginTime = aTrackStartTime;
            
          m_aChPartTotMom = aHit -> ChTrackTotMom() ;
          const G4ThreeVector m_aHitCoord =  aHit -> GetGlobalPos() ;
          m_aHitCoordX =  m_aHitCoord.x();
          m_aHitCoordY =  m_aHitCoord.y();
          m_aHitCoordZ =  m_aHitCoord.z();
               

          if( aTimeNtSelectThisHit ){
            
            if(m_RichAnaHitTimeTree) m_RichAnaHitTimeTree->Fill();

          }
          
        }
        

     }
     
 
    }


  }
  
  
}


void CherenkovG4HistoHitTime::StoreTrajectInfo(const G4Event* anEvent) {


 ResetCkvTimeParam();
  
 G4TrajectoryContainer* atrajectoryContainer=anEvent->GetTrajectoryContainer();

 if(atrajectoryContainer){


   // Loop  through the standard trajectories.

   TrajectoryVector * tv =  atrajectoryContainer -> GetVector();


   for (TrajectoryVector::const_iterator iTr = tv->begin(); tv->end() != iTr;  ++iTr ) {


     //get GiGa trajectory 

   const GiGaTrajectory * gtraj = gigaTrajectory(*iTr); 
      
     
     G4int curtid =   (* iTr)->GetTrackID();
     const G4ThreeVector & CurInitPos =  (*iTr) ->  GetPoint(0)  -> GetPosition();

     //test print
     //G4cout<<" CherenkovG4HistoHitTime StoreTrajectInfo ID TrackInitPos "<<curtid<<"  "<<CurInitPos <<G4endl;
     //end test print


     m_RichTrajIDInitPosMap.insert(std::pair<G4int,G4ThreeVector>(curtid, CurInitPos));
    
     if( gtraj ) {

       const GiGaTrajectoryPoint * agpt = (GiGaTrajectoryPoint *)  (gtraj->  GetPoint(0) );

       if( agpt ) {
         
         const G4double CurInitTime =  agpt ->  GetTime();
         m_RichTrajIDInitTimeMap.insert(std::pair<G4int,G4double>(curtid, CurInitTime));
       
       

         //test print
            
         //   G4cout<< " CherenkovG4HistoHitTime StoreTraj ID TrackInitPosA TrackInitPos TrackInitTime "<<curtid<<"  "
         //    << gtraj->  GetPoint(0)  ->  GetPosition() <<"   "<<"   "<< CurInitPos << "   "<<CurInitTime <<G4endl;
       
         //end test print
       }
       
     }


   }


 }
 
 
 

}




//=============================================================================
// Write out ntuple
//=============================================================================
void CherenkovG4HistoHitTime::EndofRunG4NtupHitTime() {
  if(m_CkvHitTimeNtupleBooked ) {
    
    if(m_RichAnaHitTimeNtupFile ) {

      // G4cout <<" Now write out the Hit time Ntuple "<<G4endl;
      
      m_RichAnaHitTimeNtupFile -> Write();
      m_RichAnaHitTimeNtupFile -> Close();
      

    }
    

  }
  
  
}




//=============================================================================
// Destructor
//=============================================================================
CherenkovG4HistoHitTime::~CherenkovG4HistoHitTime() {} 

//=============================================================================

CherenkovG4HistoHitTime* CherenkovG4HistoHitTime::getCherenkovG4HistoHitTimeInstance() {
  
  if( CherenkovG4HistoHitTimeInstance == 0 ) {

    CherenkovG4HistoHitTimeInstance = new  CherenkovG4HistoHitTime();
    
  }
  
  return CherenkovG4HistoHitTimeInstance;
  
}

//=============================================================================

