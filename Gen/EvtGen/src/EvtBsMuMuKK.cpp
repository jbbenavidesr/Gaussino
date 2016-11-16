//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2000      Caltech, UCSB
//
// Module: EvtBsMuMuKK.cc
//
// Description: Routine to implement Bs -> J/psi KK 
//
// Modification history:
// 
// Veronika Chobanova, Jeremy Dalseno, Diego Martinez Santos       
// April 21,2016       Module created   LHCb collaboration
//
//------------------------------------------------------------------------
#include <iomanip> 
#include "EvtGenBase/EvtGenKine.hh"      
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtPDL.hh" 
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtConst.hh"
#include "EvtGenBase/EvtVector3R.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensor3C.hh"
#include "EvtGenBase/EvtTensor4C.hh"
#include "EvtGenBase/EvtDiracSpinor.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtKine.hh"
#include "EvtGenBase/EvtdFunction.hh"

#include "EvtGenModels/EvtBsMuMuKK.hh"

const double pi = EvtConst::pi;
EvtComplex I = EvtComplex(0.,1.);
const double sq2 = sqrt(2.);

void EvtBsMuMuKK::init()
{
  // PDG masses
  MBs   = EvtPDL::getMass(EvtPDL::getId("B_s0"));
  MJpsi = EvtPDL::getMeanMass(EvtPDL::getId("J/psi"));
  Mf0   = 0.9499; //EvtPDL::getMeanMass(EvtPDL::getId("f_0"));
  Mphi  = EvtPDL::getMeanMass(EvtPDL::getId("phi"));
  MKp   = EvtPDL::getMass(EvtPDL::getId("K+"));
  MKm   = EvtPDL::getMass(EvtPDL::getId("K-"));
  MK0   = EvtPDL::getMass(EvtPDL::getId("K0"));
  Mpip  = EvtPDL::getMass(EvtPDL::getId("pi+"));
  Mpi0  = EvtPDL::getMass(EvtPDL::getId("pi0"));
  Mmu   = EvtPDL::getMass(EvtPDL::getId("mu+"));
  
  Gamma0phi =  EvtPDL::getWidth(EvtPDL::getId("phi"));
 
  kin_lower_limit  = 2.03*MKp;
  
  kin_upper_limit = 1.2; //1.2;ATTENTION CHANGE BACK TO 2.269 for the next round
  
  kin_middle = 0.5*(kin_upper_limit+kin_lower_limit);
    
  int_const_NR = sqrt(Int_const_sq(2*MKp, kin_upper_limit, 0, 1)); 
    
  int_Flatte_f0 = sqrt(Int_Flatte_sq( Mf0, 2*MKp, kin_upper_limit, 0, 1)); 

  p30Kp_mid_CMS = sqrt((pow(kin_middle,2) - pow(MKp+MKm,2)) * (pow(kin_middle,2) - pow(MKp-MKm,2)))/(2.*kin_middle);

  p30Kp_ll_CMS = sqrt((pow(kin_lower_limit,2) - pow(MKp+MKm,2)) * (pow(kin_lower_limit,2) - pow(MKp-MKm,2)))/(2.*kin_lower_limit);

  p30Kp_phi_CMS = sqrt((Mphi*Mphi - pow(MKp+MKm,2)) * (Mphi*Mphi - pow(MKp-MKm,2)))/(2.*Mphi);

  p30Jpsi_mid_CMS =  sqrt((MBs*MBs - pow(kin_middle+MJpsi,2)) * (MBs*MBs - pow(kin_middle-MJpsi,2)))/(2.*MBs);

  p30Jpsi_ll_CMS =  sqrt((MBs*MBs - pow(kin_lower_limit+MJpsi,2)) * (MBs*MBs - pow(kin_lower_limit-MJpsi,2)))/(2.*MBs);

  p30Jpsi_phi_CMS =  sqrt((MBs*MBs - pow(Mphi+MJpsi,2)) * (MBs*MBs - pow(Mphi-MJpsi,2)))/(2.*MBs);
  
  int_BW_phi = sqrt(Int_Breit_Wigner_sq( Gamma0phi, Mphi, 1, 0, p30Kp_phi_CMS, 2*MKp, kin_upper_limit ));
  
  //Number of arguments in the .dec file
  checkNArg(22);

  //4 daughters
  checkNDaug(4);

  //Spin-0 parent
  checkSpinParent(EvtSpinType::SCALAR);

  //Daughters
  checkSpinDaughter(0,EvtSpinType::DIRAC);
  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::SCALAR); 
  checkSpinDaughter(3,EvtSpinType::SCALAR);
  
  //B_s0 parent
  const EvtId p = getParentId(); 
    
  if( p != EvtPDL::getId("B_s0") && p != EvtPDL::getId("anti-B_s0") ) {
      report(ERROR,"EvtGen") << "EvtBsMuMuKK: Parent must be B_s0 or anti-B_s0" << std::endl;
      assert(0);
  }

  //Daughter types and ordering
  const EvtId d1 = getDaug(0);
  const EvtId d2 = getDaug(1); 
  const EvtId d3 = getDaug(2);
  const EvtId d4 = getDaug(3);
  
  if(!((d1 == EvtPDL::getId("mu+") ||  d1 == EvtPDL::getId("mu-")) &&        
       (d2 == EvtPDL::getId("mu-") ||  d2 == EvtPDL::getId("mu+")) &&        
       (d3 == EvtPDL::getId("K+") || d3 == EvtPDL::getId("K-")) && 
       (d4 == EvtPDL::getId("K-") || d4 == EvtPDL::getId("K+")))) {
      report(ERROR,"EvtGen") << "EvtBsMuMuKK: Daughter sequence should be mu+-, mu-+, K+-, K-+" << std::endl;
      assert(0);
  }
}

