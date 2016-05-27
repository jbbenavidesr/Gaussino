// Include files 

// from Gaudi
//#include "GaudiKernel/DeclareFactoryEntries.h"

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

// Geant4
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4OpticalPhoton.hh"
#include "globals.hh"
#include <math.h>
#include "G4VProcess.hh"
#include "G4Material.hh"

// LHCb
#include "DetDesc/DetectorElement.h"

// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// GiGa
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaTrackActionBase.h"


#include "RichDet/DeRichSystem.h"
#include "RichDet/DeRichHPD.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"


// local
#include "GaussRICH/RichG4TrackActionRich2Photon.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichG4SvcLocator.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichG4TrackActionRich2Photon
//
// 2016-05-08 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4TrackActionRich2Photon::RichG4TrackActionRich2Photon
 ( const std::string& type   ,
   const std::string& name   ,
   const IInterface*  parent )
   :GiGaTrackActionBase( type , name , parent ),  
    m_ZCoordInDnsOfRich1(5000.0), m_MaxPolarYScintPhotRich2(0.85),
    m_Rich2ScintPolarAngleWrtNegZLimA(0.0),
    m_Rich2ScintPolarAngleWrtNegZLimB(0.2),
    m_Rich2ScintPolarAngleWrtNegZLimC(1.0),
    m_Rich2ScintPolarAngleWrtNegZLimD(1.2),
    m_Rich2ScintPolarAngleWrtPosZLimA(0.2),
    m_Rich2ScintPolarAngleWrtPosZLimB(0.6),
    m_Rich2ScintPolarAngleWrtXLimA(1.2),
    m_Rich2ScintPolarAngleWrtXLimB(1.4),
    m_Rich2ScintPolarAngleXZSlope(-1.0),
    m_Rich2ScintPolarAngleXNegZSlope(-1.0),
    m_Rich2ScintPolarAngleXZIntercept(1.8),
    m_Rich2ScintPolarAngleXNegZZIntercept(1.8),
    m_Rich2ScintYCoordProdLimA(-1500.0),
    m_Rich2ScintYCoordProdLimB(1500.0),
    m_Rich2ScintZCoordLimA(11500.0),   
    m_RichHpdQWLogVolName("HpdQWLvName"),
    m_RichHpdPhCathodeLogVolName("HpdPhCathodeLvName"),
    m_RichG4TrackActionRich2PhotonDebugMode(0),
    m_ActivateRich2ScintPhotReduction(1)   {


  declareProperty("MaxPolarYScintPhotRich2",m_MaxPolarYScintPhotRich2);
  declareProperty("RichG4TrackActionRich2PhotonDebugMode",m_RichG4TrackActionRich2PhotonDebugMode);
  declareProperty("ActivateRich2ScintPhotReduction",m_ActivateRich2ScintPhotReduction);
  

 }


//=============================================================================
// initialize
//=============================================================================
StatusCode RichG4TrackActionRich2Photon::initialize() {

// initialize the base
  StatusCode status = GiGaTrackActionBase::initialize() ;
  if( status.isFailure() )
    { return Error("Could not intialize base class GiGaTrackActionBase!",
                   status ) ; }
 
//the following may be extracted from gigabase in the future.
  IDataProviderSvc* detSvc = RichG4SvcLocator:: RichG4detSvc ();

  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName  );
  if(Rich1DE ) {

      m_ZCoordInDnsOfRich1 = Rich1DE->param<double>("RichZDownstreamOfRich1");


      if( m_RichG4TrackActionRich2PhotonDebugMode ==1 ) {
        
        // Now get the quartz window name and the photocathode name.
        

        SmartDataPtr<DetectorElement> RichHpdQWDE(detSvc, (Rich1DeStructurePathName+RichHpdQwDeSubPathName));
        if(!RichHpdQWDE) {
          G4cout<<" RichG4TrackActionRich2Photon: Can't retrieve "+Rich1DeStructurePathName+RichHpdQwDeSubPathName<<G4endl;
          status = StatusCode::FAILURE;
        }else{

         if(RichHpdQWDE->geometry()->hasLVolume()){
           m_RichHpdQWLogVolName =(G4String)    RichHpdQWDE->geometry()->lvolumeName();
         }else{
            G4cout<<"RichG4TrackActionRich2Photon: Erroneous Log Vol for Hpd QW log vol " <<G4endl;
            status = StatusCode::FAILURE;
         }
        }
        
        
        SmartDataPtr<DetectorElement> RichHpdPCDE(detSvc, (Rich1DeStructurePathName+RichHpdPaCathDeSubPathname));
         if(!RichHpdPCDE) {
            G4cout<<"RichG4TrackActionRich2Photon:  Can't retrieve " +Rich1DeStructurePathName+RichHpdPaCathDeSubPathname
               <<G4endl;
            status = StatusCode::FAILURE;

         }else{
        
            if(RichHpdPCDE->geometry()->hasLVolume()){
              m_RichHpdPhCathodeLogVolName =(G4String)  RichHpdPCDE->geometry()->lvolumeName();
            }else{
             G4cout<<"Erroneous Log Vol for Hpd PhCathode log vol " <<G4endl;
             status = StatusCode::FAILURE;
            }
            
         }
         
      }
      

  }
  
  return status;
  
}



