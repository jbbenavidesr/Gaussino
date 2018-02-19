// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// from Geant4
#include "Geant4/G4Step.hh"
#include "Geant4/G4OpticalPhoton.hh"
#include "Geant4/G4Electron.hh"
#include "Geant4/G4Positron.hh"
#include "Geant4/G4Gamma.hh"
#include "Geant4/G4Timer.hh"
#include "Geant4/G4ParticleTable.hh"

// from Guadi
#include "GaudiKernel/DeclareFactoryEntries.h" //new
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/PropertyMgr.h"

// from GiGa
#include "GiGa/IGiGaSvc.h"
//#include "GiGa/GiGaMACROs.h"

// from GaussTools
#include "GaussTools/GaussTrackInformation.h"

// from AIDA
#include "AIDA/IHistogram1D.h"

// local
#include "MonitorTiming.h"

// stream
#include <fstream>
#include <cstdlib>

/* file
 *
 *  Implementation of class MonitorTiming for timing studies
 *
 *  author Olga Levitskaya, Gloria Corti, James McCarthy
 */

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================

DECLARE_TOOL_FACTORY( MonitorTiming )

// ============================================================================
// Standard constructor, initialize variables
// ============================================================================
/** standard constructor
 *  @see GiGaStepActionBase
 *  @see GiGaBase
 *  @see AlgTool
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================

MonitorTiming::MonitorTiming(const std::string& type,
                             const std::string& name,
                             const IInterface*  parent)
  : GiGaStepActionBase(type, name, parent)
  , m_TimerFileName()
{
  declareProperty("TimerFileName", m_TimerFileName);
  declareProperty("Volumes", knownVolumes);

  // timer instantiation
  totalTimer = new G4Timer;
  stepTimer = new G4Timer;

  theSteppingManager = new G4SteppingManager;
}

// ============================================================================
/// destructor
// ============================================================================
MonitorTiming::~MonitorTiming()
{
  delete totalTimer;
  delete stepTimer;
  delete theSteppingManager;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MonitorTiming::initialize()
{

  info()<<"*********** Initialising MonitorTimingTool ************"<<endmsg;

  m_TimerFileName2 = m_TimerFileName.data();

  //initialise the total cumulated time
  totalCumTime=0;

  std::stringstream sstream;

  for(size_t i=0;i<knownVolumes.size();i++){
    sstream<<"/"<<knownVolumes[i];
    knownVolumes_2.push_back(sstream.str());
    sstream.str("");
  }

  totalTimer->Start();

  //initially set to true for the first loop
  firstStep=true;

  StatusCode status =  GiGaStepActionBase::initialize();
  return Print("Initialized successfully", StatusCode::SUCCESS, MSG::VERBOSE);
}

StatusCode MonitorTiming::finalize()
{
  info()<<"*********** Finalising MonitorTimingTool ************"<<endmsg;

  stepTimer->Stop();

  //Get the total time for the simulation
  totalTimer->Stop();
  totalTime=totalTimer->GetRealElapsed();

  // output into file
  std:: ofstream table(m_TimerFileName2);
  if(!table.is_open())
  {
    info()<<"MonitorTiming:: Timer File not opended :: EXIT"<<endmsg;
    exit(1);
  }

  table<<"************** Volume Timing ***************"<<std::endl;

  for(size_t i=0;i<volumes.size();i++){
    totalCumTime+=cumulated_time_vol[i];
    table<<"Volume "<<i<<": "<<volumes[i]<<" cumulated time "<<cumulated_time_vol[i]<<" seconds"<<std::endl;
  }

  table << "Total Time: " << totalCumTime << std::endl;

  debug()<<"********** Other Volumes **********"<<endmsg;
  table<<std::endl<<std::endl<<"************ Other Volumes ***********"<<std::endl;

  for(size_t i=0;i<otherVolumes.size();i++) table<<"Other Volume "<<i<<": "<<otherVolumes[i]<<std::endl;

  debug()<<"********** Process Timing *********"<<endmsg;
  table<<std::endl<<std::endl<<"************** Process Timing **************"<<std::endl;
  for(size_t i=0;i<processes.size();i++) table<<"Process "<<i<<": "<<processes[i]<<" cumulated time "<<cumulated_time_proc[i]<<" seconds"<<std::endl;

  WriteSubdetTiming(table,cumulatedtime_velo,particletype_velo,"Velo");
  WriteSubdetTiming(table,cumulatedtime_tt,particletype_tt,"TT");
  WriteSubdetTiming(table,cumulatedtime_it,particletype_it,"IT");
  WriteSubdetTiming(table,cumulatedtime_ot,particletype_ot,"OT");
  WriteSubdetTiming(table,cumulatedtime_rich1,particletype_rich1,"Rich1");
  WriteSubdetTiming(table,cumulatedtime_rich2,particletype_rich2,"Rich2");
  WriteSubdetTiming(table,cumulatedtime_mag,particletype_mag,"Magnet");
  WriteSubdetTiming(table,cumulatedtime_spd,particletype_spd,"Spd");
  WriteSubdetTiming(table,cumulatedtime_prs,particletype_prs,"Prs");
  WriteSubdetTiming(table,cumulatedtime_ecal,particletype_ecal,"Ecal");
  WriteSubdetTiming(table,cumulatedtime_hcal,particletype_hcal,"Hcal");
  WriteSubdetTiming(table,cumulatedtime_muon,particletype_muon,"Muon");
  WriteSubdetTiming(table,cumulatedtime_pipe,particletype_pipe,"Pipe");
  WriteSubdetTiming(table,cumulatedtime_converter,particletype_converter,"Converter");
  WriteSubdetTiming(table,cumulatedtime_All,particletype_All,"All");

  table<<std::endl<<"************** Summary of total time in each volume **************"<<std::endl;
  for (int i=0;i<(int)detsummary_detname_vec.size();i++){
    char* line_string=new char[200];
    sprintf(line_string,"Total time in %s: %.2f seconds (%.2f%% of the total)",detsummary_detname_vec[i].c_str(),detsummary_time_vec[i],detsummary_fraction_vec[i]);
    table<<line_string<<std::endl;
    delete line_string;
    //table<<std::endl<<detsummary_detname
  }
  table << std::endl;

  info()<<"******* Total Cumulated Time: "<<totalCumTime<<" *********"<<endmsg;
  info()<<"******* Total Actual Time: "<<totalTime<<" *********"<<endmsg;

  Print("Finalization" , StatusCode::SUCCESS , MSG::VERBOSE );
  return  GiGaStepActionBase::finalize();
}


// ============================================================================
// ============================================================================
/*
 *            *****    ****     ****      ******
 *           *        *    *    *    *    *
 *          *        *      *   *     *   ****
 *           *        *    *    *    *    *
 *            *****    ****     ****      ******
 */
