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
#include "boost/format.hpp"

#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

#include "SeedingTool.h"

DECLARE_COMPONENT(SeedingTool)

StatusCode SeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  m_randSvc = service("RndmGenSvc", true);
  if (m_randSvc) {
    m_engine = m_randSvc->engine();
  }
  if (!m_randSvc) {
    return Error("Random number service not found!");
  }
  if (!m_engine) {
    return Error("Random number engine not found!");
  }

  return StatusCode::SUCCESS;
}

StatusCode SeedingTool::seed(
    unsigned int seed1, unsigned long long seed2,
    std::shared_ptr<std::vector<long int>> seeds /*= nullptr*/) {
  // First create the seeds from the numbers, just copy and paste from LbAppinit
  if (seeds == nullptr) {
    seeds = std::make_shared<std::vector<long int>>();
  }
  int seed1a = seed1 & 0x7FFFFFFF;

  // Make two 31 bit seeds out of seed2
  int seed2a = (int)(seed2 & 0x7FFFFFFF);
  int seed2b = (int)((seed2 >> 32) & 0x7FFFFFFF);

  if (seeds) {
    if (0 != seed1a) {
      seeds->push_back(seed1a);
    }
    if (0 != seed2a) {
      seeds->push_back(seed2a);
    }
    if (0 != seed2b) {
      seeds->push_back(seed2b);
    }
  }

  // Get last seed by hashing string containing seed1 and seed2
  const std::string s =
      name() +
      (boost::io::str(
          boost::format("_%1%_%2%") %
          boost::io::group(std::setfill('0'), std::hex, std::setw(8), seed1) %
          boost::io::group(std::setfill('0'), std::hex, std::setw(16), seed2)));

  //--> Hash32 algorithm from Pere Mato
  int hash = 0;
  for (std::string::const_iterator iC = s.begin(); s.end() != iC; ++iC) {
    hash += *iC;
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  // CLHEP uses the last seed as a seed (only 24 bits used) but also to generate
  // more pseudorandom seeds to populate the "seeds" vector to its capacity of
  // 24 seeds. For this generation, 31 bits are used
  seeds->push_back(abs(hash));
  seeds->push_back(0);

  StatusCode sc = m_engine->setSeeds(*seeds.get());
  if (sc.isFailure()) {
    return Error("Unable to set random number seeds", sc);
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "using seeds " << *seeds.get() << endmsg;
  }

  return StatusCode::SUCCESS;
}
