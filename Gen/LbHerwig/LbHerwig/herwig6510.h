// $Id: herwig6510.h,v 1.2 2006-09-05 12:36:47 karl Exp $
#ifndef __HERWIG65_HH__
#define __HERWIG65_HH__ 1

// Declarations for common blocks of Herwig 6.510

#include <complex>

typedef int logical;

const int m_nmxhep = 10000;
const int m_nmxpar = 500;
const int m_modmax = 50;
const int m_nmxres = 500;
const int m_nmxdks = m_nmxhep;
const int m_nmxmod = 200;
const int m_nmxcdk = m_nmxhep;
const int m_nmxqdk = 20;
const int m_nmxsud = 1024;
//const int m_nmxjet = 200;
const int m_nmxcl = 500;
const int m_imaxch = 20;
// const int m_imaxop = 40;
const int m_nxmrs = 49;
const int m_nqmrs = 37;
const int m_npmrs = 8;
const int m_nmxspn = 50;
const int m_ncfmax = 3;
const int m_nmode2 = 500;
const int m_nmode3 = 500;
const int m_ndiagr = 8;
const int m_nmodeb = 50;
const int m_nmode4 = 4;
const int m_maxhrp = 100;

// Beams, process and number of events

struct Hwbeam_t
{
    int     ipart1, ipart2;
};
extern Hwbeam_t* gHwbeam;

struct Hwbmch_t
{
    char    part1[8], part2[8];
};
extern Hwbmch_t* gHwbmch;

struct Hwproc_t
{
    double  ebeam1, ebeam2, pbeam1, pbeam2;
    int     iproc, maxev;
};
extern Hwproc_t* gHwproc;

// Basic parameters (and quantities derived from them)

struct Hwpram_t
{
    double  afch[16*2], alphem, b1lim, betaf, btclm, cafac, cffac, clmax,
            clpow, clsmr[2], cspeed, ensof, etamix, f0mix, f1mix, f2mix, gamh,
            gamw, gamz, gamzp, gev2nb, h1mix, pdiqk, pgsmx, pgspl[4], phimix,
            pifac, prsof, psplt[2], ptrms, pxrms, qcdl3, qcdl5, qcdlam, qdiqk,
            qfch[16], qg, qspac, qv, scabi, swein, tmtop, vfch[16*2], vckm[3*3],
            vgcut, vqcut, vpcut, zbinm, effmin, omhmix, et2mix, ph3mix, gcutme; 
    int     ioprem, iprint, ispac, lrsud, lwsud, modpdf[2], nbtry, ncolo, 
            nctry, ndtry, netry, nflav, ngspl, nstru, nstry, nzbin,
            iop4jt[2], nprfmt;
    logical azsoft, azspin;
    int     cldir[2];
    logical hardme, nospac, prndec, prvtx, softme, zprime,
            prndef, prntex, prnweb;
};
extern Hwpram_t* gHwpram;

struct Hwprch_t
{
    char    autpdf[2][20], bdecay[4];
};
extern Hwprch_t* gHwprch;

// Parton shower common (same format as /HEPEVT/)
struct Hwpart_t
{
    int     nevpar, npar, istpar[m_nmxpar], idpar[m_nmxpar], jmopar[2*m_nmxpar],
	    jdapar[2*m_nmxpar];
    double  ppar[5*m_nmxpar], vpar[4*m_nmxpar];
};
extern Hwpart_t* gHwpart;

// Parton polarization common
struct Hwparp_t
{
    double  decpar[2*m_nmxpar], phipar[2*m_nmxpar], rhopar[2*m_nmxpar];
    int     tmpar[m_nmxpar];
};
extern Hwparp_t* gHwparp;

// Electroweak boson common
struct Hwbosc_t
{
    double  alpfac, brhig[12], enhanc[12], gammax, rhohep[3*m_nmxhep];
    int     iophig, modbos[m_modmax];
};
extern Hwbosc_t* gHwbosc;

// Parton colour common
struct Hwparc_t
{
    int     jcopar[4*m_nmxpar];
};
extern Hwparc_t* gHwparc ;

// Other Herwig branching, event and hard subprocess common blocks
struct Hwbrch_t
{
    double  anomsc[2*2], hardst, ptint[3*2], xfact;
    int     inhad, jnhad, nspac[7], islent;
    logical breit, frost, usecmf;
};
extern Hwbrch_t* gHwbrch;

