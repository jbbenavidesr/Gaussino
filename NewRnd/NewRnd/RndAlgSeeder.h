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

/** @class IHepMC3ToGeant4Tool SimG4Interface/IHepMC3ToGeant4Tool.h IHepMC3ToGeant4Tool.h
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
  PublicToolHandle<IExtEngine> m_engine_tool{this, "RandomEngine", "MixMaxRng"};

public:
  using GaudiAlgorithm::GaudiAlgorithm;
  using GaudiAlgorithm::finalize;
  using GaudiAlgorithm::initialize;

protected:
  // Checks if a new engine needs to be created, does so and returns a reference to it.
  HepRandomEnginePtr createRndmEngine() const;
private:
  AnyDataHandle<Random::SeedPair> m_forseed{Random::Location, Gaudi::DataHandle::Reader, this};
};

class RndInitAlg : public GaudiAlgorithm
{

public:
  using GaudiAlgorithm::GaudiAlgorithm;
  using GaudiAlgorithm::finalize;
  using GaudiAlgorithm::initialize;

protected:
  void SetSeedPair( unsigned int val1, unsigned int val2 ) const { m_forseed.put( std::make_pair( val1, val2 ) ); }

private:
  mutable AnyDataHandle<Random::SeedPair> m_forseed{Random::Location, Gaudi::DataHandle::Writer, this};
};
