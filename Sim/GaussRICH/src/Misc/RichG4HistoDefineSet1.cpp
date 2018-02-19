
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
//local

#include "GaussRICH/RichG4HistoDefineSet1.h"
#include "GaussRICH/RichG4SvcLocator.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "Geant4/globals.hh"
                                                                                                                  

//
//   Author SE 21-8-2002 
//
// Declaration of the Algorithm Factory
// static const  AlgFactory<RichG4Histo>          r_factory ;
// const        IAlgFactory& RichG4HistoFactory = r_factory ;
// RichG4Histo::RichG4Histo( const std::string& name,
//                           ISvcLocator* pSvcLocator) {}

RichG4HistoDefineSet1::RichG4HistoDefineSet1() {

  // Declare job options
  m_RichG4HistoPathSet1="RICHG4HISTOSET1/";
  //  declareProperty( "RichG4HistoPath", m_RichG4HistoPath = "/RICHG4HISTOSET1/" );

}


void RichG4HistoDefineSet1::BookRichG4HistogramsSet1() {
  

  // Book Set 1 histograms

  std::cout<<" Book set1 histograms with type "<< m_RichG4HistoSet1Type <<std::endl;
  

  
  if(m_RichG4HistoSet1Type == 1 ) {
    
    bookRichG4HistogramsSet1Type1() ;

  }else if (m_RichG4HistoSet1Type  == 2){

    bookRichG4HistogramsSet1Type2() ;
            
  }else if ( m_RichG4HistoSet1Type == 3 ) {

    bookRichG4HistogramsSet1Type3() ;

  }
   
    
  
}



RichG4HistoDefineSet1::~RichG4HistoDefineSet1() {  }