struct Hwevnt_t
{
    double  avwgt, evwgt, gamwt, tlout, wbigst, wgtmax, wgtsum, wsqsum;
    int     idhw[m_nmxhep], ierror, istat, lwevt, maxer, maxpr;
    logical nowgt;
    int     nrn[2], numer, numeru, nwgts;
    logical gensof;
};
extern Hwevnt_t* gHwevnt;

struct Hwhard_t
{
    double  asfixd, clq[7*6], coss, costh, ctmax, disf[13*2],
            emlst, emmax, emmin, empow, emsca, epoln[3],
            gcoef[7], gpoln, omega0, phomas, ppoln[3], ptmax, ptmin, ptpow, 
            q2max, q2min, q2pow, q2wwmn, q2wwmx, qlim, sins, thmax, y4jt,
            tmnisr, tqwt, xx[2], xlmin, xxmin, ybmax, ybmin, yjmax, yjmin,
            ywwmax, ywwmin, whmin, zjmax, zmxisr;
    int     iaphig, ibrn[2], ibsh, ico[10], idcmf, idn[10], iflmax, iflmin, 
	    ihpro, ipro, mapq[6], maxfl;
    logical bgshat, colisr, fstevt, fstwgt, genev, hvfcen, tpol,
            durham;
};
extern Hwhard_t* gHwhard;

// Arrays for particle properties
struct Hwprop_t
{
    double  rltim[1+m_nmxres], rmass[1+m_nmxres], rspin[1+m_nmxres];
    int     ichrg[1+m_nmxres], idpdg[1+m_nmxres], iflav[1+m_nmxres], nres;
    logical vtocdk[1+m_nmxres], vtordk[1+m_nmxres],
            qorqqb[1+m_nmxres], qborqq[1+m_nmxres];
};
extern Hwprop_t* gHwprop;

struct Hwunam_t
{
    char    rname[1+m_nmxres][8];
    char    txname[2*(1+m_nmxres)][37];
};
extern Hwunam_t* gHwunam;

// Arrays for particle decays
struct Hwupdt_t
{
    double  brfrac[m_nmxdks], cmmom[m_nmxdks], dkltm[m_nmxres];
    int     idk[m_nmxdks], idkprd[5*m_nmxdks], lnext[m_nmxdks], 
	    lstrt[m_nmxres], ndkys, nme[m_nmxdks], nmodes[m_nmxres],
            nprods[m_nmxdks];
    logical dkpset, rstab[1+m_nmxres];
};
extern Hwupdt_t* gHwupdt;

// Weights used in cluster decays
struct Hwuwts_t
{
    double  repwt[4*5*5], sngwt, decwt, qwt[3], pwt[12], swtef[m_nmxres];
};
extern Hwuwts_t* gHwuwts;

// Parameters for cluster decays
struct Hwuclu_t
{
    double  cldkwt[m_nmxcdk], cthrpw[12*12], preco, resn[12*12], rmin[12*12];
    int     locn[12*12], ncldk[m_nmxcdk], nreco;
    logical clreco;
};
extern Hwuclu_t* gHwuclu;

// Variables controlling mixing and vertex information
struct Hwdist_t
{
    double  exag, gev2mm, hbar, pltcut, vmin2, vtxpip[5], xmix[2], xmrct[2],
            ymix[2], ymrct[2];
    int     iopdkl;
    logical maxdkl, mixing, pipsmr;
};
extern Hwdist_t* gHwdist;

// Arrays for temporarily storing heavy b and c hadrons decaying partonically
struct Hwqdks_t
{
    double  vtxqdk[4*m_nmxqdk];
    int     imqdk[m_nmxqdk], locq[m_nmxqdk], nqdk;
};
extern Hwqdks_t* gHwqdks;

// Parameters for Sudakov form factors
struct Hwusud_t
{
    double  accur, qev[m_nmxsud*6], sud[m_nmxsud*6];
    int     inter, nqev, nsud, sudord;
};
extern Hwusud_t* gHwusud;