//=============================================================================
// Destructor
//=============================================================================
RichG4TrackActionRich2Photon::~RichG4TrackActionRich2Photon() {} 

//=============================================================================
//=============================================================================
// PreUserTrackingAction
//=============================================================================
void RichG4TrackActionRich2Photon::PreUserTrackingAction(const G4Track* aTrack  )
{
    if( 0 == aTrack || 0 == trackMgr()) { return ; } /// RETURN !!!
  
    const G4DynamicParticle * aParticle = aTrack->GetDynamicParticle();
    if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
      G4double CurPhotonZcoord= aTrack->GetPosition().z();
       
      if(CurPhotonZcoord > m_ZCoordInDnsOfRich1 )  {
      // This is in RICH2

         const G4VProcess* aProcess = aTrack->GetCreatorProcess();
         G4String  aCreatorProcessName=  
               (aProcess) ? (aProcess ->GetProcessName()) :  "NullProcess";
         if(aCreatorProcessName == "RichG4Scintillation" ) {
       
           // get the Z component of the photon direction

           // const G4ThreeVector & PhotCurDir = aTrack->GetMomentumDirection();
           const G4ThreeVector & PhotCurDir = aTrack->GetVertexMomentumDirection();
           const G4ThreeVector & PhotCurPos = aTrack->GetVertexPosition();


           G4double aPhotPx=  PhotCurDir.x();
           G4double aPhotPy=  PhotCurDir.y();
           G4double aPhotPz=  PhotCurDir.z();
           G4double aPhotY = PhotCurPos.y();

           if(  m_RichG4TrackActionRich2PhotonDebugMode == 1 ){
             
              G4double aPhotX = PhotCurPos.x();
              G4double aPhotZ = PhotCurPos.z();
 
              const G4double PhotonEnergy =    aTrack->GetDynamicParticle()-> GetKineticEnergy();
              const  G4double PhotonWaveLen =  PhotMomCnv/(PhotonEnergy*1000000);

           
              // test histo 

              FillRich2ScintPhotonDirHisto(aPhotPx,aPhotPy, aPhotPz,  
                           aPhotX,aPhotY,  aPhotZ,  PhotonWaveLen,  0);
           
              // end test histo

           }
           


           if(m_ActivateRich2ScintPhotReduction >= 1 ) {

          // Kill the photons produced in scintillating process which are going towards 
          // the top or bottom of the vessel before they are tracked. The scintillating photons created
          // which are produced in certain other directions which do not reach the detector plane are
          // also killed. 

               std::vector<G4double> aAngleVect = getPhotonScintProdAngles(aPhotPx,aPhotPy,aPhotPz);
               G4double  aPhotPolarAngleWrtY = aAngleVect[0];
               G4double  aPhotPolarAngleWrtZ = aAngleVect[1];
               G4double  aPhotPolarAngleWrtNegZ= aAngleVect[2];
               G4double  aPhotPolarAngleWrtX =  aAngleVect[3];
               
                G4bool aKillPhoton = false;
           

                if(  aPhotPolarAngleWrtY  <  m_MaxPolarYScintPhotRich2 )aKillPhoton= true;


                if(  (aPhotPolarAngleWrtZ > m_Rich2ScintPolarAngleWrtPosZLimA) && 
                     (aPhotPolarAngleWrtZ < m_Rich2ScintPolarAngleWrtPosZLimB))  aKillPhoton= true;
                  if(  aPhotPolarAngleWrtZ > 
                     ((m_Rich2ScintPolarAngleXZSlope * aPhotPolarAngleWrtX) + 
                      m_Rich2ScintPolarAngleXZIntercept))aKillPhoton= true; 

                  if( aPhotPolarAngleWrtNegZ >
                      ((m_Rich2ScintPolarAngleXNegZSlope * aPhotPolarAngleWrtX) + 
                      m_Rich2ScintPolarAngleXNegZZIntercept))aKillPhoton= true;             
                
                  if( (aPhotY < m_Rich2ScintYCoordProdLimA) ||  
                    (aPhotY > m_Rich2ScintYCoordProdLimB))aKillPhoton= true;       
                

                if( aKillPhoton ) {
                    trackMgr()->GetTrack() -> SetTrackStatus(fStopAndKill);

                }

           }}}}
    
    
    
  
}




