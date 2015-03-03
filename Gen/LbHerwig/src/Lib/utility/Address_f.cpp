// $Id: Address_f.cpp,v 1.1 2006-09-05 12:36:51 karl Exp $

//  Sept 09 1997 P.Murat (CDF)
//  this routine is supposed to be called from FORTRAN, it returns an
//  address of its parameter
//  revision history:
//  -----------------
//------------------------------------------------------------------------------
extern "C" int adressf_(int var) {
  return var;
}

