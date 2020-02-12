#pragma once

#include "GiGaMTReDecay/IReDecaySorter.h"
#include "GaudiAlg/GaudiTool.h"
#include "HepMC3/GenParticle_fwd.h"
#include "HepMCUser/typedefs.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BaseSorter
//
// 2020-02-10: Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
class BaseSorter : public extends<GaudiTool, IReDecaySorter>
{

public:
  /// Standard constructor
  using extends::extends;

protected:

  void RecursiveTagger(HepMC3::GenParticlePtr) const;
  void RemoveDecayTrees(std::vector<HepMC3::GenEventPtr> &) const;

};