void EvtBsMuMuKK::initProbMax() 
{
    // TODO Find a more elaborate way to set this
   const double f_phi = 1. - getArg(0) - getArg(4);
   
   const EvtComplex hm =  sqrt(p30Jpsi_phi_CMS*p30Kp_phi_CMS)*(1.-getArg(4)-getArg(0))*X_J(1,p30Kp_phi_CMS,0)*p30Kp_phi_CMS
                          *(0.6*sqrt(getArg(8)*f_phi) + 0.6*sqrt(getArg(12)*f_phi) + 0.6*sqrt((1.-getArg(8)-getArg(12))*f_phi))
                          *Breit_Wigner( Gamma0phi, Mphi, Mphi, 1, p30Kp_phi_CMS, p30Kp_phi_CMS )/int_BW_phi
                          +sqrt(p30Jpsi_ll_CMS*p30Kp_ll_CMS)*X_J(1,p30Jpsi_ll_CMS,1)*p30Jpsi_ll_CMS
                          *0.8*getArg(4)*Flatte(Mf0, kin_lower_limit)/int_Flatte_f0
                          + sqrt(p30Jpsi_mid_CMS*p30Kp_mid_CMS)*X_J(1,p30Jpsi_mid_CMS,1)*p30Jpsi_mid_CMS
                          *1.2*getArg(0)/int_const_NR ;
                          
   report(INFO,"EvtGen") << "kin_upper_limit " << kin_upper_limit << std::endl;
   report(INFO,"EvtGen") << "height_max " <<  abs2(hm) << std::endl; 
   setProbMax(abs2(hm));    
}

