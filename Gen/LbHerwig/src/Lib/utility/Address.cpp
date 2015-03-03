// $Id: Address.cpp,v 1.1 2006-09-05 12:36:50 karl Exp $

//  Sept 09 1997 P.Murat (CDF)
//  this routine is supposed to be called from FORTRAN, it returns an
//  address of its parameter
//  revision history:
//  -----------------
//------------------------------------------------------------------------------
extern "C" int address_(int var) {
  return var;
}

