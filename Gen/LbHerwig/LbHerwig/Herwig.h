// $Id: Herwig.h,v 1.5 2007-04-25 12:45:14 karl Exp $
#ifndef LBHERWIG_HERWIG_H 
#define LBHERWIG_HERWIG_H 1

// Include files
#include "LbHerwig/herwig6510.h"
#include "LbHerwig/jimmy422.h"
#include "LbHerwig/mcatnlo32.h"

/** @class Herwig Herwig.h LbHerwig/Herwig.h
 * 
 *  Utility class providing access to Fortran function and common blocks
 *  of Herwig, MC@NLO and Jimmy
 *
 *  @author Karl Harrison
 *  @date   Created: 24th October 2005 / Last update: 1st September 2006
 */
class Herwig
{
public:

   void getAddress( char*, int );

   static void hwabeg();
   static void hwaend();
   static void hwanal();
   static void hwbgen();
   static void hwcdec();
   static void hwcfor();
   static void hwdhad();
   static void hwdhob();
   static void hwdhvy();
   static void hwefin();
   static void hweini();
   static void hwepro();
   static void hwrmdk();
   static void hwigin();
   static void hwmevt();

   static void hwmsct( logical & abort );

   static void hwudpr();
   static void hwuinc();
   static void hwuine();
   static void hwufne();
   static void hwupinit();
   static void hwusta( char & name );

   void initHerwigCommonBlocks();

   static void jimmin();
   static void jmefin();
   static void jminit();
   static void jmwjmx();

   static void mcatnlo_header();
   static void mcatnlo_qqinit();
   static void mcatnlo_qqgen();

   static void setlhacblk( char & name );

protected:

private:

};
#endif // LBHERWIG_HERWIG_H