// SUSY parameters
struct Hwsusy_t
{
    double  tanb, alphah, cosbpa, sinbpa, cosbma, sinbma, cosa, sina,
            cosb, sinb, cotb, zmixss[4*4], zmxnss[4*4], zsgnss[4],
            lfch[16], rfch[16], slfch[16*4], srfch[16*4], wmxuss[2*2],
            wmxvss[2*2], wsgnss[2], qmixss[6*2*2], lmixss[6*2*2],
            thetat, thetab, thetal, atss, abss, alss, muss, factss,
            ghwwss[3], ghzzss[3], ghddss[4], ghuuss[4], ghwhss[3],
            ghsqss[4*6*2*2], xlmnss, rmmnss, dmssm, senhnc[24], ssparity;
    logical susyin;
};
extern  Hwsusy_t* gHwsusy;

// R-parity violating parameters and colours
struct Hwrpar_t
{
    double  lamda1[3*3*3], lamda2[3*3*3], lamda3[3*3*3];
    int     hrdcol[2*5];
    logical rparty, colupd;
};
extern Hwrpar_t* gHwrpar;

// Parameters for minimum-bias / soft underlying event
struct Hwminb_t
{
    double  pmbn1, pmbn2, pmbn3, pmbk1, pmbk2, pmbm1, pmbm2,
            pmbp1, pmbp2, pmbp3;
};
extern Hwminb_t* gHwminb;

// Cluster common used by soft event routines
struct Hwclus_t
{
    double  ppcl[5*m_nmxcl];
    int     idcl[m_nmxcl], ncl;
};
extern Hwclus_t* gHwclus;

// Parameters for resonant graviton production
struct Hwgrav_t
{
    double  grvlam, emgrv, gamgrv;
};
extern Hwgrav_t* gHwgrav;

// Other new parameters for Herwig 6.2
struct Hw6202_t
{
    double  vipwid[3], dxrcyl, dxzmax, dxrsph;
    logical wzrfr, fix4jt;
    int     imssm, ihiggs, parity, lrsusy;
};
extern Hw6202_t* gHw6202;

// New parameters for Herwig 6.203
struct Hw6203_t
{
    double  abwgt, abwsum, avabw;
    int     nnegwt, nnegev;
    logical negwts;
};
extern Hw6203_t* gHw6203;

// New parameters for Herwig 6.3
struct Hw6300_t
{
    double  mjjmin, chnprb[m_imaxch];
    int     iopstp, iopsh;
    logical optm, chon[m_imaxch];
};
extern  Hw6300_t* gHw6300;

// New PDFs for Herwig 6.3
struct Hwpmrs_t
{
    double  fmrs[3*m_npmrs*m_nxmrs*(m_nqmrs+1)];
};
extern Hwpmrs_t* gHwpmrs;

// Circe interface for Herwig 6.3
struct Hwcirc_t
{
    int     circop, circac, circvr, circrv, circch;
};
extern Hwcirc_t* gHwcirc;

// New commons for spin correlations
struct Hwdspb_t
{
    double  abmode[2*m_nmodeb], bbmode[2*12*m_nmodeb], pbmode[12*m_nmodeb],
            wtbmax[12*m_nmodeb];
    int     idbprt[m_nmodeb], ibdrtp[m_nmodeb], ibmode[m_nmodeb], nbmode;
};
extern Hwdspb_t* gHwdspb;


// Common blocks for two-body decays
struct Hwdsp2_t
{
    double  a2mode[2*m_nmode2], p2mode[m_nmode2], wt2max[m_nmode2];
    int     id2prt[m_nmode2], i2drtp[m_nmode2], n2mode;
};
extern Hwdsp2_t* gHwdsp2;

// Common block for three-body decays
struct Hwdsp3_t
{
    double  a3mode[2*m_ndiagr*m_nmode3], b3mode[2*m_ndiagr*m_nmode3],
            p3mode[m_nmode3], wt3max[m_nmode3],
            spn3cf[m_ncfmax*m_ncfmax*m_nmode3];
    int     id3prt[m_nmode3], i3mode[m_ndiagr*m_nmode3],
            i3drtp[m_ndiagr*m_nmode3], n3mode, ndi3by[m_nmode3],
            n3ncfl[m_nmode3], i3drcf[m_ndiagr*m_nmode3];
};
extern Hwdsp3_t* gHwdsp3;

