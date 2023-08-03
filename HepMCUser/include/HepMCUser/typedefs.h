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
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include <vector>

namespace HepMC3 {
  typedef std::shared_ptr<GenEvent>   GenEventPtr;
  typedef std::vector<GenEventPtr>    GenEventPtrs;
  typedef std::vector<GenParticlePtr> GenParticlePtrs;
} // namespace HepMC3
