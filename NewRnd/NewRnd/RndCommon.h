#pragma once
#include "CLHEP/Random/RandomEngine.h"
#include "boost/format.hpp"
#include <memory>
#include <stdexcept>
#include <string>

/**
 * @file RndCommon.h
 * @brief Common utilities for managing CLHEP engines in multithreaded code
 * @author Dominik Muller
 * @version 1.0
 * @date 2019-01-21
 */

class HepRandomEnginePtr;

namespace RndCommon
{
  class RndConstructor
  {
  public:
    virtual CLHEP::HepRandomEngine* construct() const = 0;
  };

  void seedEngine( CLHEP::HepRandomEngine& engine, unsigned int seed1, unsigned int seed2,
                   std::string label = "Random" );
  void seedEngine( HepRandomEnginePtr& engine, unsigned int seed1, unsigned int seed2, std::string label = "Random" );
}

// SmartPointer wrapper for HepRandomEngines that allows to optionally set a constructor for subengines.
// Subengines are stored and deleted when wrapper is destroyed.
class HepRandomEnginePtr
{
public:
  HepRandomEnginePtr() = delete;
  HepRandomEnginePtr( CLHEP::HepRandomEngine* engine, const RndCommon::RndConstructor* constructor = nullptr,
                      std::string label = "Random" )
      : m_engine( engine ), m_constructor( constructor ), m_label( label )
  {
  }

  // Get pointer to storeed engine
  CLHEP::HepRandomEngine* get() { return m_engine.get(); }
  CLHEP::HepRandomEngine& getref() { return *m_engine.get(); }
  CLHEP::HepRandomEngine* operator->() { return get(); }
  // Get smartpointer to a subengine
  HepRandomEnginePtr createSubRndmEngine();

private:
  std::shared_ptr<CLHEP::HepRandomEngine> m_engine{nullptr};
  const RndCommon::RndConstructor* m_constructor{nullptr};
  std::vector<HepRandomEnginePtr> m_children;
  std::string m_label;
};