void RichG4TrackActionRich2Photon::PostUserTrackingAction(const G4Track* aTrack  )
{

  // This is used only for debug mode and hence switched off by default.

  if( m_RichG4TrackActionRich2PhotonDebugMode == 1 ) {
    
    if( 0 == aTrack || 0 == trackMgr()) { return ; } /// RETURN !!!
  
    const G4DynamicParticle * aParticle = aTrack->GetDynamicParticle();
    if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
      // G4double CurPhotonZcoord= aTrack->GetPosition().z();
      G4double CurPhotonProdZcoord= aTrack->GetVertexPosition().z();
       
      if(CurPhotonProdZcoord > m_ZCoordInDnsOfRich1 )  {
      // This is in RICH2

        const G4VProcess* aProcess = aTrack->GetCreatorProcess();
        G4String  aCreatorProcessName=  
               (aProcess) ? (aProcess ->GetProcessName()) :  "NullProcess";
       if(aCreatorProcessName == "RichG4Scintillation" ) {
       
         // get the Z component of the photon direction when it was produced

           const G4ThreeVector & PhotCurDir = aTrack->GetVertexMomentumDirection();
           const G4ThreeVector & PhotCurPos = aTrack->GetVertexPosition();
           const G4double PhotonEnergy =    aTrack->GetDynamicParticle()-> GetKineticEnergy();
           const  G4double PhotonWaveLen =  PhotMomCnv/(PhotonEnergy*1000000);
           
           const G4String CurLogVolName =  aTrack-> GetVolume() ->GetLogicalVolume() ->GetName();
           // const G4String CurMaterialName =  aTrack->GetMaterial()->GetName();

           if( (CurLogVolName == m_RichHpdQWLogVolName)  ||  
               (CurLogVolName == m_RichHpdPhCathodeLogVolName)){

           G4double aPhotPx=  PhotCurDir.x();
           G4double aPhotPy=  PhotCurDir.y();
           G4double aPhotPz=  PhotCurDir.z();
           G4double aPhotX = PhotCurPos.x();
           G4double aPhotY = PhotCurPos.y();
           G4double aPhotZ = PhotCurPos.z();

           // test histo 

           FillRich2ScintPhotonDirHisto(aPhotPx,aPhotPy, aPhotPz,aPhotX,aPhotY,aPhotZ,PhotonWaveLen,  1);
           
           // end test histo

           }else {
             //test print 
             // G4cout<< " ScintPhotonNonHpdQW last pos "<< CurLogVolName<<"  "<<PhotCurPos<<"  "<< PhotCurDir<<  G4endl;
             // end test print
             

           }
           
           

       }
       
       
       
      
      }
      
      
      

    }
    
    
    
  }
  
  
}

