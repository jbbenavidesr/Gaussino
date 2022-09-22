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

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "RndInit/IRndSeedingTool.h"

// Forward declarations
class IRndmEngine;
class IRndmGenSvc;

/** @class SeedingTool SeedingTool.h "SeedingTool.h"
 *
 *  Class to seed the random number service, similar to what was done in
 * LbAppInit. This is probably a bad idea once going multithreaded so this is
 * just hacked togethter to work.
 *
 *  @author Dominik Muller
 *  @date   2018-01-30
 */

class SeedingTool : public GaudiTool, virtual public IRndSeedingTool {
  public:
  SeedingTool(const std::string& type, const std::string& name,
              const IInterface* parent)
      : GaudiTool(type, name, parent) {
    declareInterface<IRndSeedingTool>(this);
  }

  virtual StatusCode initialize() override;

  virtual StatusCode seed(
      unsigned int seed1, unsigned long long seed2,
      std::shared_ptr<std::vector<long int>> seeds = nullptr) override;

  private:
  mutable SmartIF<IRndmGenSvc> m_randSvc;
  mutable IRndmEngine* m_engine{nullptr};
};
