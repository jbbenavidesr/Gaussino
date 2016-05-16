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
// LHCb
#include "DetDesc/DetectorElement.h"

// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"

// GiGa
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaTrackActionBase.h"


// local
#include "GaussRICH/RichG4TrackActionRich2Photon.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichG4SvcLocator.h"


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
    m_ZCoordInDnsOfRich1(5000.0){ }


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
      
       
      // get the Z component of the photon direction
      // kill the backward going photons
       const G4ThreeVector & PhotCurDir = aTrack->GetMomentumDirection();
           if( PhotCurDir.z() <= 0.0 ) {

              trackMgr()->GetTrack() ->SetTrackStatus(fStopAndKill);

           }
           
       

      }
      

    }
    
    
  
}


