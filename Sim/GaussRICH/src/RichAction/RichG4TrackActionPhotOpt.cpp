// $Id: RichG4TrackActionPhotOpt.cpp,v 1.7 2009-07-03 11:59:49 seaso Exp $
// Include files 

// from Gaudi
//#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IMessageSvc.h"

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

// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"

// GiGa 
#include "GiGa/GiGaTrajectory.h"

// LHCb
#include "DetDesc/DetectorElement.h"

// local
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichG4TrackActionPhotOpt.h"
#include "GaussRICH/RichG4SvcLocator.h"
#include "GaussRICH/RichHpdProperties.h"
#include "GaussRICH/RichPEInfoAttach.h"
#include "GaussRICH/RichG4RadiatorMaterialIdValues.h"
#include "GaussRICH/RichScintilParamAdmin.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichG4TrackActionPhotOpt
//
// 2003-04-29 : Sajan EASO
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory. moved to Factories.cpp
//DECLARE_TOOL_FACTORY( RichG4TrackActionPhotOpt );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4TrackActionPhotOpt::RichG4TrackActionPhotOpt
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent ), m_MaxHpdQuantumEffFromDB(0.45),
     m_MaxRich1Mirror1Reflect(1.0), m_MaxRich1Mirror2Reflect(1.0),
     m_MaxRich2Mirror1Reflect(1.0),  m_MaxRich2Mirror2Reflect(1.0),
    m_ZDownstreamOfRich1(5000.0),m_Rich1NominalAerogelBeginZLocation(1110.0),
    m_Rich1NominalAerogelEndZLocation(1062.1),
    m_Rich1AerogelTotPhotonSuppressFactor(1.0), 
    m_Rich1GasTotPhotonSuppressFactor(1.0), 
    m_Rich2GasTotPhotonSuppressFactor(1.0),
    m_RichHpdMaxQEOverRideDB(false),
    m_RichMirrorReflMaxOverRideDB(false),
    m_RichHpdMaxQEValueWhenOverRideDB(0.45),
    m_MaxRich1Mirror1ReflectWhenOverRideDB(0.95),
    m_MaxRich1Mirror2ReflectWhenOverRideDB(0.97),
    m_MaxRich2Mirror1ReflectWhenOverRideDB(0.98),
    m_MaxRich2Mirror2ReflectWhenOverRideDB(0.98),
    m_RichHpdSiDetEfficiency(0.85),
    m_RichHpdReadoutEffWithAerogel(1.0),
    m_RichHpdReadoutEffWithRich1Gas(1.0),
    m_RichHpdReadoutEffWithRich2Gas(1.0),
    m_Rich1EffConvolutionSuppressFactor(1.0),
    m_Rich2EffConvolutionSuppressFactor(1.0)
{ 

  //    m_Rich1EffConvolutionSuppressFactor(0.93),
  //  m_Rich2EffConvolutionSuppressFactor(0.94)

  declareProperty("RichHpdMaxQEOverRideDB",m_RichHpdMaxQEOverRideDB);
  declareProperty("RichMirrorReflMaxOverRideDB", m_RichMirrorReflMaxOverRideDB);
  declareProperty("RichHpdMaxQEValueWhenOverRideDB",m_RichHpdMaxQEValueWhenOverRideDB);

  declareProperty("MaxRich1Mirror1ReflectWhenOverRideDB",m_MaxRich1Mirror1ReflectWhenOverRideDB);
  declareProperty("MaxRich1Mirror2ReflectWhenOverRideDB",m_MaxRich1Mirror2ReflectWhenOverRideDB);
  declareProperty("MaxRich2Mirror1ReflectWhenOverRideDB",m_MaxRich2Mirror1ReflectWhenOverRideDB);
  declareProperty("MaxRich2Mirror2ReflectWhenOverRideDB",m_MaxRich2Mirror2ReflectWhenOverRideDB);  

  declareProperty("RichHpdSiDetEfficiency", m_RichHpdSiDetEfficiency);
  declareProperty("RichHpdReadOutEffInAerogel",m_RichHpdReadoutEffWithAerogel);
  declareProperty("RichHpdReadOutEffInRich1Gas", m_RichHpdReadoutEffWithRich1Gas);
  declareProperty("RichHpdReadOutEffInRich2Gas", m_RichHpdReadoutEffWithRich2Gas);
  declareProperty("Rich1EffConvolutionSuppressFactor",m_Rich1EffConvolutionSuppressFactor);
  declareProperty("Rich2EffConvolutionSuppressFactor",m_Rich2EffConvolutionSuppressFactor);
  
}

//=============================================================================
// Destructor
//=============================================================================
RichG4TrackActionPhotOpt::~RichG4TrackActionPhotOpt(){ }