// ============================================================================
/** stepping action
 *  @see G4UserSteppingAction
 *  @param step Geant4 step
 */
// ============================================================================

void MonitorTiming::UserSteppingAction(const G4Step* theStep)
{

  // I_  TIMER INITIALIZATION =====================================================

  //Check if this is the first step in the event
  if(firstStep){
    stepTime=0;
    firstStep=false;
  }
  else{
    stepTimer->Stop();
    stepTime=stepTimer->GetRealElapsed();
  }

  //get track info
  track = theStep->GetTrack();
  partdef = track->GetDefinition();

  std::string ParticleName = partdef->GetParticleName();
  int abslundid=abs(partdef->GetPDGEncoding());

  //Getting step information

  // III_ VOLUME TIMING ======================================================

  Vol=theStep->GetPreStepPoint()->GetPhysicalVolume();

  /*
    Get the volume name

    Loop over known detector names to check for matches in the volume name
    If none of the known detector names are in the volume name,
    add it to the otherVolumes vector
  */

  VolName=(std::string)Vol->GetName();

  // See if the volume is in the list of known detectors
  bool foundKnownVol=false;
  for(size_t i=0;i<knownVolumes_2.size();i++){
    if(VolName.find(knownVolumes_2[i]) != std::string::npos){
      VolumeName=knownVolumes[i];
      foundKnownVol=true;
    }
  }

  bool newVolume=true; //boolean to check if G4 has been in this volume already

  if(!foundKnownVol){
    VolumeName="OtherVolume";
    for(size_t i=0;i<otherVolumes.size();i++){
      if(otherVolumes[i]==VolName){
        newVolume=false;
        break;
      }
    }
    if(newVolume) otherVolumes.push_back(VolName);

  }

  newVolume=true;

  // add to the cumulative time for the volume we are in
  // if this is the first time in the volume, add it to the volumes vector
  for(size_t i=0;i<volumes.size();i++){
    if(VolumeName==volumes[i]){
      cumulated_time_vol[i]+=stepTime;
      newVolume=false;
    }
  }

  if(newVolume){
    volumes.push_back(VolumeName);
    cumulated_time_vol.push_back(stepTime);
  }

  FillSubdetTiming(abslundid,stepTime,cumulatedtime_velo,particletype_velo,"Velo");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_tt,particletype_tt,"TT");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_it,particletype_it,"IT");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_ot,particletype_ot,"OT");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_rich1,particletype_rich1,"Rich1");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_rich2,particletype_rich2,"Rich2");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_mag,particletype_mag,"Magnet");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_spd,particletype_spd,"Spd");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_prs,particletype_prs,"Prs");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_ecal,particletype_ecal,"Ecal");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_hcal,particletype_hcal,"Hcal");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_muon,particletype_muon,"Muon");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_pipe,particletype_pipe,"Pipe");
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_converter,particletype_converter,"Converter");
  //NB: OtherVolume is missing
  FillSubdetTiming(abslundid,stepTime,cumulatedtime_All,particletype_All,"All");

  // Keep track of the full name of all sub-volumes
  newVolume=true;
  for(size_t i=0;i<subVolumes.size();i++){
    if(VolName==subVolumes[i]) {
      cumulated_time_subvol[i]+=stepTime;
      newVolume=false;
    }
  }
  if(newVolume){
    subVolumes.push_back(VolName);
    cumulated_time_subvol.push_back(stepTime);
  }

  // PROCESS TIMING ==========================================

  processName=theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

  bool newProcess=true; //boolean to check if G4 has been in this volume already

  // add to the cumulative time for the process
  // if this is the first time for this process, add it to the processess vector
  for(size_t i=0;i<processes.size();i++){
    if(processName==processes[i]){
      cumulated_time_proc[i]+=stepTime;
      newProcess=false;
    }
  }

  if(newProcess){
    processes.push_back(processName);
    cumulated_time_proc.push_back(stepTime);
  }
  stepTimer->Start();
}

