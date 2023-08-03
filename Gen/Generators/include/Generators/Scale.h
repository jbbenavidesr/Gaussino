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
// $Id: Scale.h,v 1.1 2006-10-06 14:11:16 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GENERATORS_SCALE_H
#  define GENERATORS_SCALE_H 1
// ============================================================================
// HepMC
// ============================================================================
#  include "HepMC3/GenEvent.h"
// ============================================================================

namespace GeneratorUtils {
  /** @fn scale
   *  Simple functon to rescale HepMCEvent
   *  in between Pythina and LHC units
   *  @param event generator event
   *  @param mom   scale factor for 4-momentum
   *  @param time  scale factor for time component of the position vector
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-05
   */
  void scale( HepMC3::GenEvent* event, const double mom, const double time );

} // end of namespace GeneratorUtils

// ============================================================================
// The END
// ============================================================================
#endif // GENERATORS_SCALE_H
// ============================================================================
