#pragma once

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IFullGenEventCutTool.h"
#include "GaudiKernel/Transform4DTypes.h"

/** @class MinimalStableInLHCb MinimalStableInLHCb.h
 *
 *  Simple full event cut tool to require a minimal number of particles.
 *  Intended for performance and throughput tests.
 *
 *  @author Dominik Muller
 *  @date   2018-09-05
 */
class MinimalStableInLHCb : public extends<GaudiTool, IFullGenEventCutTool> {
  Gaudi::Property<unsigned int> m_minParticles{this, "NGenParticles",1,"Required number of generated particles"};
  Gaudi::Property<double> m_minEta{this, "MinEta", 2.0};
  Gaudi::Property<double> m_maxEta{this, "MaxEta", 4.9};
 public:
   using extends::extends;

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool studyFullEvent( const HepMC3::GenEventPtrs & theEvents ,
                               const LHCb::GenCollisions & theCollisions ) const override;

};
