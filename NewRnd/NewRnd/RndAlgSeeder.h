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
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AnyDataHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "NewRnd/RndCommon.h"

#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class G4Event;
namespace HepMC
{
  class GenEvent;
}

/** @class IExtEngine
 *
 *  Abstract interface to tool converting HepMC3 to Geant4
 *
 *  @author Dominik Muller
 *  @date   22.6.2018
 */

class IExtEngine : virtual public IAlgTool, virtual public RndCommon::RndConstructor
{
public:
  DeclareInterfaceID( IExtEngine, 1, 0 );
  virtual ~IExtEngine() = default;
  virtual CLHEP::HepRandomEngine* construct() const override = 0;
};

namespace Random
{
  typedef std::pair<unsigned int, unsigned int> SeedPair;
  const std::string Location = "SeedsInternalUsage";
}

class RndAlgSeeder : public GaudiAlgorithm
{
  Gaudi::Property<int> m_forcedSeed{this, "ForcedSeed", 0, "Force seed to value if not 0"};
  ToolHandle<IExtEngine> m_engine_tool{this, "RandomEngine", "MixMaxRng"};

public:
  using GaudiAlgorithm::GaudiAlgorithm;
  using GaudiAlgorithm::finalize;
  using GaudiAlgorithm::initialize;

protected:
  // Checks if a new engine needs to be created, does so and returns a reference to it.
  HepRandomEnginePtr createRndmEngine() const;
private:
  DataObjectReadHandle<Random::SeedPair> m_forseed{Random::Location, this};
};

class RndInitAlg : public GaudiAlgorithm
{

public:
  using GaudiAlgorithm::GaudiAlgorithm;
  using GaudiAlgorithm::finalize;
  using GaudiAlgorithm::initialize;

protected:
  void SetSeedPair( unsigned int val1, unsigned int val2 ) const { m_forseed.put( Random::SeedPair{ val1, val2 } ); }
  std::pair<unsigned int, unsigned int> GetSeedPair( ) const { return *m_forseed.get(); }

private:
  mutable DataObjectWriteHandle<Random::SeedPair> m_forseed{Random::Location, this};
};
