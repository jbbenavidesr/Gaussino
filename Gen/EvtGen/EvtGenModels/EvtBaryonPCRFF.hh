//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtBaryonVminusAFF.hh
//
// Description:Form factor routines specific to EvtBaryonVminusA
//
// Modification history:
//
//    R.J. Tesarek     May 28, 2004     Module created
//    Karen Gibson     1/20/2006        Module updated for 1/2+->1/2+
//    Zhou Xing        Nov 2010         1/2+->1/2-, 1/2+->3/2- Lambda decays
//    
//------------------------------------------------------------------------

#ifndef EVTBARYONPCRFF_HH
#define EVTBARYONPCRFF_HH

#include "EvtGenBase/EvtSemiLeptonicFF.hh"
 
class EvtId;

class EvtBaryonPCRFF : public EvtSemiLeptonicFF {
  
public:

  void getscalarff( EvtId, EvtId,
		    double, double, double *,
		    double *);
  void getvectorff( EvtId, EvtId, 
		    double, double, double *, 
		    double *, double *, double *);
  void gettensorff( EvtId, EvtId, 
		    double, double, double *,
		    double *, double *, double *);

  void getbaryonff(EvtId, EvtId, 
		   double, double, double*, 
		   double*, double*, double*);

  void getdiracff( EvtId parent, EvtId daught,
                   double q2, double mass, 
                   double *f1, double *f2, double *f3,
                   double *g1, double *g2, double *g3 );
  
  void getraritaff( EvtId parent, EvtId daught,
                    double q2, double mass, 
                    double *f1, double *f2, double *f3, double *f4,
                    double *g1, double *g2, double *g3, double *g4 );
};

#endif
 