void EvtBsMuMuKK::decay( EvtParticle *p )  
{ 
    // Amplitudes
    // --- Non-resonant S wave
    const double f_S_NR                 = getArg(0);
    const double A_S_NR                 = sqrt(f_S_NR);
    // --- f0 
    const double f_f0                   = getArg(4); 
    const double A_f0                   = sqrt(f_f0);
    // --- phi
    const double f_phi                  = 1. - f_S_NR - f_f0;
    const double A_phi_0                = sqrt(getArg(8)*f_phi);
    const double A_phi_perp             = sqrt(getArg(12)*f_phi);
    const double A_phi_par              = sqrt(f_phi-A_phi_perp*A_phi_perp-A_phi_0*A_phi_0);
    
    // Strong phases
    const double delta_S_NR             = getArg(1);
    const double delta_f0               = getArg(5);
    const double delta_phi_0            = getArg(9);
    const double delta_phi_perp         = getArg(13);
    const double delta_phi_par          = getArg(16);
    
    // phiS
    // --- phi
    const double phis_phi_0             = getArg(10);
    const double phis_phi_perp          = getArg(14);
    const double phis_phi_par           = getArg(17);
    // --- Non-resonant S wave
    const double phis_S_NR              = getArg(2);
    // --- f0 
    const double phis_f0                = getArg(6);
    
    // |lambda|
    // --- Non-resonant S wave
    const double lambda_S_NR_abs        = getArg(3);
    // --- f0 
    const double lambda_f0_abs          = getArg(7);
    // --- phi
    const double lambda_phi_0_abs       = getArg(11);
    const double lambda_phi_perp_abs    = getArg(15);
    const double lambda_phi_par_abs     = getArg(18);

    //Time dependence  
    static double Gamma      = getArg(19);
    static double deltaGamma = getArg(20);
    static double ctau       = 1./Gamma;
    
    static double deltaMs = getArg(21);
    
    //This overrules the lifetimes made in OtherB
    const double time = -log(EvtRandom::Flat())*(ctau);//ctau has same dimensions as t
    
    if(p->getParent())
        p->getParent()->setLifetime(time);
    else
        p->setLifetime(time);

    double mt = exp(-0.25*deltaGamma*time);
    double pt = exp(+0.25*deltaGamma*time);

    EvtComplex gplus  = 0.5*( mt*EvtComplex(cos(0.5*deltaMs*time),sin( 0.5*deltaMs*time))
                            + pt*EvtComplex(cos(0.5*deltaMs*time),sin(-0.5*deltaMs*time)));
    EvtComplex gminus = 0.5*( mt*EvtComplex(cos(0.5*deltaMs*time),sin( 0.5*deltaMs*time))
                            - pt*EvtComplex(cos(0.5*deltaMs*time),sin(-0.5*deltaMs*time)));

    const int q = gen_q();
    p->setAttribute("q",q);
    
    EvtComplex a_S_NR       = Amp_time_q(q, gplus, gminus, delta_S_NR,     lambda_S_NR_abs,     A_S_NR,     phis_S_NR,     -1 );
    EvtComplex a_f0         = Amp_time_q(q, gplus, gminus, delta_f0,       lambda_f0_abs,       A_f0,       phis_f0,       -1 );
    EvtComplex a0_phi       = Amp_time_q(q, gplus, gminus, delta_phi_0,    lambda_phi_0_abs,    A_phi_0,    phis_phi_0,    +1 );
    EvtComplex aperp_phi    = Amp_time_q(q, gplus, gminus, delta_phi_perp, lambda_phi_perp_abs, A_phi_perp, phis_phi_perp, -1 );
    EvtComplex apar_phi     = Amp_time_q(q, gplus, gminus, delta_phi_par,  lambda_phi_par_abs,  A_phi_par,  phis_phi_par,  +1 );
     
    // Generate 4-momenta
    double mKK = EvtRandom::Flat(2.*MKp,kin_upper_limit);
    double mass[10]   = {MJpsi, mKK, 0., 0., 0., 0., 0., 0., 0., 0.};
    double Kmass[10]  = {MKp,   MKm, 0., 0., 0., 0., 0., 0., 0., 0.}; 
    double muMass[10] = {Mmu,   Mmu, 0., 0., 0., 0., 0., 0., 0., 0.}; 
          
    EvtVector4R mypV[2], mypK[2], mypmu[2];
    EvtGenKine::PhaseSpace(2,mass,mypV,MBs);
    EvtGenKine::PhaseSpace(2,Kmass,mypK,mKK); 
    EvtGenKine::PhaseSpace(2,muMass,mypmu,MJpsi);
   
    EvtVector4R p4mup = boostTo(mypmu[0], mypV[0]);
    EvtVector4R p4mum = boostTo(mypmu[1], mypV[0]);
    EvtVector4R p4Kp  = boostTo(mypK[0], mypV[1]);
    EvtVector4R p4Km  = boostTo(mypK[1], mypV[1]);
    
    p->makeDaughters(getNDaug(),getDaugs());
    
    EvtParticle *muplus, *muminus, *Kplus, *Kminus;
    
    muplus  = p->getDaug(0);
    muminus = p->getDaug(1);
    Kplus   = p->getDaug(2);
    Kminus  = p->getDaug(3);

    muplus->init(getDaug(0), p4mup);
    muminus->init(getDaug(1), p4mum);
    Kplus->init(getDaug(2), p4Kp);
    Kminus->init(getDaug(3), p4Km);

    EvtVector4R p4KK   = p4Kp + p4Km;
    EvtVector4R p4mumu = p4mup + p4mum;
    EvtVector4R p4Bs   = p4mup + p4mum + p4KK;
    
    // Kp momentum in the KK CMS
    const double p3Kp_KK_CMS  = sqrt((p4KK.mass2() - pow(MKp+MKm,2)) * (p4KK.mass2()-  pow(MKp-MKm,2)))/(2.*p4KK.mass());
    
    // J/psi momentum in the KK CMS
    const double p3Jpsi_KK_CMS = sqrt((p4Bs.mass2()- pow(p4KK.mass()+MJpsi,2)) * (p4Bs.mass2()-pow(p4KK.mass()-MJpsi,2)))/(2.*p4Bs.mass());

    // Mass lineshape
    // Non-resonant S wave
    EvtComplex P_NR = 1./int_const_NR; 
    
    // f0 Flatte
    EvtComplex F_f0 = Flatte(Mf0, p4KK.mass())/int_Flatte_f0;
    
    // phi Breit Wigner
    EvtComplex BW_phi = Breit_Wigner( Gamma0phi, Mphi, p4KK.mass(), 1, p30Kp_phi_CMS, p3Kp_KK_CMS )/int_BW_phi;
        
    // Barrier factors: Always taking the lowest Bs L
    const double X_KK_0 = 1.;
    const double X_KK_1  = X_J(1,p3Kp_KK_CMS,0);
    const double X_NR_Jpsi_1 = X_J(1,p3Jpsi_KK_CMS,1);
    const double X_f0_Jpsi_1 = X_J(1,p3Jpsi_KK_CMS,1);
    const double X_phi_Jpsi_0 = 1.;
    
    // Birth momentum factors: pow(p3(K+),LR)* pow(p3(J/psi),LB) 
    const double f_PHSP = sqrt(p3Jpsi_KK_CMS*p3Kp_KK_CMS);
    const double f_BMF_NR  = 1.*p3Jpsi_KK_CMS;
    const double f_BMF_f0  = 1.*p3Jpsi_KK_CMS;
    const double f_BMF_phi = p3Kp_KK_CMS*1.;
    
    // TODO Angular distribution and sum over KK states
    double costheta_K, costheta_mu, chi;
    
    costheta_K  = EvtDecayAngle(p4Bs,p4KK,p4Kp);
    costheta_mu = EvtDecayAngle(p4Bs,p4mumu,p4mup); 
    chi         = EvtDecayAngleChi(p4Bs,p4mup,p4mum,p4Kp,p4Km);

    // Build helicity amplitudes
    const EvtComplex H0_phi = a0_phi ;
    const EvtComplex Hp_phi = (apar_phi + aperp_phi)/sq2;
    const EvtComplex Hm_phi = (apar_phi - aperp_phi)/sq2;
    
    // muon polarization +1
    // KK Spin-0 NR
    const EvtComplex mp_hS_NR  = a_S_NR*angular_distribution(0,0,1,costheta_K,costheta_mu,chi);
    
    // KK Spin-0 f0
    const EvtComplex mp_h_f0   = a_f0*angular_distribution(0,0,1,costheta_K,costheta_mu,chi);

    // KK Spin-1
    const EvtComplex mp_h0_phi = H0_phi*angular_distribution(1,0,1, costheta_K,costheta_mu,chi);
    const EvtComplex mp_hp_phi = Hp_phi*angular_distribution(1,1,1, costheta_K,costheta_mu,chi);
    const EvtComplex mp_hm_phi = Hm_phi*angular_distribution(1,-1,1,costheta_K,costheta_mu,chi);
    
    // Sum over KK states:
    const EvtComplex Amp_tot_plus = f_PHSP*(P_NR*X_KK_0*X_NR_Jpsi_1*f_BMF_NR*mp_hS_NR +
					    F_f0*X_KK_0*X_f0_Jpsi_1*f_BMF_f0*mp_h_f0 + 
					    BW_phi*X_KK_1*X_phi_Jpsi_0*f_BMF_phi*(mp_h0_phi + mp_hp_phi + mp_hm_phi));

    // muon polarization -1
    // KK Spin-0 NR
    const EvtComplex mm_hS_NR  = a_S_NR*angular_distribution(0,0,-1,costheta_K,costheta_mu,chi);
    
    // KK Spin-0
    const EvtComplex mm_h_f0   = a_f0*angular_distribution(0,0,-1,costheta_K,costheta_mu,chi); 

    // KK Spin-1
    const EvtComplex mm_h0_phi = H0_phi*angular_distribution(1,0,-1, costheta_K,costheta_mu,chi);
    const EvtComplex mm_hp_phi = Hp_phi*angular_distribution(1,+1,-1,costheta_K,costheta_mu,chi);
    const EvtComplex mm_hm_phi = Hm_phi*angular_distribution(1,-1,-1,costheta_K,costheta_mu,chi);
    
    const EvtComplex Amp_tot_minus = f_PHSP*(P_NR*X_KK_0*X_NR_Jpsi_1*f_BMF_NR*mm_hS_NR + 
                                             F_f0*X_KK_0*X_f0_Jpsi_1*f_BMF_f0*mm_h_f0 + 
                                             BW_phi*X_KK_1*X_phi_Jpsi_0*f_BMF_phi*(mm_h0_phi + mm_hp_phi + mm_hm_phi));
    
    vertex(0, 0, 0);
    vertex(0, 1, Amp_tot_plus);
    vertex(1, 0, Amp_tot_minus);
    vertex(1, 1, 0.);
    
}

