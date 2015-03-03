// $Id: Herwig.cpp,v 1.2 2007-04-25 12:45:15 karl Exp $

// Include files

// local
#include "LbHerwig/Herwig.h"
#include<iostream>

//-----------------------------------------------------------------------------
// Implementation file for class : Herwig
//
// 2005-10-24 : Karl Harrison
//-----------------------------------------------------------------------------

// HERWIG_COMMON_BLOCK_ADDRESS Fortran function
extern "C"
{
#ifdef WIN32
  void* __stdcall HERWIG_COMMON_BLOCK_ADDRESS( char*, int ) ;
  void* __stdcall JIMMY_COMMON_BLOCK_ADDRESS( char*, int ) ;
  void* __stdcall MCATNLO_COMMON_BLOCK_ADDRESS( char*, int ) ;
#else
  void* herwig_common_block_address_( char*, int ) ;
  void* jimmy_common_block_address_( char*, int ) ;
  void* mcatnlo_common_block_address_( char*, int ) ;
#endif
}

static int herwigCommonBlocksInitialized = 0;

void Herwig::initHerwigCommonBlocks()
{

  if ( herwigCommonBlocksInitialized ) return;

  herwigCommonBlocksInitialized = 1;

  // Herwig
  gHwbeam    = (Hwbeam_t*   ) herwig_common_block_address_("HWBEAM",6); 
  gHwbmch    = (Hwbmch_t*   ) herwig_common_block_address_("HWBMCH",6); 
  gHwbosc    = (Hwbosc_t*   ) herwig_common_block_address_("HWBOSC",6);
  gHwbrch    = (Hwbrch_t*   ) herwig_common_block_address_("HWBRCH",6);
  gHwcirc    = (Hwcirc_t*   ) herwig_common_block_address_("HWCIRC",6);
  gHwclus    = (Hwclus_t*   ) herwig_common_block_address_("HWCLUS",6);
  gHwdist    = (Hwdist_t*   ) herwig_common_block_address_("HWDIST",6); 
  gHwdktl    = (Hwdktl_t*   ) herwig_common_block_address_("HWDKTL",6); 
  gHwdspb    = (Hwdspb_t*   ) herwig_common_block_address_("HWDSPB",6);
  gHwdspn    = (Hwdspn_t*   ) herwig_common_block_address_("HWDSPN",6);
  gHwdsp2    = (Hwdsp2_t*   ) herwig_common_block_address_("HWDSP2",6);
  gHwdsp3    = (Hwdsp3_t*   ) herwig_common_block_address_("HWDSP3",6);
  gHwdsp4    = (Hwdsp4_t*   ) herwig_common_block_address_("HWDSP4",6);
  gHwevnt    = (Hwevnt_t*   ) herwig_common_block_address_("HWEVNT",6); 
  gHwgupr    = (Hwgupr_t*   ) herwig_common_block_address_("HWGUPR",6);
  gHwgrav    = (Hwgrav_t*   ) herwig_common_block_address_("HWGRAV",6);
  gHwhard    = (Hwhard_t*   ) herwig_common_block_address_("HWHARD",6); 
  gHwminb    = (Hwminb_t*   ) herwig_common_block_address_("HWMINB",6);
  gHwparc    = (Hwparc_t*   ) herwig_common_block_address_("HWPARC",6);
  gHwparp    = (Hwparp_t*   ) herwig_common_block_address_("HWPARP",6);
  gHwpart    = (Hwpart_t*   ) herwig_common_block_address_("HWPART",6);
  gHwpmrs    = (Hwpmrs_t*   ) herwig_common_block_address_("HWPMRS",6);
  gHwpram    = (Hwpram_t*   ) herwig_common_block_address_("HWPRAM",6);
  gHwprch    = (Hwprch_t*   ) herwig_common_block_address_("HWPRCH",6);
  gHwproc    = (Hwproc_t*   ) herwig_common_block_address_("HWPROC",6);
  gHwprop    = (Hwprop_t*   ) herwig_common_block_address_("HWPROP",6); 
  gHwqdks    = (Hwqdks_t*   ) herwig_common_block_address_("HWQDKS",6); 
  gHwrpar    = (Hwrpar_t*   ) herwig_common_block_address_("HWRPAR",6);
  gHwspin    = (Hwspin_t*   ) herwig_common_block_address_("HWSPIN",6);
  gHwstau    = (Hwstau_t*   ) herwig_common_block_address_("HWSTAU",6);
  gHwsusy    = (Hwsusy_t*   ) herwig_common_block_address_("HWSUSY",6);
  gHwuclu    = (Hwuclu_t*   ) herwig_common_block_address_("HWUCLU",6); 
  gHwunam    = (Hwunam_t*   ) herwig_common_block_address_("HWUNAM",6);  
  gHwupdt    = (Hwupdt_t*   ) herwig_common_block_address_("HWUPDT",6); 
  gHwusud    = (Hwusud_t*   ) herwig_common_block_address_("HWUSUD",6);
  gHwuwts    = (Hwuwts_t*   ) herwig_common_block_address_("HWUWTS",6); 
  gHw6202    = (Hw6202_t*   ) herwig_common_block_address_("HW6202",6);
  gHw6203    = (Hw6203_t*   ) herwig_common_block_address_("HW6203",6);
  gHw6300    = (Hw6300_t*   ) herwig_common_block_address_("HW6300",6);
  gHw6500    = (Hw6500_t*   ) herwig_common_block_address_("HW6500",6);
  gHw6504    = (Hw6504_t*   ) herwig_common_block_address_("HW6504",6);
  gHw6506    = (Hw6506_t*   ) herwig_common_block_address_("HW6506",6);
  gHw6510    = (Hw6510_t*   ) herwig_common_block_address_("HW6510",6);

  // MC@NLO
  gCetacut   = (Cetacut_t*  ) mcatnlo_common_block_address_("CETACUT",7); 
  gCiwgtnorm = (Ciwgtnorm_t*) mcatnlo_common_block_address_("CIWGTNORM",9); 
  gFixvar    = (Fixvar_t*   ) mcatnlo_common_block_address_("FIXVAR",6); 
  gMcnlopar  = (Mcnlopar_t* ) mcatnlo_common_block_address_("MCNLOPAR",8); 
  gParmass   = (Parmass_t*  ) mcatnlo_common_block_address_("PARMASS",7); 
  gVvjin     = (Vvjin_t*    ) mcatnlo_common_block_address_("VVJIN",5); 

  // Jimmy
  gJmparm    = (Jmparm_t*   ) jimmy_common_block_address_("JMPARM",6);

}