std::vector<G4double> RichG4TrackActionRich2Photon::getPhotonScintProdAngles(G4double aPhotPx, G4double aPhotPy,G4double aPhotPz) 
{
  std::vector<G4double> aVect(4);

   G4double  aPhotPyAbs =   (aPhotPy >= 0.0) ? aPhotPy : (-1.0* aPhotPy) ;
   G4double  aPhotPxAbs =   (aPhotPx >= 0.0) ? aPhotPx : (-1.0* aPhotPx) ;
  
   G4double aPhotPolarAngleWrtY =
       ( aPhotPyAbs != 0.0 ) ? atan2( sqrt( aPhotPx*aPhotPx  + aPhotPz* aPhotPz ), aPhotPyAbs ) : (G4double) (M_PI * 0.5);

   G4double  aPhotPolarAngleWrtZ = -1.0;
   if(aPhotPz >=  0.0 )  aPhotPolarAngleWrtZ = atan2( sqrt( aPhotPx*aPhotPx  + aPhotPy* aPhotPy ), aPhotPz );
   
    G4double  aPhotPolarAngleWrtNegZ = -1.0;
   if(aPhotPz <  0.0 )  aPhotPolarAngleWrtNegZ = atan2( sqrt( aPhotPx*aPhotPx  + aPhotPy* aPhotPy ), -1.0*aPhotPz );

   G4double aPhotPolarAngleWrtX =
       ( aPhotPxAbs != 0.0 ) ? atan2( sqrt( aPhotPy*aPhotPy  + aPhotPz* aPhotPz ), aPhotPxAbs ) : (G4double) (M_PI * 0.5);


   aVect[0]=  aPhotPolarAngleWrtY;
   aVect[1]=  aPhotPolarAngleWrtZ;
   aVect[2]=  aPhotPolarAngleWrtNegZ;
   aVect[3]=  aPhotPolarAngleWrtX;
   
   return aVect;
}