// Common block for four-body decays
struct Hwdsp4_t
{
    double  a4mode[2*12*m_nmode4], b4mode[2*12*m_nmode4],
            p4mode[12*12*m_nmode4], wt4max[12*12*m_nmode4];
    int     id4prt[m_nmode4], i4mode[2*m_nmode4], n4mode;
};
extern Hwdsp4_t* gHwdsp4;

// Common block for spin correlations in event
struct Hwdspn_t
{
    int     ndecsy, nsearch, lrdec, lwdec;
    logical syspin, threeb, fourb;
    char    taudec[6];
};
extern Hwdspn_t* gHwdspn;

struct Hwspin_t
{
    std::complex<double> mespn[2*2*2*2*m_ncfmax*m_nmxspn], rhospn[2*2*m_nmxspn];
    double  spncfc[m_ncfmax*m_ncfmax*m_nmxspn];
    int     idspn[m_nmxspn], jmospn[m_nmxspn], jdaspn[2*m_nmxspn], nspn,
            isnhep[m_nmxhep], nsntry;
    logical decspn[m_nmxspn];
    int     ncfl[m_nmxspn], spcopt;
};
extern Hwspin_t* gHwspin;

struct Hwstau_t
{
    int     jak1, jak2, itdkrc, ifphot;
};
extern Hwstau_t* gHwstau;

// Common block for Les Houches interface
struct Hwgupr_t
{
    double  lhwgt[m_maxhrp], lhwgts[m_maxhrp], lhxsct[m_maxhrp],
            lhxerr[m_maxhrp], lhxmax[m_maxhrp], lhmxsm;
    int     lhiwgt[m_maxhrp], lhnevt[m_maxhrp], ityplh;
    logical lhsoft, lhglsf;
};
extern Hwgupr_t* gHwgupr;

// Common block for Herwig 6.5
struct Hw6500_t
{
    logical prespl;
};
extern Hw6500_t* gHw6500;

// Common block for Herwig 6.504
struct Hw6504_t
{
    int    itoprd;
};
extern Hw6504_t* gHw6504;

// Common block for Herwig 6.506
struct Hw6506_t
{
    double  pdfx0,pdfpow;
};
extern Hw6506_t* gHw6506;

// Common block for Herwig 6.510
struct Hw6510_t
{
    int     ndetry;
};
extern Hw6510_t* gHw6510;

// Declare pointers for accessing the common blocks

Hwbeam_t* gHwbeam;
Hwbmch_t* gHwbmch;
Hwproc_t* gHwproc;
Hwpram_t* gHwpram;
Hwprch_t* gHwprch;
Hwpart_t* gHwpart;
Hwparp_t* gHwparp;
Hwbosc_t* gHwbosc;
Hwparc_t* gHwparc; 
Hwbrch_t* gHwbrch;
Hwevnt_t* gHwevnt;
Hwhard_t* gHwhard;
Hwprop_t* gHwprop;
Hwunam_t* gHwunam;
Hwupdt_t* gHwupdt;
Hwuwts_t* gHwuwts;
Hwuclu_t* gHwuclu;
Hwdist_t* gHwdist;
Hwqdks_t* gHwqdks;
Hwusud_t* gHwusud;
// 4 new, v61
Hwsusy_t* gHwsusy;
Hwrpar_t* gHwrpar;
Hwminb_t* gHwminb;
Hwclus_t* gHwclus;
// 2 new, v6202
Hwgrav_t* gHwgrav;
Hw6202_t* gHw6202;
// 1 new 6203
Hw6203_t* gHw6203;
// new 6300
Hw6300_t* gHw6300;
Hwpmrs_t* gHwpmrs;
Hwcirc_t* gHwcirc;

// new 6400
Hwdspb_t* gHwdspb;
Hwdsp2_t* gHwdsp2; 
Hwdsp3_t* gHwdsp3;
Hwdsp4_t* gHwdsp4;
Hwdspn_t* gHwdspn;
Hwspin_t* gHwspin;
Hwstau_t* gHwstau;

// new 6500
Hwgupr_t* gHwgupr;
Hw6500_t* gHw6500;

// new 6504
Hw6504_t* gHw6504;

// new 6506
Hw6506_t* gHw6506;

// new 6510
Hw6510_t* gHw6510;

#endif