// HWABEG Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWABEG( ) ;
#else
  void hwabeg_( ) ;
#endif
}

void Herwig::hwabeg()
{
#ifdef WIN32
  HWABEG( ) ;
#else
  hwabeg_ ( ) ;
#endif
}

// HWAEND Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWAEND( ) ;
#else
  void hwaend_( ) ;
#endif
}

void Herwig::hwaend()
{
#ifdef WIN32
  HWAEND( ) ;
#else
  hwaend_ ( ) ;
#endif
}

// HWANAL Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWANAL( ) ;
#else
  void hwanal_( ) ;
#endif
}

void Herwig::hwanal()
{
#ifdef WIN32
  HWANAL( ) ;
#else
  hwanal_ ( ) ;
#endif
}

// HWBGEN Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWBGEN( ) ;
#else
  void hwbgen_( ) ;
#endif
}

void Herwig::hwbgen()
{
#ifdef WIN32
  HWBGEN( ) ;
#else
  hwbgen_ ( ) ;
#endif
}

// HWCDEC Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWCDEC( ) ;
#else
  void hwcdec_( ) ;
#endif
}

void Herwig::hwcdec()
{
#ifdef WIN32
  HWCDEC( ) ;
#else
  hwcdec_ ( ) ;
#endif
}

// HWCFOR Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWCFOR( ) ;
#else
  void hwcfor_( ) ;
#endif
}

void Herwig::hwcfor()
{
#ifdef WIN32
  HWCFOR( ) ;
#else
  hwcfor_ ( ) ;
#endif
}

// HWDHAD Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWDHAD( ) ;
#else
  void hwdhad_( ) ;
#endif
}

void Herwig::hwdhad()
{
#ifdef WIN32
  HWDHAD( ) ;
#else
  hwdhad_ ( ) ;
#endif
}

// HWDHOB Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWDHOB( ) ;
#else
  void hwdhob_( ) ;
#endif
}

void Herwig::hwdhob()
{
#ifdef WIN32
  HWDHOB( ) ;
#else
  hwdhob_ ( ) ;
#endif
}

// HWDHVY Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWDHVY( ) ;
#else
  void hwdhvy_( ) ;
#endif
}

void Herwig::hwdhvy()
{
#ifdef WIN32
  HWDHVY( ) ;
#else
  hwdhvy_ ( ) ;
#endif
}

// HWRMDK Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWRMDK( ) ;
#else
  void hwrmdk_( ) ;
#endif
}

void Herwig::hwrmdk()
{
#ifdef WIN32
  HWRMDK( ) ;
#else
  hwrmdk_ ( ) ;
#endif
}

// HWEFIN Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWEFIN( ) ;
#else
  void hwefin_( ) ;
#endif
}

void Herwig::hwefin()
{
#ifdef WIN32
  HWEFIN( ) ;
#else
  hwefin_ ( ) ;
#endif
}

// HWEINI Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWEINI( ) ;
#else
  void hweini_( ) ;
#endif
}

void Herwig::hweini()
{
#ifdef WIN32
  HWEINI( ) ;
#else
  hweini_ ( ) ;
#endif
}

// HWEPRO Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWEPRO( ) ;
#else
  void hwepro_( ) ;
#endif
}

void Herwig::hwepro()
{
#ifdef WIN32
  HWEPRO( ) ;
#else
  hwepro_ ( ) ;
#endif
}