void RichG4TrackActionRich2Photon::FillRich2ScintPhotonDirHisto(G4double aPhotPx, G4double aPhotPy, 
                                                                G4double aPhotPz,G4double aPhotX,  
                                                                G4double aPhotY, G4double aPhotZ, 
                                                                G4double PhotWaveLen, G4int PrepostFlag) {
  
   IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();


   std::vector<G4double> aAngleVect = getPhotonScintProdAngles(aPhotPx,aPhotPy,aPhotPz);
   G4double  aPhotPolarAngleWrtY = aAngleVect[0];
   G4double  aPhotPolarAngleWrtZ = aAngleVect[1];
   G4double  aPhotPolarAngleWrtNegZ= aAngleVect[2];
   G4double  aPhotPolarAngleWrtX =  aAngleVect[3];

    // G4cout<<" RichG4TrackActionRich2 XYZ  PolarangleY Max " <<  aPhotPx <<"  "<<aPhotPy
    //      <<"  "<<aPhotPz<<"  "<<    aPhotPolarAngleWrtY <<"    "
    //       << m_MaxPolarYScintPhotRich2 <<G4endl;




   if( PrepostFlag == 0 ) 
   {
     
  SmartDataPtr<IHistogram1D>hPhtotScintVertAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2100");         

  SmartDataPtr<IHistogram1D>hPhtotScintForwardAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2110");         
           
  SmartDataPtr<IHistogram1D>hPhtotScintBackwardAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2120");         

  SmartDataPtr<IHistogram1D>hPhtotScintHorizAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2130");         
  SmartDataPtr<IHistogram2D> hPhtotScintForwardVsHorizAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2135");    
  SmartDataPtr<IHistogram2D> hPhtotScintNegZVsHorizAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2136");    

  SmartDataPtr<IHistogram1D>hPhtotScintXRich2  (CurrentHistoSvc,"RICHG4HISTOSET1/2140");     
  SmartDataPtr<IHistogram1D>hPhtotScintYRich2  (CurrentHistoSvc,"RICHG4HISTOSET1/2150");     
  SmartDataPtr<IHistogram1D>hPhtotScintZRich2  (CurrentHistoSvc,"RICHG4HISTOSET1/2160");     
  SmartDataPtr<IHistogram1D>hPhtotScintWavelengthRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2170"); 
  SmartDataPtr<IHistogram2D> hPhtotScintXvsZRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2161");
  


  if(hPhtotScintVertAngleRich2 ) hPhtotScintVertAngleRich2 -> fill(aPhotPolarAngleWrtY);
  if(hPhtotScintForwardAngleRich2) hPhtotScintForwardAngleRich2 -> fill( aPhotPolarAngleWrtZ);
  if(hPhtotScintBackwardAngleRich2) hPhtotScintBackwardAngleRich2  -> fill(aPhotPolarAngleWrtNegZ);  
  if(hPhtotScintHorizAngleRich2 ) hPhtotScintHorizAngleRich2 -> fill(aPhotPolarAngleWrtX);

  if( hPhtotScintForwardVsHorizAngleRich2)  hPhtotScintForwardVsHorizAngleRich2->
                                            fill(aPhotPolarAngleWrtX,aPhotPolarAngleWrtZ);
  if(hPhtotScintNegZVsHorizAngleRich2) hPhtotScintNegZVsHorizAngleRich2->
                                            fill(aPhotPolarAngleWrtX,aPhotPolarAngleWrtNegZ);
  


  if(hPhtotScintXRich2) hPhtotScintXRich2->fill(aPhotX);
  if(hPhtotScintYRich2) hPhtotScintYRich2->fill(aPhotY);
  if(hPhtotScintZRich2) hPhtotScintZRich2->fill(aPhotZ);
  if(hPhtotScintXvsZRich2) hPhtotScintXvsZRich2->fill(aPhotZ,aPhotX);
  
  
  if(hPhtotScintWavelengthRich2) hPhtotScintWavelengthRich2->fill(PhotWaveLen);
  

   } else if ( PrepostFlag == 1 ) {
     
  SmartDataPtr<IHistogram1D>hPhDetPlaneScintVertAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2200");         

  SmartDataPtr<IHistogram1D>hPhDetPlaneScintForwardAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2210");         
           
  SmartDataPtr<IHistogram1D>hPhDetPlaneScintBackwardAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2220");         

  SmartDataPtr<IHistogram1D>hPhDetPlaneScintHorizAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2230");         
  SmartDataPtr<IHistogram2D>hPhDetPlaneScintForwardVsHorizAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2235");  
  SmartDataPtr<IHistogram2D>hPhDetPlaneScintNegZVsHorizAngleRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2236");  

  SmartDataPtr<IHistogram1D>hPhDetPlaneScintXRich2  (CurrentHistoSvc,"RICHG4HISTOSET1/2240");     
  SmartDataPtr<IHistogram1D>hPhDetPlaneScintYRich2  (CurrentHistoSvc,"RICHG4HISTOSET1/2250");     
  SmartDataPtr<IHistogram1D>hPhDetPlaneScintZRich2  (CurrentHistoSvc,"RICHG4HISTOSET1/2260");     

  SmartDataPtr<IHistogram1D>hPhDetPlaneScintWavelengthRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2270"); 
   SmartDataPtr<IHistogram2D> hPhDetPlaneScintXvsZRich2 (CurrentHistoSvc,"RICHG4HISTOSET1/2261"); 



  if(hPhDetPlaneScintVertAngleRich2 ) hPhDetPlaneScintVertAngleRich2 -> fill(aPhotPolarAngleWrtY);
  if(hPhDetPlaneScintForwardAngleRich2) hPhDetPlaneScintForwardAngleRich2 -> fill( aPhotPolarAngleWrtZ);
  if(hPhDetPlaneScintBackwardAngleRich2) hPhDetPlaneScintBackwardAngleRich2  -> fill(aPhotPolarAngleWrtNegZ);  
  if(hPhDetPlaneScintHorizAngleRich2 ) hPhDetPlaneScintHorizAngleRich2 -> fill(aPhotPolarAngleWrtX);
  if( hPhDetPlaneScintForwardVsHorizAngleRich2)  hPhDetPlaneScintForwardVsHorizAngleRich2->
                                                    fill(aPhotPolarAngleWrtX,aPhotPolarAngleWrtZ);
  if(hPhDetPlaneScintNegZVsHorizAngleRich2) hPhDetPlaneScintNegZVsHorizAngleRich2->
                                              fill(aPhotPolarAngleWrtX,aPhotPolarAngleWrtNegZ);
  

  if(hPhDetPlaneScintXRich2) hPhDetPlaneScintXRich2->fill( aPhotX);
  if(hPhDetPlaneScintYRich2) hPhDetPlaneScintYRich2->fill( aPhotY);
  if(hPhDetPlaneScintZRich2) hPhDetPlaneScintZRich2->fill( aPhotZ);
  if(hPhDetPlaneScintXvsZRich2)  hPhDetPlaneScintXvsZRich2->fill( aPhotZ,aPhotX);

  if(hPhDetPlaneScintWavelengthRich2)  hPhDetPlaneScintWavelengthRich2  ->fill(PhotWaveLen);

   }
   
   
   
}



