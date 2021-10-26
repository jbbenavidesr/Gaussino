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
#pragma once

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IFullGenEventCutTool.h"
#include "GaudiKernel/Transform4DTypes.h"

/** @class MinimalNGenParticles MinimalNGenParticles.h
 *
 *  Simple full event cut tool to require a minimal number of particles.
 *  Intended for performance and throughput tests.
 *
 *  @author Dominik Muller
 *  @date   2018-09-05
 */
class MinimalNGenParticles : public extends<GaudiTool, IFullGenEventCutTool> {
  Gaudi::Property<unsigned int> m_minParticles{this, "NGenParticles",1,"Required number of generated particles"};
 public:
   using extends::extends;

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool studyFullEvent( const HepMC3::GenEventPtrs & theEvents ,
                               const LHCb::GenCollisions & theCollisions ) const override;

};