EvtComplex EvtBsMuMuKK::get_rho(const double m0, 
                                const double m) 
{    
    const double rho_sq = 1.-4.*m0*m0/(m*m);
    EvtComplex rho;
   
    if(rho_sq > 0. ) {
        rho = EvtComplex(sqrt(rho_sq),0.);
    } else {
        rho = EvtComplex(0.,sqrt(-rho_sq));
    }

    return rho;
}
 
// Integral const over KK and J/psi phasespace, including PHSP factor, Bs birth momentum and KK decay momentum
double EvtBsMuMuKK::Int_const_sq(  const double M_KK_ll, 
                                   const double M_KK_ul,
                                   const int JR, 
                                   const int JB  )
{
    int bins = 1000;
    
    double bin_width   = (M_KK_ul-M_KK_ll)/static_cast<double>(bins);
    
    EvtComplex const_sq_int = 0;
    
    for(int i = 0; i < bins; i++) {
        const double M_KK_i = M_KK_ll + static_cast<double>(i)*bin_width;
        const double M_KK_f = M_KK_ll + static_cast<double>(i+1)*bin_width;
        
        const double p3Kp_KK_CMS_i = sqrt((pow(M_KK_i,2) - pow(MKp+MKm,2)) * (pow(M_KK_i,2) - pow(MKp-MKm,2)))/(2.*M_KK_i);
        const double p3Kp_KK_CMS_f = sqrt((pow(M_KK_f,2) - pow(MKp+MKm,2)) * (pow(M_KK_f,2) - pow(MKp-MKm,2)))/(2.*M_KK_f);
            
        const double p3Jpsi_Bs_CMS_i = sqrt((pow(MBs,2) - pow(M_KK_i+MJpsi,2)) * (pow(MBs,2) - pow(M_KK_i-MJpsi,2)))/(2.*MBs);
        const double p3Jpsi_Bs_CMS_f = sqrt((pow(MBs,2) - pow(M_KK_f+MJpsi,2)) * (pow(MBs,2) - pow(M_KK_f-MJpsi,2)))/(2.*MBs);
        
        const double f_PHSP_i = sqrt(p3Kp_KK_CMS_i*p3Jpsi_Bs_CMS_i);
        const double f_PHSP_f = sqrt(p3Kp_KK_CMS_f*p3Jpsi_Bs_CMS_f);
        
        const double f_MBF_KK_i = pow(p3Kp_KK_CMS_i,JR);
        const double f_MBF_KK_f = pow(p3Kp_KK_CMS_f,JR);
        
        const double f_MBF_Bs_i = pow(p3Jpsi_Bs_CMS_i,JB);
        const double f_MBF_Bs_f = pow(p3Jpsi_Bs_CMS_f,JB);
        
        const double X_JR_i = X_J(JR,p3Kp_KK_CMS_i,0);
        const double X_JR_f = X_J(JR,p3Kp_KK_CMS_f,0);
        
        const double X_JB_i = X_J(JB,p3Jpsi_Bs_CMS_i,1);
        const double X_JB_f = X_J(JB,p3Jpsi_Bs_CMS_f,1);
        
        const EvtComplex a_i = f_PHSP_i*f_MBF_KK_i*f_MBF_Bs_i*X_JR_i*X_JB_i;
        const EvtComplex a_st_i = conj(a_i);
        const EvtComplex a_f = f_PHSP_f*f_MBF_KK_f*f_MBF_Bs_f*X_JR_f*X_JB_f; 
        const EvtComplex a_st_f = conj(a_f);
                                        
        const_sq_int += 0.5*bin_width*(a_i*a_st_i+a_f*a_st_f);
    }
    
    return real(const_sq_int);
}
 