//=============================================================================
// initialize
//=============================================================================
StatusCode RichG4TrackActionPhotOpt::initialize() 
{

 // initialize the base 
  StatusCode status = GiGaTrackActionBase::initialize() ; 

 if( status.isFailure() ) 
    { return Error("Could not intialize base class GiGaTrackActionBase!", 
                   status ) ; } 
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream log( msgSvc , "RichG4TrackActionPhotOpt" );


 //the following may be extracted from gigabase in the future.
  IDataProviderSvc* detSvc = RichG4SvcLocator:: RichG4detSvc ();
 

  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName  );
     if( !Rich1DE ){
       return Error( 
      "Can't retrieve  " +Rich1DeStructurePathName  +" in RichG4TrackActionPhotOpt" );
     } 
     else {

       m_MaxHpdQuantumEffFromDB=Rich1DE->param<double>("RichHpdMaxQE");

       int aMaxReflUse=0;
       if(Rich1DE ->exists("RichUseMirrorMaxReflectivityFlag")){
           aMaxReflUse= Rich1DE ->param<int> ("RichUseMirrorMaxReflectivityFlag");   
       }
       
         
       if(aMaxReflUse > 0 ) 
       {
         
           m_MaxRich1Mirror1Reflect=
                 Rich1DE->param<double>("Rich1Mirror1MaxReflect");
           m_MaxRich1Mirror2Reflect=
                 Rich1DE->param<double>("Rich1Mirror2MaxReflect");
           m_MaxRich2Mirror1Reflect=
                 Rich1DE->param<double>("Rich2Mirror1MaxReflect");
           m_MaxRich2Mirror2Reflect=
                 Rich1DE->param<double>("Rich2Mirror2MaxReflect");

       }
       
       m_ZDownstreamOfRich1 = 
         Rich1DE->param<double>("RichZDownstreamOfRich1");
       if( Rich1DE->exists("Rich1AerogelNominalZBeginLocation")){
            m_Rich1NominalAerogelBeginZLocation = 
                      Rich1DE->param<double>("Rich1AerogelNominalZBeginLocation");
       }else {
            m_Rich1NominalAerogelBeginZLocation= AgelZBeginAnalysis;
       }
       
       
       if (Rich1DE->exists("Rich1AerogelNominalZEndLocation")){
            m_Rich1NominalAerogelEndZLocation   = 
                      Rich1DE->param<double>("Rich1AerogelNominalZEndLocation" );
       }else {
            m_Rich1NominalAerogelEndZLocation= AgelZEndAnalysis;
         
       }
       
         

     }
     
       
       //      m_Rich1TotPhotonSuppressFactor=  m_MaxHpdQuantumEffFromDB;
       //      m_Rich2TotPhotonSuppressFactor=  m_MaxHpdQuantumEffFromDB;

       double  aRich1CommonPhotonSuppressFactor =  m_MaxHpdQuantumEffFromDB * 
                                                    m_MaxRich1Mirror1Reflect * 
                                                    m_MaxRich1Mirror2Reflect * 
                                                    m_RichHpdSiDetEfficiency *
                                                    m_Rich1EffConvolutionSuppressFactor;

       double aRich2CommonPhotonSuppressFactor =  m_MaxHpdQuantumEffFromDB *
                                                  m_MaxRich2Mirror1Reflect *
                                                  m_MaxRich2Mirror2Reflect *
                                                  m_RichHpdSiDetEfficiency *
                                                  m_Rich2EffConvolutionSuppressFactor;
       
       m_Rich1AerogelTotPhotonSuppressFactor =  aRich1CommonPhotonSuppressFactor * m_RichHpdReadoutEffWithAerogel   ;
       m_Rich1GasTotPhotonSuppressFactor     =  aRich1CommonPhotonSuppressFactor * m_RichHpdReadoutEffWithRich1Gas  ;
       m_Rich2GasTotPhotonSuppressFactor     =  aRich2CommonPhotonSuppressFactor *  m_RichHpdReadoutEffWithRich2Gas ;
       
       
       if(m_RichHpdMaxQEOverRideDB) {
         double aRich1ComonPhotonSuppressFactorODB =  m_RichHpdMaxQEValueWhenOverRideDB * 
                                                      m_MaxRich1Mirror1ReflectWhenOverRideDB *
                                                      m_MaxRich1Mirror2ReflectWhenOverRideDB *
                                                      m_RichHpdSiDetEfficiency* 
                                                      m_Rich1EffConvolutionSuppressFactor;           


         double aRich2ComonPhotonSuppressFactorODB =  m_RichHpdMaxQEValueWhenOverRideDB *
                                                      m_MaxRich2Mirror1ReflectWhenOverRideDB  *
                                                      m_MaxRich2Mirror2ReflectWhenOverRideDB  *
                                                      m_RichHpdSiDetEfficiency*
                                                      m_Rich2EffConvolutionSuppressFactor;

         m_Rich1AerogelTotPhotonSuppressFactor= aRich1ComonPhotonSuppressFactorODB * m_RichHpdReadoutEffWithAerogel   ;
         m_Rich1GasTotPhotonSuppressFactor    = aRich1ComonPhotonSuppressFactorODB * m_RichHpdReadoutEffWithRich1Gas  ;
         m_Rich2GasTotPhotonSuppressFactor    = aRich2ComonPhotonSuppressFactorODB * m_RichHpdReadoutEffWithRich2Gas  ;
 

         RichHpdProperties* aRichHpdProperties = RichHpdProperties::getRichHpdPropertiesInstance();
         aRichHpdProperties->setHpdActivateOverRideMaxQEFromDB(m_RichHpdMaxQEOverRideDB);
         aRichHpdProperties->setHpdDBOverRideMaxQEValue(m_RichHpdMaxQEValueWhenOverRideDB);
         aRichHpdProperties-> setHpdMaximumQuantumEfficiency();
       }
       
         RichScintilParamAdmin* aRichScintilParamAdmin = 
                RichScintilParamAdmin::getRichScintilParamAdminInstance();
         aRichScintilParamAdmin->setRichScintilScaleFactor(m_Rich2GasTotPhotonSuppressFactor);
         


       log << MSG::INFO <<" Rich HPD MaxQE SiDetEff  and  OverallEff  for  Aerogel Rich1Gas Rich2Gas  "
           << m_MaxHpdQuantumEffFromDB<<"    " <<  m_RichHpdSiDetEfficiency <<"   "<< m_RichHpdReadoutEffWithAerogel <<"    "
           << m_RichHpdReadoutEffWithRich1Gas  <<"   "<< m_RichHpdReadoutEffWithRich2Gas  <<endmsg;       
       log << MSG::INFO <<"  Rich Max Mirror reflectivity R1M1 R1M2 R2M1 R2M2  ConvSup R1 R2 " 
           << m_MaxRich1Mirror1Reflect << "   "<< m_MaxRich1Mirror2Reflect<<"   "
           << m_MaxRich2Mirror1Reflect << "   "<< m_MaxRich2Mirror2Reflect << "    "
           <<  m_Rich1EffConvolutionSuppressFactor<<"   "<< m_Rich2EffConvolutionSuppressFactor  <<endmsg;

       log << MSG::INFO <<" Total Photon suppress factor for Aerogel Rich1Gas Rich2Gas "
          << m_Rich1AerogelTotPhotonSuppressFactor <<"    "<<  m_Rich1GasTotPhotonSuppressFactor <<"    "
          << m_Rich2GasTotPhotonSuppressFactor <<endmsg;
       

     
     
     
     
     return status;
     

}

