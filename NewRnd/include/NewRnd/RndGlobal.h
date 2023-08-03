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
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "NewRnd/RndCommon.h"
#include "TRandom.h"
#include "Utils/LocalTL.h"
#include <iostream>
#include <thread>

#include <functional>

class ThreadLocalgRandom : public TRandom {
  friend class ThreadLocalEngine;

public:
  // Throw a flat random number.
  virtual Double_t Rndm() override;
  // There is some backwards compatible Rndm with arguments we will pull
  // in here to stop pesky little warning from overloaded virtuals
  using TRandom::Rndm;

  // Throw an array of flat random numbers.
  virtual void RndmArray( Int_t n, Double_t* array ) override;

  // Throw an array of floats.
  virtual void RndmArray( Int_t n, Float_t* array ) override;

private:
  // Uses the engine to create a threadlocal CLHEP::RandFlat instance
  // that can be used by this thread;
  static void SetEngine( CLHEP::HepRandomEngine& engine ) {
    Instance().ClearEngine();
    Instance().m_generator = new CLHEP::RandFlat{ engine, 0, 1 };
  }
  // Removes the thread-local instance of CLHEP::RandFlat;
  static void ClearEngine() {
    if ( Instance().m_generator ) {
      delete Instance().m_generator;
      Instance().m_generator = nullptr;
    }
  }

  ThreadLocalgRandom() {
    m_rootGenerator = gRandom;
    gRandom         = this;
  }
  ~ThreadLocalgRandom() { gRandom = m_rootGenerator; }

  static ThreadLocalgRandom& Instance() {
    static ThreadLocalgRandom inst;
    return inst;
  }

  TRandom*                             m_rootGenerator = nullptr;
  static thread_local CLHEP::RandFlat* m_generator;
};

class ThreadLocalEngine {
  ThreadLocalEngine() = delete;

public:
  class Guard {
  public:
    Guard()               = delete;
    Guard( const Guard& ) = delete;
    Guard( Guard&& )      = delete;

    Guard( HepRandomEnginePtr& engine ) {
      ThreadLocalEngine::Set( engine );
      ThreadLocalgRandom::SetEngine( *engine.get() );
    }
    ~Guard() {
      ThreadLocalgRandom::ClearEngine();
      ThreadLocalEngine::Unset();
    }
  };

private:
  static void Set( HepRandomEnginePtr& engine ) { m_engine = &engine; }
  static void Unset() { m_engine = nullptr; }

  thread_local static HepRandomEnginePtr* m_engine;

public:
  static CLHEP::HepRandomEngine& Get() { return *m_engine->get(); }
  static HepRandomEnginePtr&     GetPtr() { return *m_engine; }
};