EvtComplex EvtBsMuMuKK::Flatte( const double m0, 
                                const double m )
{
    const double gpipi = 0.167; // GeV units
    const double gKK = 3.05*gpipi;

    EvtComplex Flatte_0 = 1./(m0*m0 - m*m - I*m0*
                              (gpipi*((2./3.)*get_rho(Mpip, m)+ (1./3.)*get_rho(Mpi0,m)) 
                               + gKK*((1./2.)*get_rho(MKp, m) + (1./2.)*get_rho(MK0,m))));
    return Flatte_0;
}

// Integral Flatte over KK and J/psi phasespace, including PHSP factor, Bs birth momentum and KK decay momentum
double EvtBsMuMuKK::Int_Flatte_sq( const double m0, 
                                   const double M_KK_ll, 
                                   const double M_KK_ul,
                                   const int JR, 
                                   const int JB  )
{
    int bins = 1000;
    
    double bin_width = (M_KK_ul-M_KK_ll)/static_cast<double>(bins);
    
    EvtComplex flatte_sq_int = 0;
    
    for (int i = 0; i < bins; i++) {
        const double M_KK_i = M_KK_ll + static_cast<double>(i)*bin_width;
        const double M_KK_f = M_KK_ll + static_cast<double>(i+1)*bin_width;
        
        const double p3Kp_KK_CMS_i = sqrt((pow(M_KK_i,2) - pow(MKp+MKm,2)) * (pow(M_KK_i,2) - pow(MKp-MKm,2)))/(2.*M_KK_i);
        const double p3Kp_KK_CMS_f = sqrt((pow(M_KK_f,2) - pow(MKp+MKm,2)) * (pow(M_KK_f,2) - pow(MKp-MKm,2)))/(2.*M_KK_f);
            
        const double p3Jpsi_Bs_CMS_i = sqrt((pow(MBs,2) - pow(M_KK_i+MJpsi,2)) * (pow(MBs,2) - pow(M_KK_i-MJpsi,2)))/(2.*MBs);
        const double p3Jpsi_Bs_CMS_f = sqrt((pow(MBs,2) - pow(M_KK_f+MJpsi,2)) * (pow(MBs,2) - pow(M_KK_f-MJpsi,2)))/(2.*MBs);
        
        const double f_PHSP_i = sqrt(p3Kp_KK_CMS_i*p3Jpsi_Bs_CMS_i);
        const double f_PHSP_f = sqrt(p3Kp_KK_CMS_f*p3Jpsi_Bs_CMS_f);
        
        const double f_MBF_KK_i = pow(p3Kp_KK_CMS_i,JR);
        const double f_MBF_KK_f = pow(p3Kp_KK_CMS_f,JR);
        
        const double f_MBF_Bs_i = pow(p3Jpsi_Bs_CMS_i,JB);
        const double f_MBF_Bs_f = pow(p3Jpsi_Bs_CMS_f,JB);
        
        const double X_JR_i = X_J(JR,p3Kp_KK_CMS_i,0);
        const double X_JR_f = X_J(JR,p3Kp_KK_CMS_f,0);
        
        const double X_JB_i = X_J(JB,p3Jpsi_Bs_CMS_i,1);
        const double X_JB_f = X_J(JB,p3Jpsi_Bs_CMS_f,1);
        
        const EvtComplex Flatte_i = Flatte(m0, M_KK_i);
        const EvtComplex Flatte_f = Flatte(m0, M_KK_f);
        
        const EvtComplex a_i = f_PHSP_i*f_MBF_KK_i*f_MBF_Bs_i*X_JR_i*X_JB_i*Flatte_i;
        const EvtComplex a_st_i = conj(a_i);
        const EvtComplex a_f = f_PHSP_f*f_MBF_KK_f*f_MBF_Bs_f*X_JR_f*X_JB_f*Flatte_f;
        const EvtComplex a_st_f = conj(a_f);
                                        
        flatte_sq_int += 0.5*bin_width*(a_i*a_st_i+a_f*a_st_f);
    }
    
    return real(flatte_sq_int);
}

