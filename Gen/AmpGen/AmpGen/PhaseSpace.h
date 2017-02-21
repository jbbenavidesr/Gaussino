// @(#)root/physics:$Id$
// Author: Rene Brun , Valerio Filippini  06/09/2000

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//   Phase Space Generator, based on the GENBOD routine of CERNLIB           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_PhaseSpace
#define ROOT_PhaseSpace

#include "TLorentzVector.h"
#include "TRandom.h"
#include <random>

namespace AmpGen { 
  class PhaseSpace  {

    private:
    Int_t        fNt;             // number of decay particles
    Double_t     fMass[18];       // masses of particles
    Double_t     fTeCmTm;         // total energy in the C.M. minus the total mass
    Double_t     fWtMax;          // maximum weigth
    std::vector<TLorentzVector> fDecPro;
    TRandom* fRand;
  
    double rndm(){ return fRand->Rndm() ; } 
    Double_t PDK(Double_t a, Double_t b, Double_t c);

    public:
    PhaseSpace(): fNt(0),   fTeCmTm(0.), fWtMax(0.),fRand(0) {}
    PhaseSpace(const PhaseSpace &gen);
    PhaseSpace& operator=(const PhaseSpace &gen);

    Bool_t          SetDecay(const double& m0, const std::vector<double>& mass);
    Bool_t          SetDecay(TLorentzVector &P, Int_t nt, const Double_t *mass);
    Double_t        Generate();
    TLorentzVector *GetDecay(Int_t n);

    Int_t    GetNt()      const { return fNt;}
    Double_t GetWtMax()   const { return fWtMax;}
    
    void     setRandom( TRandom* rand ) { 
      
   fRand = rand ; } 
  };
}
#endif

