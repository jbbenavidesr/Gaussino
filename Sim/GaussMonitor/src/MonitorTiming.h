// ============================================================================
#ifndef       GaussTools_MonitorTiming_H
#define       GaussTools_MonitorTiming_H 1 
// ============================================================================
// include files 
/// Geant4
#include "Geant4/G4Timer.hh"
#include "Geant4/G4VProcess.hh"
#include "Geant4/G4ProcessManager.hh"
// GiGa
#include "GiGa/GiGaStepActionBase.h"
#include "Geant4/G4SteppingManager.hh"
// Gaudi Kernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/NTuple.h"
//GaussTools
#include "GaussTools/GaussTrackInformation.h"
/// stream
#include <fstream>
#include <cstdlib>
// forward declarations 
template <class TYPE> class GiGaFactory;

/** @class MonitorTiming MonitorTiming.h
 *   
 *  @author  James McCarthy
 *  @date    12/2013
 */

class MonitorTiming: virtual public GiGaStepActionBase
{
  /// friend factory for instantiation
  friend class GiGaFactory<MonitorTiming>;
  
  //protected:
  
  /** standard constructor 
   *  @see GiGaStepActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */

public: 
  
  MonitorTiming
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor 
  ~MonitorTiming();
  
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  
public:
  
  /* stepping action
   *  @see G4UserSteppingAction
   *  @param step Geant4 step
   */
  
  /* MonitorTiming(); ///< no default constructor
  MonitorTiming( const MonitorTiming& ); ///< no copy  
  MonitorTiming& operator=( const MonitorTiming& ) ; ///< no =*/

  virtual void UserSteppingAction ( const G4Step* );
  void OutputDatas();
  void MSInit();
  
 private:
 
  //Step information variables
  G4StepPoint *thePreStepPoint,*thePostStepPoint;

  //Timers
  G4Timer *totalTimer, *stepTimer; //*trackTimer;

  //Volume variables
  const G4VPhysicalVolume* Vol;
  std::vector<double> cumulated_time_vol;
  std::vector<std::string> volumes;
  
  std::vector<std::string> subVolumes;
  std::vector<double> cumulated_time_subvol;
  
  //aggiunta 28/2/16
  //std::vector<double> cumulated_time_vol_muons;
  G4Track* track;
  G4ParticleDefinition* partdef;
  //G4double totalCumTime_muons;
  std::vector<int> particletype_velo;
  std::vector<double> cumulatedtime_velo;
  std::vector<int> particletype_tt;
  std::vector<double> cumulatedtime_tt;
  std::vector<int> particletype_it;
  std::vector<double> cumulatedtime_it;
  std::vector<int> particletype_ot;
  std::vector<double> cumulatedtime_ot;
  std::vector<int> particletype_rich1;
  std::vector<double> cumulatedtime_rich1;
  std::vector<int> particletype_rich2;
  std::vector<double> cumulatedtime_rich2;
  std::vector<int> particletype_mag;
  std::vector<double> cumulatedtime_mag;
  std::vector<int> particletype_spd;
  std::vector<double> cumulatedtime_spd;
  std::vector<int> particletype_prs;
  std::vector<double> cumulatedtime_prs;
  std::vector<int> particletype_ecal;
  std::vector<double> cumulatedtime_ecal;
  std::vector<int> particletype_hcal;
  std::vector<double> cumulatedtime_hcal;
  std::vector<int> particletype_muon;
  std::vector<double> cumulatedtime_muon;
  std::vector<int> particletype_pipe;
  std::vector<double> cumulatedtime_pipe;
  std::vector<int> particletype_converter;
  std::vector<double> cumulatedtime_converter;
  std::vector<int> particletype_other;
  std::vector<double> cumulatedtime_other;
  std::vector<int> particletype_All;
  std::vector<double> cumulatedtime_All;

  std::vector<std::string> detsummary_detname_vec;
  std::vector<double> detsummary_time_vec;
  std::vector<double> detsummary_fraction_vec;

  void FillSubdetTiming(int abslundid,double stepTime,std::vector<double>& cumulatedtime_det,std::vector<int>& particletype_vec,std::string det);
  void WriteSubdetTiming(std:: ofstream& table,std::vector<double> cumulatedtime_det,std::vector<int> particletype_vec,std::string det);
  std::vector<int> SortList(std::vector<double> vec);

  //fine aggiunta
  
  //std::vector<std::vector<std::string> > vec_subVolumes;
  
  std::vector<std::string> knownVolumes;
  std::vector<std::string> knownVolumes_2;
  std::vector<std::string> otherVolumes;
  std::string VolName, VolumeName;

  G4double stepTime, totalTime, totalCumTime;

  bool firstStep;


  //process variables
  G4SteppingManager* theSteppingManager;

  std::string processName;
  std::vector<double> cumulated_time_proc;
  std::vector<std::string> processes;
    
  //Particle Control Variables
  //G4Track* track;
  
  //G4VUserTrackInformation* uinf;
  
  //G4ParticleDefinition* partdef;
  
  //GaussTrackInformation* ginf;

  //const G4VProcess* theCProcess;
  //std ::string oldvolname, ParticleName, old_name, initial_volume,CPname;
  
  //G4ThreeVector DeltaPos, position, initial_position;
  //G4Track *oldTrack;
  //G4double energy, radius, max_radius, time, time0, timecumul, initial_energy;
  //int r, loops, volchanged; // r is a boolean variable to check if the clock already Runs
  //int keep, stepnb;
    
public:
  std::ofstream Vmsgr;
  
  
private:
  //std::string  m_OutputFileName;
  std::string  m_TimerFileName;
  //const char*  m_OutputFileName2;
  const char*  m_TimerFileName2;

  //std::vector<sdt::string> m_Volumes;
  /*
  NTuple::Tuple* m_datas;
  
  NTuple::Item<long>              m_ntrk;
  NTuple::Item<float>             m_energy;
  // Items for the column wise n-tuple
  NTuple::Array<long>             m_iNumbers;
  NTuple::Array<float>            m_fNumbers;
  NTuple::Item<long>              m_n;
  */
};

// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<    GaussTools_MonitorTiming_H
// ============================================================================
