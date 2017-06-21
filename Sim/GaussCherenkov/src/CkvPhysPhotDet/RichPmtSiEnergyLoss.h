#ifndef RichPmtSiEnergyLoss_h
#define RichPmtSiEnergyLoss_h 1

#include "Geant4/globals.hh"
#include <vector>

// #include "Geant4/G4VEnergyLoss.hh" // Removed from G4r10?
#include "Geant4/G4VContinuousDiscreteProcess.hh"

#include "Geant4/Randomize.hh"
#include "Geant4/G4VParticleChange.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4ios.hh"
#include "GiGa/GiGaBase.h"

// class RichPmtSiEnergyLoss : public G4VEnergyLoss {
class RichPmtSiEnergyLoss : public G4VContinuousDiscreteProcess {

public:
  RichPmtSiEnergyLoss(const G4String& processName ,G4ProcessType   aType = fUserDefined  );

  virtual ~RichPmtSiEnergyLoss();

  G4bool IsApplicable(const G4ParticleDefinition&);
  // true for all charged particles

  G4double GetContinuousStepLimit(const G4Track& track,
                                  G4double previousStepSize,
                                  G4double currentMinimumStep,
                                  G4double& currentSafety);
  // for all materials other than the PmtSilicon this limit
  // is a large number so that the process is not invoked.
  G4VParticleChange* AlongStepDoIt(const G4Track& aTrack,
                                   const G4Step& aStep) ;

  G4double GetMeanFreePath(const G4Track& track,
                           G4double previousStepSize,
                           G4ForceCondition* condition);
  // the meanfree path
  // is a large number so that the process is not invoked.
  // at all as a post step process.
  //
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                  const G4Step& aStep);
  G4double RichPmtSiEnergyDeposit(G4double ElossInput);
  G4double RichPmtSiBackScatter(G4double EnergyInput);
  void InitializePmtProcParam();

  void setPmtSiDetEff(double aValueA) {  m_PmtSiDetEff= (G4double) aValueA;}
  void setPmtSiPixelChipEff(double aValueB){ m_SiPixelChipEff = (G4double) aValueB ;}     

  void setPmtPeBackScaProb(double aValueC) {PeBackScaProb=(G4double) aValueC ;}
  
    
private:

  // hide assignment and copy operators.
  RichPmtSiEnergyLoss ( RichPmtSiEnergyLoss &);
  RichPmtSiEnergyLoss & operator=(const  RichPmtSiEnergyLoss &right);

  //now the data members.
  G4int fMatIndexPmtSiEloss;
  G4int fMatIndexPmtEnvelopeKovar;
  G4String PmtSiElossMaterialName;
  G4String PmtEnvelopeMaterialName;
  G4double MinKineticEnergy ;
  G4double MipEnergyPmtSiEloss;
  G4double finalRangeforSiDetStep;
  G4double PhElectronMaxEnergy;
  G4double SiHitDetGlobalEff;

  G4double m_PmtSiDetEff;  
  G4double m_SiPixelChipEff;
  
    
  G4double PeBackScaProb;
  G4double PeBackScaProbCorrected;

  G4double m_siliconDetXSize;
  G4double m_siliconDetYSize;
  G4double m_siliconDetZSize;
  
  
};

inline G4double RichPmtSiEnergyLoss::RichPmtSiBackScatter (G4double EnergyInput)
{
  return EnergyInput;
}

#endif