EvtComplex EvtBsMuMuKK::Breit_Wigner( const double Gamma0, 
                                      const double m0, 
                                      const double m, 
                                      const int J, 
                                      const double q0, 
                                      const double q ) 
{
    double X_J_q0_sq = pow(X_J(J, q0, 0), 2);
    double X_J_q_sq = pow(X_J(J, q, 0), 2);
    
    double Gamma = Gamma0*pow(q/q0,2*J+1)*(m0/m)*(X_J_q_sq/X_J_q0_sq);        
    
    return 1./(m0*m0-m*m - I*m0*Gamma);
}

// Integral Breit-Wigner over KK and J/psi phasespace, including PHSP factor, Bs birth momentum and KK decay momentum
double EvtBsMuMuKK::Int_Breit_Wigner_sq( const double Gamma0, 
                                         const double m0,
                                         const int JR, 
                                         const int JB, 
                                         const double q0, 
                                         const double M_KK_ll, 
                                         const double M_KK_ul ) 
{
    int bins = 1000;
    double bin_width = (M_KK_ul-M_KK_ll)/static_cast<double>(bins);
    EvtComplex bw_int = 0.; 
    
    for (int i = 0; i < bins; i++) {
        const double M_KK_i = M_KK_ll + static_cast<double>(i)*bin_width;
        const double M_KK_f = M_KK_ll + static_cast<double>(i+1)*bin_width;
        
        const double p3Kp_KK_CMS_i = sqrt((pow(M_KK_i,2) - pow(MKp+MKm,2)) * (pow(M_KK_i,2) - pow(MKp-MKm,2)))/(2.*M_KK_i);
        const double p3Kp_KK_CMS_f = sqrt((pow(M_KK_f,2) - pow(MKp+MKm,2)) * (pow(M_KK_f,2) - pow(MKp-MKm,2)))/(2.*M_KK_f);
            
        const double p3Jpsi_Bs_CMS_i = sqrt((pow(MBs,2) - pow(M_KK_i+MJpsi,2)) * (pow(MBs,2) - pow(M_KK_i-MJpsi,2)))/(2.*MBs);
        const double p3Jpsi_Bs_CMS_f = sqrt((pow(MBs,2) - pow(M_KK_f+MJpsi,2)) * (pow(MBs,2) - pow(M_KK_f-MJpsi,2)))/(2.*MBs);
        
        const double f_PHSP_i = sqrt(p3Kp_KK_CMS_i*p3Jpsi_Bs_CMS_i);
        const double f_PHSP_f = sqrt(p3Kp_KK_CMS_f*p3Jpsi_Bs_CMS_f);
        
        const double f_MBF_KK_i = pow(p3Kp_KK_CMS_i,JR);
        const double f_MBF_KK_f = pow(p3Kp_KK_CMS_f,JR);
        
        const double f_MBF_Bs_i = pow(p3Jpsi_Bs_CMS_i,JB);
        const double f_MBF_Bs_f = pow(p3Jpsi_Bs_CMS_f,JB);
        
        const double X_JR_i = X_J(JR,p3Kp_KK_CMS_i,0);
        const double X_JR_f = X_J(JR,p3Kp_KK_CMS_f,0);
        
        const double X_JB_i = X_J(JB,p3Jpsi_Bs_CMS_i,1);
        const double X_JB_f = X_J(JB,p3Jpsi_Bs_CMS_f,1);
        
        const EvtComplex a_i = f_PHSP_i*f_MBF_KK_i*f_MBF_Bs_i*X_JR_i*X_JB_i*Breit_Wigner(Gamma0, m0, M_KK_i, JR, q0, p3Kp_KK_CMS_i);
        const EvtComplex a_st_i = conj(a_i);
        const EvtComplex a_f = f_PHSP_f*f_MBF_KK_f*f_MBF_Bs_f*X_JR_f*X_JB_f*Breit_Wigner(Gamma0, m0, M_KK_f, JR, q0, p3Kp_KK_CMS_f); 
        const EvtComplex a_st_f = conj(a_f);
                    
        bw_int += 0.5*bin_width*(a_i*a_st_i+a_f*a_st_f);
    }
    return real(bw_int);
}