//=============================================================================
// PreUserTrackingAction (Geant4)
//=============================================================================
void RichG4TrackActionPhotOpt::PreUserTrackingAction  
       ( const G4Track* aTrack ) {


   if( 0 == aTrack || 0 == trackMgr()) { return ; } /// RETURN !!!
 
  const G4DynamicParticle * aParticle = aTrack->GetDynamicParticle();
  if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
    const G4double ZPhotOrigin=  aTrack-> GetVertexPosition().z();
    G4double PhotonSupFact=  m_Rich1GasTotPhotonSuppressFactor;

    if(ZPhotOrigin >  m_ZDownstreamOfRich1){

      const G4VProcess* aProcess = aTrack->GetCreatorProcess();
      G4String  aCreatorProcessName=  
                   (aProcess) ? (aProcess ->GetProcessName()) :  "NullProcess";
      if(aCreatorProcessName != "RichG4Scintillation" ) {        
          PhotonSupFact=  m_Rich2GasTotPhotonSuppressFactor;
      }else {

        PhotonSupFact= 1.0;
      }
      
      
    }else  if ( ( ZPhotOrigin < m_Rich1NominalAerogelEndZLocation) && 
                ( ZPhotOrigin > m_Rich1NominalAerogelBeginZLocation ) ) {

      int currentRadiatorNumber=RichPhotTkRadiatorNumber( *aTrack);
      RichG4RadiatorMaterialIdValues* aRMIdValues=
         RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();
         if(aRMIdValues -> IsRich1AerogelAnyTileRad( currentRadiatorNumber ) ) {
             PhotonSupFact= m_Rich1AerogelTotPhotonSuppressFactor;
         }
      
    }
    
    
    
   
    
   G4double aRandomNum = G4UniformRand();
   if(aRandomNum >  PhotonSupFact ) {

     // Kill the photon       
       trackMgr()->GetTrack() ->SetTrackStatus(fStopAndKill);
     
   }
              
      
  }
  
}

//=============================================================================
// PostUserTrackingAction (Geant4)
//=============================================================================
//void RichG4TrackActionPhotOpt::PostUserTrackingAction 
//   ( const G4Track* aTrack ) 
//{ } 

//=============================================================================

