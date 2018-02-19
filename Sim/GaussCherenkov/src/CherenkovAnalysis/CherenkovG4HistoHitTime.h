#ifndef CHERENKOVANALYSIS_CHERENKOVG4HISTOHITTIME_H 
#define CHERENKOVANALYSIS_CHERENKOVG4HISTOHITTIME_H 1
// Include files
#include "Geant4/globals.hh"

#include <string>
#include <cmath>
#include <vector>

#include "GaussCherenkov/CkvG4SvcLocator.h"
#include "GaudiKernel/NTuple.h"
#include "Geant4/G4Event.hh"

 class TFile;
 class TTree;

#include <map>


/** @class CherenkovG4HistoHitTime CherenkovG4HistoHitTime.h CherenkovAnalysis/CherenkovG4HistoHitTime.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2016-07-10
 */
class CherenkovG4HistoHitTime {
public: 

  virtual ~CherenkovG4HistoHitTime( ); ///< Destructor
  static CherenkovG4HistoHitTime*  getCherenkovG4HistoHitTimeInstance();

  void InitCherenkovG4NtupHitTime();
  void FillG4NtupHitTime(const G4Event* anEvent, int  NumRichColl, const std::vector<int> & RichG4CollectionID  );
  void EndofRunG4NtupHitTime();
  void ResetCkvTimeParam();
  void StoreTrajectInfo(const G4Event* anEvent);
  
  void SetCkvAnaNtupHitTimeFileName( G4String aFileName)
  {  m_CkvAnaNtupHitTimeFileName = aFileName; }
  
  

protected:

private:
  /// Standard constructor
  CherenkovG4HistoHitTime( ); 

  

  static CherenkovG4HistoHitTime*  CherenkovG4HistoHitTimeInstance;
 


  bool m_CkvHitTimeNtupleBooked;

  G4String m_CkvAnaNtupHitTimeFileName;
  TFile* m_RichAnaHitTimeNtupFile;
  TTree* m_RichAnaHitTimeTree;

  

  G4double  m_aHitTime; 
  G4int  m_aRichDetNum;
  G4int  m_aRichHitCollection;
  G4int  m_aChParticleType;
  G4int  m_aChTrackID;
  G4double  m_aChPartOriginX;
  G4double  m_aChPartOriginY;
  G4double  m_aChPartOriginZ;
  G4double  m_aChPartTotMom;
  G4double  m_aHitCoordX;
  G4double  m_aHitCoordY;
  G4double  m_aHitCoordZ;
  G4double  m_aChOriginTime;
  
  
  std::multimap<G4int,G4ThreeVector> m_RichTrajIDInitPosMap;
  
  std::multimap<G4int,G4double> m_RichTrajIDInitTimeMap;


};
#endif // CHERENKOVANALYSIS_CHERENKOVG4HISTOHITTIME_H