// HWIGIN Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWIGIN( ) ;
#else
  void hwigin_( ) ;
#endif
}

void Herwig::hwigin()
{
#ifdef WIN32
  HWIGIN( ) ;
#else
  hwigin_ ( ) ;
#endif
}

// HWMEVT Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWMEVT( ) ;
#else
  void hwmevt_( ) ;
#endif
}

void Herwig::hwmevt()
{
#ifdef WIN32
  HWMEVT( ) ;
#else
  hwmevt_ ( ) ;
#endif
}

// HWMSCT Fortran function (from Jimmy)
extern "C"
{
#ifdef WIN32
  void __stdcall HWMSCT( logical & abort ) ;
#else
  void hwmsct_( logical & abort ) ;
#endif
}

void Herwig::hwmsct( logical & abort )
{
#ifdef WIN32
  HWMSCT( abort ) ;
#else
  hwmsct_ ( abort ) ;
#endif
}


// HWUDPR Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWUDPR( ) ;
#else
  void hwudpr_( ) ;
#endif
}

void Herwig::hwudpr()
{
#ifdef WIN32
  HWUDPR( ) ;
#else
  hwudpr_ ( ) ;
#endif
}

// HWUINC Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWUINC( ) ;
#else
  void hwuinc_( ) ;
#endif
}

void Herwig::hwuinc()
{
#ifdef WIN32
  HWUINC( ) ;
#else
  hwuinc_ ( ) ;
#endif
}

// HWUINE Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWUINE( ) ;
#else
  void hwuine_( ) ;
#endif
}

void Herwig::hwuine()
{
#ifdef WIN32
  HWUINE( ) ;
#else
  hwuine_ ( ) ;
#endif
}

// HWUFNE Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWUFNE( ) ;
#else
  void hwufne_( ) ;
#endif
}

void Herwig::hwufne()
{
#ifdef WIN32
  HWUFNE( ) ;
#else
  hwufne_ ( ) ;
#endif
}

// UPINIT Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWUPINIT( ) ;
#else
  void hwupinit_( ) ;
#endif
}

void Herwig::hwupinit()
{
#ifdef WIN32
  HWUPINIT( ) ;
#else
  hwupinit_ ( ) ;
#endif
}

// HWUSTA Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall HWUSTA( char & name ) ;
#else
  void hwusta_( char & name ) ;
#endif
}

void Herwig::hwusta( char & name )
{
#ifdef WIN32
  HWUSTA( name ) ;
#else
  hwusta_ ( name ) ;
#endif
}

// JIMMIN Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall JIMMIN( ) ;
#else
  void jimmin_( ) ;
#endif
}

void Herwig::jimmin()
{
#ifdef WIN32
  JIMMIN( ) ;
#else
  jimmin_ ( ) ;
#endif
}

// JMEFIN Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall JMEFIN( ) ;
#else
  void jmefin_( ) ;
#endif
}

void Herwig::jmefin()
{
#ifdef WIN32
  JMEFIN( ) ;
#else
  jmefin_ ( ) ;
#endif
}

// JMINIT Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall JMINIT( ) ;
#else
  void jminit_( ) ;
#endif
}

void Herwig::jminit()
{
#ifdef WIN32
  JMINIT( ) ;
#else
  jminit_ ( ) ;
#endif
}

// JMWJMX Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall JMWJMX( ) ;
#else
  void jmwjmx_( ) ;
#endif
}

void Herwig::jmwjmx()
{
#ifdef WIN32
  JMWJMX( ) ;
#else
  jmwjmx_ ( ) ;
#endif
}


// MCATNLO_HEADER Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall MCATNLO_HEADER( ) ;
#else
  void mcatnlo_header_( ) ;
#endif
}

void Herwig::mcatnlo_header()
{
#ifdef WIN32
  MCATNLO_HEADER( ) ;
#else
  mcatnlo_header_ ( ) ;
#endif
}

// MCATNLO_QQGEN Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall MCATNLO_QQGEN( ) ;
#else
  void mcatnlo_qqgen_( ) ;
#endif
}

void Herwig::mcatnlo_qqgen()
{
#ifdef WIN32
  MCATNLO_QQGEN( ) ;
#else
  mcatnlo_qqgen_ ( ) ;
#endif
}

// MCATNLO_QQINIT Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall MCATNLO_QQINIT( ) ;
#else
  void mcatnlo_qqinit_( ) ;
#endif
}

void Herwig::mcatnlo_qqinit()
{
#ifdef WIN32
  MCATNLO_QQINIT( ) ;
#else
  mcatnlo_qqinit_ ( ) ;
#endif
}

// SETLHACBLK Fortran function
extern "C"
{
#ifdef WIN32
  void __stdcall SETLHACBLK( char & name ) ;
#else
  void setlhacblk_( char & name ) ;
#endif
}

void Herwig::setlhacblk( char & name )
{
#ifdef WIN32
  SETLHACBLK( name ) ;
#else
  setlhacblk_ ( name ) ;
#endif
}
