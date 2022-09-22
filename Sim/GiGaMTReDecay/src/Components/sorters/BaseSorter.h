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
