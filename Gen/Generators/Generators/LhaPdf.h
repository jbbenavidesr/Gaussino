/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// $Id: LhaPdf.h,v 1.3 2008-05-23 11:55:05 robbep Exp $
#ifndef GENERATORS_LHAPDF_H 
#define GENERATORS_LHAPDF_H 1

// Include files
#include "Generators/LHAPDFCommonBlocks.h"

/** @class LhaPdf LhaPdf.h "Generators/LhaPdf.h"
 * 
 *  Utility class to access LHAPDF Fortran Functions. 
 *
 *  @author Patrick Robbe
 *  @date   2005-12-06
 */
class LhaPdf {
 public:
  /// Access to LHACONTROL common block
  static Lhacontrol & lhacontrol() { return s_lhacontrol ; }
  
 private:
  static Lhacontrol s_lhacontrol ; ///< LHACONTROL Common Block data
};
#endif // GENERATORS_LHAPDF_H
