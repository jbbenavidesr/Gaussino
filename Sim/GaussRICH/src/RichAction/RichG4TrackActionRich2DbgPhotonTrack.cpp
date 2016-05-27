// Include files 

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


// local
#include "GaussRICH/RichG4TrackActionRich2DbgPhotonTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4TrackActionRich2DbgPhotonTrack
//
// 2016-05-27 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4TrackActionRich2DbgPhotonTrack::RichG4TrackActionRich2DbgPhotonTrack
  (const std::string& type   , 
   const std::string& name   ,
   const IInterface*  parent )
    :GiGaTrackActionBase( type , name , parent ),
     m_ZCoordInDownstreamOfRich1(5000.0),
     m_DeActiavteRich2PhotonTrackForDebug(0) {

  declareProperty("DeActiavteRich2PhotonTrackForDebug", m_DeActiavteRich2PhotonTrackForDebug);


}

StatusCode RichG4TrackActionRich2DbgPhotonTrack::initialize() {
// initialize the base
  StatusCode status = GiGaTrackActionBase::initialize() ;
  if( status.isFailure() )
    { return Error("Could not intialize base class GiGaTrackActionBase!",
                   status ) ; }




  return status;
  

}
void RichG4TrackActionRich2DbgPhotonTrack::PreUserTrackingAction(const G4Track* aTrack  ) {
  
      if( 0 == aTrack || 0 == trackMgr()) { return ; } /// RETURN !!!
  
    const G4DynamicParticle * aParticle = aTrack->GetDynamicParticle();
    if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
      G4double CurPhotonZcoord= aTrack->GetPosition().z();
       
      if(CurPhotonZcoord >  m_ZCoordInDownstreamOfRich1 )  {
      // This is in RICH2
      // For test rich2 photons switched off for special studies. 
      // This is not the default configuration.
        if(  m_DeActiavteRich2PhotonTrackForDebug >= 1 )  {
             trackMgr()->GetTrack() -> SetTrackStatus(fStopAndKill);         

        }
      }
    }
}






//=============================================================================
// Destructor
//=============================================================================
RichG4TrackActionRich2DbgPhotonTrack::~RichG4TrackActionRich2DbgPhotonTrack() {} 

//=============================================================================
