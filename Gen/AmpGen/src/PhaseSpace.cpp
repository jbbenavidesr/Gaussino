// @(#)root/physics:$Id$
// Author: Rene Brun , Valerio Filippini  06/09/2000

//_____________________________________________________________________________________
//
//  Utility class to generate n-body event,
//  with constant cross-section (default)
//  or with Fermi energy dependence (opt="Fermi").
//  The event is generated in the center-of-mass frame,
//  but the decay products are finally boosted
//  using the betas of the original particle.
//
//  The code is based on the GENBOD function (W515 from CERNLIB)
//  using the Raubold and Lynch method
//      F. James, Monte Carlo Phase Space, CERN 68-15 (1968)
//
// see example of use in $ROOTSYS/tutorials/physics/PhaseSpace.C
//
// Note that Momentum, Energy units are Gev/C, GeV

#include "AmpGen/PhaseSpace.h"
#include "TRandom.h"
#include "TMath.h"
#include <array>

const Int_t kMAXP = 18;

////////////////////////////////////////////////////////////////////////////////

///the PDK function

using namespace AmpGen;

Double_t PhaseSpace::PDK(Double_t a, Double_t b, Double_t c)
{
  Double_t x = (a-b-c)*(a+b+c)*(a-b+c)*(a+b-c);
  x = TMath::Sqrt(x)/(2*a);
  return x;
}

////////////////////////////////////////////////////////////////////////////////
/// Assignment operator

PhaseSpace& PhaseSpace::operator=(const PhaseSpace &gen)
{
  fNt      = gen.fNt;
  fWtMax   = gen.fWtMax;
  fTeCmTm  = gen.fTeCmTm;
 
  for (Int_t i=0;i<fNt;i++) 
 //   fPrims.push_back(gen.fPrims[i]);
    fMass[i]   = gen.fMass[i];
  
  for (Int_t i=0;i<fNt;i++)
    fDecPro[i] = gen.fDecPro[i];
  
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
///  Generate a random final state.
///  The function returns the weigth of the current event.
///  The TLorentzVector of each decay product can be obtained using GetDecay(n).
///
/// Note that Momentum, Energy units are Gev/C, GeV

Double_t PhaseSpace::Generate()
{
  
//  std::vector<double> rno( fNt, 0 );
  std::array<double,kMAXP> rno;
  Double_t  pd[kMAXP];
  Double_t invMas[kMAXP];
     
  rno[0] = 0;
  Int_t n;

  double wt=fWtMax;
  do { 
    wt=fWtMax;
    for (n=1; n<fNt-1; n++) rno[n]=rndm();   // fNt-2 random numbers
    std::sort( rno.begin()+1, rno.begin()+fNt  );

    rno[fNt-1] = 1;
    double sum=0;
    for (n=0; n<fNt; n++) {
      sum      += fMass[n];
      invMas[n] = rno[n]*fTeCmTm + sum;
    }
    for (n=0; n<fNt-1; n++) {
      pd[n] = PDK(invMas[n+1],invMas[n],fMass[n+1]);
      wt *= pd[n];
    }
  } while ( wt < rndm() );
  
  /* 
     only compute the full event if is phaseSpace -> very large 
     speed increase compared with default ROOT implementation 
     if trying to generate "unweighted" event. 
     also, this is much more intuitive 
  */

  //
  //-----> complete specification of event (Raubold-Lynch method)
  //
  fDecPro[0].SetPxPyPzE(0, pd[0], 0 , sqrt(pd[0]*pd[0]+fMass[0]*fMass[0]) );

  Int_t i=1;
  Int_t j;
  while (1) {
    fDecPro[i].SetPxPyPzE(0, -pd[i-1], 0 , sqrt(pd[i-1]*pd[i-1]+fMass[i]*fMass[i]) );
    Double_t cZ   = 2*rndm() - 1;
    Double_t sZ   = sqrt(1-cZ*cZ);
    Double_t angY = 2*M_PI * rndm();
    Double_t cY   = cos(angY);
    Double_t sY   = sin(angY);
    for (j=0; j<=i; j++) {
      TLorentzVector& v = fDecPro[j];
      Double_t x = v.Px();
      Double_t y = v.Py();
      v.SetPx( cZ*x - sZ*y );
      v.SetPy( sZ*x + cZ*y );   // rotation around Z
      x = v.Px();
      Double_t z = v.Pz();
      v.SetPx( cY*x - sY*z );
      v.SetPz( sY*x + cY*z );   // rotation around Y
    }

    if (i == (fNt-1)) break;
    double beta = pd[i] / sqrt(pd[i]*pd[i] + invMas[i]*invMas[i]);
    for (j=0; j<=i; j++) fDecPro[j].Boost(0,beta,0);
    i++;
  }

  return wt;
}

////////////////////////////////////////////////////////////////////////////////
///return Lorentz vector corresponding to decay n

TLorentzVector *PhaseSpace::GetDecay(Int_t n)
{
  if (n>fNt) return 0;
  return &( fDecPro[n] );
}


////////////////////////////////////////////////////////////////////////////////
/// input:
/// TLorentzVector &P:    decay particle (Momentum, Energy units are Gev/C, GeV)
/// Int_t nt:             number of decay products
/// Double_t *mass:       array of decay product masses
/// Option_t *opt:        default -> constant cross section
///                       "Fermi" -> Fermi energy dependece
/// return value:
/// kTRUE:      the decay is permitted by kinematics
/// kFALSE:     the decay is forbidden by kinematics
///

Bool_t PhaseSpace::SetDecay(TLorentzVector &P, Int_t nt, const Double_t *mass){
  std::vector<double> masses;
  masses.assign( mass, mass+nt);
  return SetDecay( P.Mag(), masses );
}

Bool_t PhaseSpace::SetDecay( const double& m0, const std::vector<double>& mass )
{
  Int_t n;
  fNt = mass.size();
  fDecPro.resize( fNt , TLorentzVector(0,0,0,0));
 
  fTeCmTm = m0;           // total energy in C.M. minus the sum of the masses
  for (n=0;n<fNt;n++) {
    fMass[n]  = mass[n];
    fTeCmTm  -= mass[n];
  }

  if (fTeCmTm<=0) return kFALSE;    // not enough energy for this decay

  Double_t emmax = fTeCmTm + fMass[0];
  Double_t emmin = 0;
  Double_t wtmax = 1;
  for (n=1; n<fNt; n++) {
    emmin += fMass[n-1];
    emmax += fMass[n];
    wtmax *= PDK(emmax, emmin, fMass[n]);
  }
  fWtMax = 1/wtmax;

  return kTRUE;
}

PhaseSpace::PhaseSpace(const PhaseSpace &gen) 
{
  fNt      = gen.fNt;
  fWtMax   = gen.fWtMax;
  fTeCmTm  = gen.fTeCmTm;
  for (Int_t i=0;i<fNt;i++) {
    fMass[i]   = gen.fMass[i];
    fDecPro[i] = gen.fDecPro[i];
  }
}


