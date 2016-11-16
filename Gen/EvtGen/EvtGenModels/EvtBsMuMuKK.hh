#ifndef EVTBSMUMUKK_HH
#define EVTBSMUMUKK_HH

#include <string>
#include <iostream> 
#include <iomanip> 

#include "EvtGenBase/EvtDecayAmp.hh"
#include "EvtGenBase/EvtCPUtil.hh"
#include "EvtGenBase/EvtPDL.hh"
 
class EvtParticle;

class EvtBsMuMuKK : public EvtDecayAmp
{
 public:
  virtual std::string getName()
  { return "BS_MUMUKK"; }

  virtual EvtDecayBase* clone()
  { return new EvtBsMuMuKK; }

  virtual void init();

  virtual void initProbMax();

  virtual void decay( EvtParticle *p );

  double Int_const_sq(  const double M_KK_ll, 
                        const double M_KK_ul,
                        const int JR, 
                        const int JB  );

  EvtComplex Flatte( const double m0, 
                     const double m );
  
  double Int_Flatte_sq( const double m0, 
                        const double M_KK_ll, 
                        const double M_KK_ul,
                        const int JR, 
                        const int JB  );
 
  EvtComplex get_rho(const double m0, 
                     const double m);
  
  EvtComplex Breit_Wigner( const double Gamma0, 
                           const double m0, 
                           const double m, 
                           const int J, 
                           const double q0, 
                           const double q );

  double Int_Breit_Wigner_sq( const double Gamma0, 
                              const double m0,
                              const int JR, 
                              const int JB, 
                              const double q0,
                              const double M_KK_ll, 
                              const double M_KK_ul );
  
  double X_J(const int J, 
             const double q,
             const int isB);
  
  double wigner_d(int J, 
                  int l, 
                  int alpha, 
                  double theta);
  
  EvtComplex angular_distribution(int J, 
                                  int l, 
                                  int alpha, 
                                  double cK, 
                                  double cL, 
                                  double chi);
  
  EvtComplex Amp_time_q(int q, 
                        EvtComplex gplus, 
                        EvtComplex gminus, 
                        double delta,
                        double lambda_abs,
                        double Amp,
                        double phis,
                        int eta  );
  
  int gen_q();
  
  private:
  double MBs, MJpsi, Mf0, Mphi, MKp, MKm, MK0, Mpip, Mpi0, Mmu, Gamma0phi;
  double kin_lower_limit, kin_upper_limit, kin_middle;
  double p30Kp_mid_CMS, p30Kp_ll_CMS, p30Kp_phi_CMS; 
  double p30Jpsi_mid_CMS, p30Jpsi_ll_CMS, p30Jpsi_phi_CMS;
  double int_const_NR, int_Flatte_f0, int_BW_phi;
};
#endif //EVTBSMUMUKK_HH
