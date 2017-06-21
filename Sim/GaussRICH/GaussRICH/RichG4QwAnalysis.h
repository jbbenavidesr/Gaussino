// $Id: RichG4QwAnalysis.h,v 1.2 2006-02-27 14:10:30 seaso Exp $
#ifndef GAUSSRICH_RICHG4QWANALYSIS_H 
#define GAUSSRICH_RICHG4QWANALYSIS_H 1

// Include files

#include "GaudiKernel/NTuple.h"
#include "Geant4/G4Track.hh" 

 class TFile;
 class TTree;


/** @class RichG4QwAnalysis RichG4QwAnalysis.h RichAnalysis/RichG4QwAnalysis.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2006-02-21 
 *  Modif  09-05-2016  SE
 */

class RichG4QwAnalysis {
public: 

  virtual ~RichG4QwAnalysis( ); ///< Destructor
  static RichG4QwAnalysis* getRichG4QwAnalysisInstance();
  void InitQwAnalysis();
  void FillQwAnalysisHisto(const G4Track& aChTrack);
  void reset_NumPartInQwHisto() {
    m_qPart=0;
  }
  void WriteOutQwNtuple() {
    m_qNtuple->write();
  }
  bool qwAnalysisNtupleBooked() {return m_qwAnalysisNtupleBooked;}

  void setAnalysisNtupleFileName(G4String aString){
    m_RichAnaNtupFileName = aString;
    
  }
  

  void InitQwAnalysisA();
  void FillQwAnalysisHistoA(const G4Track& aChTrack);
  void EndofRunQwAnalysis();
  

protected:

private:
  /// Standard constructor kept private.
  RichG4QwAnalysis( ); 
  static RichG4QwAnalysis* RichG4QwAnalysisInstance;

  INTuple *  m_qNtuple;
  int  m_qPartMax; // max number of particles for the ntuple.
  
  NTuple::Item <long> m_qPart; // number of tracks used 
  
  NTuple::Array <float> m_ChProdX;
  NTuple::Array <float> m_ChProdY;
  NTuple::Array <float> m_ChProdZ;
  NTuple::Array <float> m_RDetNum;
  NTuple::Array <float> m_ChPartType;
  NTuple::Array <float> m_ChProcType;
  NTuple::Array <float> m_ChTotEner;
  NTuple::Array <float> m_RadNum; 
  NTuple::Array <float> m_ChDirX;
  NTuple::Array <float> m_ChDirY;
  NTuple::Array <float> m_ChDirZ;
  
  bool  m_qwAnalysisNtupleBooked; 
  
  G4String m_RichAnaNtupFileName;
  TFile* m_RichAnaNtupFile;
  TTree* m_RichAnaTree;

  G4double m_aChProdX;
  G4double m_aChProdY;
  G4double m_aChProdZ;
  G4double m_aRDetNum;
  G4double m_aChPartType;
  G4double m_aChProcType;
  G4double m_aChTotEner;
  G4double m_aRadNum;
  G4double m_aChDirX;
  G4double m_aChDirY;
  G4double m_aChDirZ;
  

};
#endif //GAUSSRICH_RICHG4QWANALYSIS_H