// Blatt-Weisskopf barrier factors (using GeV)
double EvtBsMuMuKK::X_J(const int J, 
                        const double q,
                        const int isB
                       )
{
    double r_BW = 1.;
    
    if (isB==0) {
        r_BW = 1.5;
    } else if (isB==1) {
        r_BW = 5.;
    }
       
    double zsq = pow(r_BW*q,2);

    double X_J(1.0);

    if (J==1) {
        X_J = sqrt(1./(1.+zsq));
    } else if(J==2) {
        X_J = sqrt(1./(zsq*zsq+3.*zsq+9.));
    }

    return X_J;
}

// EvtGen d matrix: Input is 2J instead of J etc
double EvtBsMuMuKK::wigner_d(int J, 
                             int l, 
                             int alpha, 
                             double theta) 
{
  return EvtdFunction::d(2*J, 2*l, 2*alpha, theta);  
}

// J spin of KK, l spin proj of J/psi, alpha dimuon spin
EvtComplex EvtBsMuMuKK::angular_distribution(int J, 
                                             int l, 
                                             int alpha, 
                                             double cK, 
                                             double cL, 
                                             double chi)
{
  double thetaL = acos(cL);
  double thetaK = acos(cK);
  
  EvtComplex out = 0.5*sqrt((2*J+1)/pi)*exp(EvtComplex(0,-l*chi));

  out = out * wigner_d(1, l, alpha, thetaL)*wigner_d(J, -l, 0, thetaK);
  
  return out;
}

// Time-dependent amplitude calculation
EvtComplex EvtBsMuMuKK::Amp_time_q(int q, 
                                   EvtComplex gplus, 
                                   EvtComplex gminus, 
                                   double delta,
                                   double lambda_abs,
                                   double Amp,
                                   double phis,
                                   int eta)
{
  EvtComplex amp_time  = Amp * EvtComplex(cos(-delta),sin(-delta));
  if (q == -1) {
      amp_time = amp_time * (gplus + eta*pow(lambda_abs,-1.*q)*EvtComplex(cos(q*phis),sin(q*phis))*gminus);
  } else {
      amp_time = amp_time * eta * (gplus + eta*pow(lambda_abs,-1.*q)*EvtComplex(cos(q*phis),sin(q*phis))*gminus);
  }
  return amp_time;
}

// Flavour generation
int EvtBsMuMuKK::gen_q()
{
  int flavour(1);
  if( EvtRandom::Flat() < 0.5 ) {flavour = -1;}
  return flavour;
}