void MonitorTiming::FillSubdetTiming(int abslundid,double stepTime,std::vector<double>& cumulatedtime_det,std::vector<int>& particletype_det,std::string det){
  if (det=="All" || det==VolumeName){//NB:VolumeName is private member
    bool newType=true;
    for (size_t i=0;i<particletype_det.size();i++){
      if (particletype_det[i]==abslundid){
        cumulatedtime_det[i]+=stepTime;
        newType=false;
      }
    }//end for loop
    if (newType){
      particletype_det.push_back(abslundid);
      cumulatedtime_det.push_back(stepTime);
    }
  }//end VolumeName==det cond

  return;
}//end FillSubdetTiming


void MonitorTiming::WriteSubdetTiming(std:: ofstream& table,std::vector<double> cumulatedtime_det,std::vector<int> particletype_vec,std::string det){
  table<<std::endl<<std::endl<<"************** Timing per particle type in "<<det<<" (fraction in "<<det<<") **************"<<std::endl;
  double time_det=0;
  std::vector<int> sortindex_vec=SortList(cumulatedtime_det);
  for (size_t i=0;i<particletype_vec.size();i++) time_det+=cumulatedtime_det[i];
  for (size_t i=0;i<particletype_vec.size();i++){
  }
  for (size_t i=0;i<particletype_vec.size();i++){
    int index=sortindex_vec[i];
    if (cumulatedtime_det[index]==0) continue;
    G4ParticleDefinition* partdef= G4ParticleTable::GetParticleTable()->FindParticle(particletype_vec[index]);
    std::string ParticleName;
    if (partdef) ParticleName=partdef->GetParticleName();
    else if (particletype_vec[index]==0) ParticleName="opticalphoton";
    else ParticleName="unknown";
    char* line_string=new char[200];
    if (ParticleName.size()<8) ParticleName+="\t";
    sprintf(line_string,"%s\t: cumulated time %.2f seconds (%.2f%%)",ParticleName.c_str(),cumulatedtime_det[index],100*cumulatedtime_det[index]/time_det);
    table<<line_string<<std::endl;
    delete line_string;
  }
  char* line_string=new char[200];
  sprintf(line_string,"Time in %s: %.2f seconds (%.2f%% of the total)",det.c_str(),time_det,100*time_det/totalCumTime);
  table<<line_string<<std::endl;
  delete line_string;

  detsummary_detname_vec.push_back(det.c_str());
  detsummary_time_vec.push_back(time_det);
  double fraction=100*time_det/totalCumTime;
  detsummary_fraction_vec.push_back(fraction);

  return;
}//end WriteSubdetTiming

std::vector<int> MonitorTiming::SortList(std::vector<double> vec){
  std::vector<int> out_vec;
  int size=(int)vec.size();
  for (int i=0;i<(int)vec.size();i++) out_vec.push_back(i);

  int j;
  do{
    j = 0;
    for (int i=0;i<(size-1);i++){
      if (vec[i+1]>vec[i]){
        j = 1;
        double temp=vec[i];
        vec[i]=vec[i+1];
        vec[i+1]=temp;
        int temp2=out_vec[i];
        out_vec[i]=out_vec[i+1];
        out_vec[i+1]=temp2;
      }
    }//end i<mat_dim
  } while (j == 1);

  return out_vec;
}//end SortList

// ============================================================================
// ============================================================================
// The END
// ============================================================================