void RichG4HistoDefineSet1::bookRichG4HistogramsSet1Type1() {

  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), "RichG4HistoSet1" );
  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo Set1" << endmsg;
  std::string title;

  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();

  title="Total number of Rich1 Hits per event ";


  RichG4Histolog << MSG::INFO<<"Current Histo Path for Set1 Histograms is    "
                               <<m_RichG4HistoPathSet1<<endmsg;

  m_hNumTotHitRich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"1",
                             title,100,0.0,200.0);
  if(! m_hNumTotHitRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

    title="GlobalPhotoelectron Origin Y vs X for Rich1 ";
  
    //    m_hGlobalRich1PEOriginXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"2",
    //  title,300,-2000.0,2000.0,300,-2000.0, 2000.0);
    // if(!m_hGlobalRich1PEOriginXY ) {
    //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

    //    }

    title="Global PE hit coordinate positive Y vs X for Rich1 ";

    m_hGlobalRich1PeOriginPosHitXY = 
      CurHistoSvc->book(m_RichG4HistoPathSet1+"4",title,
     280,-700.0,700.0,80,800.0, 2400.0);


    title="Global hit coordinate from Agel positive Y vs X for Rich1 ";

    m_hGlobalPEOriginAgelPosXY= 
         CurHistoSvc->book(m_RichG4HistoPathSet1+"122",title,
     280,-700.0,700.0,80,800.0, 2400.0);


    //  title="GlobalPhotoelectron Origin from Agel Y vs X for Rich1 ";

    //  m_hGlobalPEOriginAgelXY = CurHistoSvc
    //  ->book(m_RichG4HistoPathSet1+"120",title,300,-2000.0,
    //    2000.0,300,-2000.0, 2000.0);
    // if(!m_hGlobalPEOriginAgelXY ) {
    // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

    //  }

  //  title="GlobalPE Origin from Agel Top Proj Y vs X for Rich1 ";

  //  m_hGlobalPEOriginAgelTopXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"122",
  //  title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginAgelTopXY ) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  // }
  //  title="GlobalPE Origin from Agel Bot Proj Y vs X for Rich1 ";

  // m_hGlobalPEOriginAgelBotXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"124",
  //  title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginAgelBotXY ) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  // }
  
    title="Global hit coordinate from C4F10 postive Y vs X for Rich1 ";

    m_hGlobalPEOriginC4F10PosXY= 
         CurHistoSvc->book(m_RichG4HistoPathSet1+"152",title,
     280,-700.0,700.0,80,800.0, 2400.0);


    //   title="GlobalPhotoelectron Origin from C4F10 Y vs X for Rich1 ";

    //  m_hGlobalPEOriginC4F10XY = CurHistoSvc
    //    ->book(m_RichG4HistoPathSet1+"150",title,300,-2000.0,
    //                2000.0,300,-2000.0, 2000.0);
    //  if(!m_hGlobalPEOriginC4F10XY ) {
    //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

    //  }
  title="GlobalPhotoelectron Origin from CF4 Y vs X for Rich2 ";

  //  m_hGlobalPEOriginCF4XY = CurHistoSvc->
  //  book(m_RichG4HistoPathSet1+"170",title,
  //            300,-5000.0,5000.0,300,-5000.0, 5000.0);
  //  if(!m_hGlobalPEOriginCF4XY ) {
  //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  //  }
  title="GlobalPhotoelectron Origin from CF4 Y vs positive X for Rich2 ";

  m_hGlobalPEOriginCF4PosXY = CurHistoSvc
         ->book(m_RichG4HistoPathSet1+"172",title,
                200,2500.0,4500.0,200,-1000.0, 1000.0);
  if(!m_hGlobalPEOriginCF4PosXY ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  // title="GlobalPE Origin from C4F10 Top Proj Y vs X for Rich1 ";

  // m_hGlobalPEOriginC4F10TopXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"152",
  //          title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginC4F10TopXY ) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  // }
  // title="GlobalPE Origin from C4F10 Bot Proj Y vs X for Rich1 ";

  //  m_hGlobalPEOriginC4F10BotXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"154",
  //            title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginC4F10BotXY ) {
  //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  // }

  title="Photon Wavelength before incident on  Rich1 Mirror1 ";

  m_hWaveLenBeforeRich1Mirror1 = CurHistoSvc->book(m_RichG4HistoPathSet1+"40",
                       title,200,150.0,950.0);
  if(!m_hWaveLenBeforeRich1Mirror1 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Photon Wavelength after incident on  Rich1 Mirror1 ";

  m_hWaveLenAfterRich1Mirror1 = CurHistoSvc->book(m_RichG4HistoPathSet1+"42",
                   title,200,150.0,950.0);
  if(!m_hWaveLenAfterRich1Mirror1 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Photon Wavelength before incident on  Rich1 Gas QW ";

  m_hWaveLenBeforeRich1GasQW = CurHistoSvc->book(m_RichG4HistoPathSet1+"50",
               title,200,150.0,950.0);
  if(!m_hWaveLenBeforeRich1GasQW ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Photon Wavelength after exiting from  Rich1 Gas QW ";

  m_hWaveLenAfterRich1GasQW = CurHistoSvc
             ->book(m_RichG4HistoPathSet1+"52",title,200,150.0,950.0);
  if(!m_hWaveLenAfterRich1GasQW ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }
  title="Photon Wavelength before  Rich1 QE ";
  m_hWaveLenBeforeRich1QE = CurHistoSvc->book(m_RichG4HistoPathSet1+"60",title,
					      200,150.0,950.0);
  if(!m_hWaveLenBeforeRich1QE){
    RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg; 

  } 

  title="Photon Wavelength After  Rich1 QE ";     
  m_hWaveLenAfterRich1QE = CurHistoSvc->book(m_RichG4HistoPathSet1+"62",title,
					     200,150.0,950.0);
  if(!m_hWaveLenAfterRich1QE){
    RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;
   
  }


    //  title="Global PE hit coordinate Y vs X for Rich1 ";

    //    m_hGlobalHitXY= CurHistoSvc->book(m_RichG4HistoPathSet1+"3",title,
    // 300,-2000.0,2000.0,300,-2000.0, 2000.0);

    //    if(!m_hGlobalHitXY ) {
    //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

    //    }
  

  title="Cherenkov Angle in Aerogel from G4 ";


  m_hCkvAgelRich1= 
         CurHistoSvc->book(m_RichG4HistoPathSet1+"10",title,1200,0.1,0.40);
  if(! m_hCkvAgelRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Cherenkov Angle in C4F10 from G4 ";


  m_hCkvC4F10Rich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"20",title,
                                                    1200,0.03,0.07);
  if(! m_hCkvC4F10Rich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }
  title="Cherenkov Angle in CF4 from G4 ";


  m_hCkvCF4Rich2= CurHistoSvc->book(m_RichG4HistoPathSet1+"70",
             title,1200,0.0,0.05);
  if(! m_hCkvCF4Rich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Cherenkov Photon Production Z Coord from G4 ";

  //  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endmsg;

  m_hCkvZEmissionPtRich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"25",
                 title,1300,900,2200);
  if(! m_hCkvZEmissionPtRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  //    title="Ref Index-1 vs wavelength for C4F10 from G4 ";

  //  m_hRefIndC4F10Rich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"22",
  //    title,200,150.0,950.0,200,0.001,0.002);
  // if(! m_hRefIndC4F10Rich1) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  //  }

  title="Ref Index-1 vs wavelength for CF4 from G4 ";

  m_hRefIndCF4Rich2= CurHistoSvc->book(m_RichG4HistoPathSet1+"72",title,
              200,150.0,950.0,200,0.0001,0.001);
  if(! m_hRefIndCF4Rich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  //  title="Ref Index-1 vs wavelength for Agel from G4 ";

  //  m_hRefIndAgelRich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"12",title,
  //  200,150.0,950.0,200,0.01,0.08);
  //  if(! m_hRefIndAgelRich1) {
  //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  //  }

  //  title = "Hpd Qw Hits Rich1 : Track prod X vs Z ";
  //  M_hHpdQwOrigXZR1= CurHistoSvc->book(m_RichG4HistoPathSet1+"1001", title,
  //				      100, 0.0, 2500.0, 100, -1000., 1000. );

  //  title = "Hpd Qw Hits Rich2 : Track prod X vs Z ";
  // M_hHpdQwOrigXZR2= CurHistoSvc->book(m_RichG4HistoPathSet1+"1002", title,
  //				      100, 5000., 13000.0, 100, -5000., 5000. );



  //  title = "Hpd Qw Hits Rich1 : Track prod Y vs Z ";

  //  M_hHpdQwOrigYZR1= CurHistoSvc->book(m_RichG4HistoPathSet1+"1010", title,
  //				      100, 0.0, 2500.0, 100, -3000., 3000. );


  //  title = "Hpd Qw Hits Rich2 : Track prod Y vs Z ";

  //  M_hHpdQwOrigYZR2= CurHistoSvc->book(m_RichG4HistoPathSet1+"1011", title,
	//			      100, 5000., 13000.0, 100, -3000., 3000. );

  

  //    title="Energy Loss in CF4 in 1 meter ";
  //  m_hEnergyLossInCF4 = CurHistoSvc->book(m_RichG4HistoPathSet1+"901",
  //                                                      title,100,0.0, 10.0);
  //
  //   title = "Number of Scintillating Photons produced in 1 meter in CF4";
  //
  //    m_hNumScintPhotProdInCF4 =CurHistoSvc->book( m_RichG4HistoPathSet1+"910",
  //                                                            title, 100,0.0, 3000.0);
  //

}


void RichG4HistoDefineSet1::bookRichG4HistogramsSet1Type2() {



  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), "RichG4HistoSet1" );

  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo Set1A" << endmsg;


  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();


  std::string title="Step Num for Photons in Rich2 produced from scintillation ";

  m_hStepNumScintPhotRich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"1040",
                       title,200,0.0,200.0);
  if(!m_hStepNumScintPhotRich2 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Step Num for Photons in Rich2 produced from scintillation Large size";

  m_hStepNumScintPhotLBARich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"1041",
                       title,200,0.0,500.0);
  if(!m_hStepNumScintPhotLBARich2 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Step Num for Photons in Rich2 produced from scintillation Small size";

  m_hStepNumScintPhotSBARich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"1042",
                       title,202,-1.0,100.0);
  if(!m_hStepNumScintPhotSBARich2 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Step Num for Photons in Rich2 produced from scintillation Small size Above 5";

  m_hStepNumScintPhotSBA2Rich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"1043",
                       title,202,-1.0,100.0);
  if(!m_hStepNumScintPhotSBA2Rich2 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Step Num for Photons in Rich2 produced from Cherenkov";

  m_hStepNumCkvPhotRich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"1044",
                       title,202,-1.0,100.0);
  if(!m_hStepNumCkvPhotRich2 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Step Num for Photons in Rich2 produced from Cherenkov large size";

  m_hStepNumCkvPhotLBARich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"1045",
                       title,500,0.0,500.0);
  if(!m_hStepNumCkvPhotLBARich2 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

  title="Step Num for Photons in Rich2 produced from Cherenkov small size above 5";

  m_hStepNumCkvPhotSBA2Rich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"1046",
                       title,201,-1.0,200.0);
  if(!m_hStepNumCkvPhotSBA2Rich2 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endmsg;

  }

}

void RichG4HistoDefineSet1::bookRichG4HistogramsSet1Type3() {

 MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), "RichG4HistoSet1" );

  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo Set1B" << endmsg;


  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();


   std::string title="Angle wrt Y axis for Photons in Rich2 produced from scintillation ";

   m_hPhtotScintVertAngleRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2100",
                                                   title,200, 0.0, 2.0);

   
   title="Angle wrt positive Z axis for Photons in Rich2 produced from scintillation ";
   
   m_hPhtotScintForwardAngleRich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"2110",
                                                   title,200, 0.0, 2.0); 


   title="Angle wrt negative Z axis for Photons in Rich2 produced from scintillation ";
  
   m_hPhtotScintBackwardAngleRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2120",
                                                       title, 200, 0.0, 2.0);
   
   
   title="Angle wrt X axis for Photons in Rich2 produced from scintillation ";
   
   m_hPhtotScintHorizAngleRich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"2130",
                                                   title,200, 0.0, 2.0); 

   title = "Angle wrt ForwardZ  vs wrt X  for Photons in Rich2 from scintillation";
   
   m_hPhtotScintForwardVsHorizAngleRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2135",
                                                             title,200, 0.0, 2.0, 200, 0.0, 2.0 ); 
 
  title = "Angle wrt Neg Z  vs wrt X  for Photons in Rich2 from scintillation";
   
   m_hPhtotScintNegZVsHorizAngleRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2136",
                                                             title,200, 0.0, 2.0, 200, 0.0, 2.0 ); 

   title=" X coordinate of origin of photons in Rich2 produced froms scintillation";
   m_hPhtotScintXRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2140",
                                            title, 400, -3700.0, 3700.0);
    

   title=" Y coordinate of origin of photons in Rich2 produced froms scintillation";
   m_hPhtotScintYRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2150",
                                            title, 400, -3700.0, 3700.0);

   title=" Z coordinate of origin of photons in Rich2 produced froms scintillation";
   m_hPhtotScintZRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2160",
                                            title, 400, 9400.0, 12400.0);


   title=" X vs Z coordinate of origin of photons in Rich2 produced froms scintillation";
   m_hPhtotScintXvsZRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2161",
                                              title, 400, 9400.0, 12400.0,400, -3700.0, 3700.0);
  
   title ="Wavelength of photons  in Rich2 produced froms scintillation";
    m_hPhtotScintWavelengthRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2170", 
                                                     title, 400, 100, 900.0);
    
    title = "Momentum of particles which produce photons in Rich2 from scintillation";
    m_hPhtotScintPartMomRich2 =  CurHistoSvc->book(m_RichG4HistoPathSet1+"2180",
                                                   title, 200, 0.0, 100000.0);
    
   title="On Detector Plane: Angle wrt Y axis for Photons in Rich2 produced from scintillation ";

   m_hPhDetPlaneScintVertAngleRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2200",
                                                   title,200, 0.0, 2.0);

   
   title="On Detector Plane: Angle wrt positive Z axis for Photons in Rich2 produced from scintillation ";
   
   m_hPhDetPlaneScintForwardAngleRich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"2210",
                                                   title,200, 0.0, 2.0); 


   title="On Detector Plane:Angle wrt negative Z axis for Photons in Rich2 produced from scintillation ";
  
   m_hPhDetPlaneScintBackwardAngleRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2220",
                                                       title, 200, 0.0, 2.0);
   
   
   title="On Detector Plane: Angle wrt X axis for Photons in Rich2 produced from scintillation ";
   
   m_hPhDetPlaneScintHorizAngleRich2  = CurHistoSvc->book(m_RichG4HistoPathSet1+"2230",
                                                   title,200, 0.0, 2.0); 


  title = "On Detector Plane: Angle wrt ForwardZ vs wrt X  for Photons in Rich2 from scintillation";
   
   m_hPhDetPlaneScintForwardVsHorizAngleRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2235",
                                                             title,200, 0.0, 2.0, 200, 0.0, 2.0 ); 

  title = "On Detector Plane: Angle wrt NegZ vs wrt X  for Photons in Rich2 from scintillation";
   
   m_hPhDetPlaneScintNegZVsHorizAngleRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2236",
                                                             title,200, 0.0, 2.0, 200, 0.0, 2.0 ); 

   title=" On Detector Plane: X coordinate of origin of photons in Rich2 produced froms scintillation";
   m_hPhDetPlaneScintXRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2240",
                                            title, 400, -3700.0, 3700.0);
    

   title="On Detector Plane:  Y coordinate of origin of photons in Rich2 produced froms scintillation";
   m_hPhDetPlaneScintYRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2250",
                                            title, 400, -3700.0, 3700.0);

   title=" On Detector Plane: Z coordinate of origin of photons in Rich2 produced froms scintillation";
   m_hPhDetPlaneScintZRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2260",
                                            title, 400, 9400.0, 12400.0);

   title=" On Detector Plane: X vs Z coordinate of origin of photons in Rich2 produced froms scintillation";
   m_hPhDetPlaneScintXvsZRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2261",
                                                   title, 400, 9400.0, 12400.0, 400, -3700.0, 3700.0);
   
   title =" On Detector Plane: Wavelength of photons  in Rich2 produced froms scintillation";

    m_hPhDetPlaneScintWavelengthRich2 = CurHistoSvc->book(m_RichG4HistoPathSet1+"2270", 
                                                     title, 400, 100, 900.0);
    
    title = "On Detector Plane: Momentum of particles which produce photons in Rich2 from scintillation";
    m_hPhDetPlaneScintPartMomRich2 =  CurHistoSvc->book(m_RichG4HistoPathSet1+"2280",
                                                   title, 200, 0.0, 100000.0);


}



