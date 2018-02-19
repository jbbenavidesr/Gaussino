// Include files 

#include "Geant4/G4Track.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4OpticalPhoton.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4Step.hh"
#include <math.h>
#include <vector>
#include "Geant4/G4VProcess.hh"
#include "Geant4/G4ProcessVector.hh"
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4TrackingManager.hh"
#include "Geant4/G4TrackStatus.hh"

// Gaudi
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "GaussRICH/RichG4StepAnalysis15.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "GaussRICH/RichG4SvcLocator.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichG4StepAnalysis15
// To test on the photons produced from scintillation process
// If they go through too many steps, they are being killed.
// 2016-05-22 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4StepAnalysis15::RichG4StepAnalysis15( 
  const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent):GiGaStepActionBase ( type , name , parent ),
                             m_Rich2ScintMaxStepNumCutValue(10),
                             m_Rich2CkvMaxStepNumCutValue(100)   {
   
  
}

//=============================================================================
// Destructor
//=============================================================================
RichG4StepAnalysis15::~RichG4StepAnalysis15() {} 

//=============================================================================
void RichG4StepAnalysis15::UserSteppingAction( const G4Step* aStep ) {
  
 //    G4cout<<"Now in Rich Step Analysis 15 " <<G4endl;
 
    const G4Track* aTrack= aStep ->GetTrack();
    if(aTrack->GetDynamicParticle()->GetDefinition() ==  G4OpticalPhoton::OpticalPhoton() )
    {
       G4StepPoint* aPreStepPoint = aStep->GetPreStepPoint();
       G4StepPoint* aPostStepPoint = aStep->GetPostStepPoint();
       if(aPostStepPoint->GetStepStatus() == fGeomBoundary ) {
         if( aTrack->GetDynamicParticle()->GetKineticEnergy() > 0.0 ) {
             
           const G4ThreeVector & prePos=aPreStepPoint->GetPosition();
           const G4ThreeVector & postPos=aPostStepPoint->GetPosition();
           if( (prePos.z() >= ZUpsRich2Analysis) && 
               (postPos.z() <= ZDnsRich2Analysis) ){
             // we are in RICH2
             const G4int CurStepNum=  aTrack->GetCurrentStepNumber() ;
            
             //GaussTrackInformation* aRichPhotTrackInfo = 
             //         (GaussTrackInformation*)aTrack->GetUserInformation();    
             // if( aRichPhotTrackInfo ) {
             // if( aRichPhotTrackInfo->detInfo() ){
             //   RichInfo* aRichTypeInfo =
             //            ( RichInfo*) (aRichPhotTrackInfo->detInfo());
             //   if( aRichTypeInfo && aRichTypeInfo->HasUserPhotInfo() ){
             //       RichPhotInfo* aRichPhotInfo =
             //                     aRichTypeInfo-> RichPhotInformation();
             //       if( aRichPhotInfo ) {

             //          aProdProc = aRichPhotInfo->PhotonSourceInfo()  ;

             //       }}}
             // }
             
           
              const G4VProcess* aProcess = aTrack->GetCreatorProcess(); 
              G4String  aCreatorProcessName=  
               (aProcess) ? (aProcess ->GetProcessName()) :  "NullProcess";
              if(aCreatorProcessName == "RichG4Scintillation" ) {
                // FillHistoRich2Scint(CurStepNum,2);
                if(CurStepNum > m_Rich2ScintMaxStepNumCutValue ) {
                    aStep ->GetTrack()   -> SetTrackStatus(fStopAndKill);

                }
                
              }
              
     
              // }else if (aCreatorProcessName == "RichG4Cerenkov"    ) {
               
                // FillHistoRich2Scint(CurStepNum,1); 

              // }
              

           }
           
           
           
           
             

             
             
             
         }
         
           
           
           

       }
       
       
    }
    
       
  
}

    
void   RichG4StepAnalysis15::FillHistoRich2Scint(G4int aSNum , G4int aProcNum ) {
  
  const G4double aCurStepNum= aSNum * 1.0;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
  SmartDataPtr<IHistogram1D>hStepNumRich2Scint(CurrentHistoSvc,"RICHG4HISTOSET1/1040");
  SmartDataPtr<IHistogram1D>hStepNumLBARich2Scint(CurrentHistoSvc,"RICHG4HISTOSET1/1041");
  SmartDataPtr<IHistogram1D>hStepNumSBARich2Scint(CurrentHistoSvc,"RICHG4HISTOSET1/1042");
  SmartDataPtr<IHistogram1D>hStepNumSBA2Rich2Scint(CurrentHistoSvc,"RICHG4HISTOSET1/1043");
  SmartDataPtr<IHistogram1D>hStepNumRich2Ckv(CurrentHistoSvc,"RICHG4HISTOSET1/1044");
  SmartDataPtr<IHistogram1D>hStepNumLBARich2Ckv(CurrentHistoSvc,"RICHG4HISTOSET1/1045");
  SmartDataPtr<IHistogram1D>hStepNumSBA2Rich2Ckv(CurrentHistoSvc,"RICHG4HISTOSET1/1046");
  if( aProcNum == 2 ) 
  {
    
     if(hStepNumRich2Scint)  hStepNumRich2Scint->fill(aCurStepNum);
     if(hStepNumLBARich2Scint)  hStepNumLBARich2Scint->fill(aCurStepNum);
     if(hStepNumSBARich2Scint)  hStepNumSBARich2Scint->fill(aCurStepNum);
     if(hStepNumSBA2Rich2Scint && aCurStepNum > 5 )  hStepNumSBA2Rich2Scint->fill(aCurStepNum);
  }else if ( aProcNum == 1 ) {
    
    if(hStepNumRich2Ckv) hStepNumRich2Ckv->fill(aCurStepNum); 
    if(hStepNumLBARich2Ckv) hStepNumLBARich2Ckv->fill(aCurStepNum);
    if(hStepNumSBA2Rich2Ckv && aCurStepNum > 5 ) hStepNumSBA2Rich2Ckv ->fill(aCurStepNum);
  }
  
  
}




