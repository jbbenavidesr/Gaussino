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
// $Id: GaussGenUtil.h,v 1.1 2007-03-07 18:51:00 gcorti Exp $
#ifndef GAUSSGENUTIL_H 
#define GAUSSGENUTIL_H 1

// Include files
#include "HepMC3/GenParticle.h"


/** @namespace GenUtil GenUtil.h
 *  @brief Utilities for generator
 *  The lifetime returned is c*tau (mm)
 *
 *  @author Gloria CORTI
 *  @date   2007-02-16
 */

namespace GaussGenUtil {

  double lifetime( const HepMC3::ConstGenParticlePtr& );
  
}

#endif // GAUSSGENUTIL_H
