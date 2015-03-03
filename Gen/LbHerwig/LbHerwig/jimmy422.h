// $Id: jimmy422.h,v 1.1 2006-09-05 12:36:47 karl Exp $
#ifndef __JIMMY42_HH__
#define __JIMMY42_HH__ 1

// Declarations for common block of Jimmy 4.2

typedef int logical;

const int m_maxms = 100;
const int m_nproc = 117;
const int m_npsimp = 16;

// Jimmy parameters

struct Jmparm_t
{
    double  ptjim, ygamma, jmzmin, jmrad[ 264 ], phad, jmu2, jmv2,
            jmarry[ ( 6 + m_maxms ) * ( 1 + m_npsimp ) ], nlost, totscat;
    logical anomoff;
    int     jcmvar, jmue0, jmptyp[ m_nproc ], jmbug, fn_type, msflag, maxmstry;
};
extern Jmparm_t* gJmparm;

// Declare pointer for accessing the common block

Jmparm_t* gJmparm;

#endif
