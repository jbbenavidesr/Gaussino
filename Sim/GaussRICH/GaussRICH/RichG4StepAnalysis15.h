#ifndef GAUSSRICH_RICHG4STEPANALYSIS15_H 
#define GAUSSRICH_RICHG4STEPANALYSIS15_H 1

// Include files
#include "GiGa/GiGaStepActionBase.h"
template <class TYPE> class GiGaFactory;
class G4Step;

/** @class RichG4StepAnalysis15 RichG4StepAnalysis15.h GaussRICH/RichG4StepAnalysis15.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2016-05-22
 */
class RichG4StepAnalysis15:virtual public  GiGaStepActionBase  {
friend class GiGaFactory<RichG4StepAnalysis15>;
public: 
  /// Standard constructor
  RichG4StepAnalysis15(
    const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ); 

  virtual ~RichG4StepAnalysis15( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );
  void    FillHistoRich2Scint(G4int aSNum , G4int aProcNum );

protected:

private:

  RichG4StepAnalysis15();
  RichG4StepAnalysis15(const  RichG4StepAnalysis15& );
  RichG4StepAnalysis15& operator=(const  RichG4StepAnalysis15& );

  G4int m_Rich2ScintMaxStepNumCutValue;
  G4int m_Rich2CkvMaxStepNumCutValue;
  


};
#endif // GAUSSRICH_RICHG4STEPANALYSIS15_H
