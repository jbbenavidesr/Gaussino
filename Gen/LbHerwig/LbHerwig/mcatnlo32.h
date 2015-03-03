// $Id: mcatnlo32.h,v 1.1 2006-09-05 12:36:47 karl Exp $
#ifndef __MCATNLO32_HH__
#define __MCATNLO32_HH__ 1

// Declarations for common blocks of MC@NLO 3.2

typedef int logical;

// Common block for MC@NLO eta cut

struct Cetacut_t
{
    double  etacut;
};

extern Cetacut_t* gCetacut;

// Common block for MC@NLO weight parameter

struct Ciwgtnorm_t
{
    int     iwgtnorm;
};

extern Ciwgtnorm_t* gCiwgtnorm;

// Common block for MC@NLO QCD-related parameters
struct Fixvar_t
{
    double  xm2, xlam, zg, ze2;
};

extern Fixvar_t* gFixvar;

// Common block for MC@NLO parameters

struct Mcnlopar_t
{
    double  ac1, ac2, ecm, ecmlst[100], fh1lst[100], fh1mclst[100],
            fh2lst[100],fh2mclst[100], renlst[100], renmclst[100],
            xfh, xfhmc, xm, xmlst[100], xren, xrenmc;
    int     ibswrite, idpdfset, iproc, iseed, iseed0, iseld, it1, it2,
            itmpvv, iverbose, jecm, maxevt, maxtrials, ncl3;
    logical evgen;
    char    gname[20], part1[4], part2[4], pref[80], prefev[80], scheme[2];
};

extern Mcnlopar_t* gMcnlopar;

// Common block for MC@NLO particle masses

struct Parmass_t
{
    double  xmass[27];
};

extern Parmass_t* gParmass;

// Common block for MC@NLO events file
struct Vvjin_t
{
    char    qqin[50];
};
extern Vvjin_t* gVvjin;

// Declare pointers for accessing the common blocks

Cetacut_t* gCetacut;
Ciwgtnorm_t* gCiwgtnorm;
Fixvar_t* gFixvar;
Mcnlopar_t* gMcnlopar;
Parmass_t* gParmass;
Vvjin_t* gVvjin;

#endif
