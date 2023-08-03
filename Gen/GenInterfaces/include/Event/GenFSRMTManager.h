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

// #include "Event/GenFSR.h"
#include "Event/GenFSR.h"
#include "Utils/LocalTL.h"
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

class GenFSRMTManager {
public:
  /*! \brief Get a thread-local GenFSR instance
   *
   *  Function to obtain a pointer to a thread-local GenFSR instance
   *  which can be filled in each thread separatly. All instances are
   *  internally tracked and the combined FSR is returned on demand
   *
   * \return GenFSR pointer not owned by user
   */
  static LHCb::GenFSR* GetGenFSR( const std::string& name ) {
    if ( name == "" ) { return nullptr; }
    static std::mutex init_lock;
    auto&             inst = _inst();
    if ( inst._fsr.find( name ) == std::end( inst._fsr ) ) {
      // Lock this initialization part.
      // Don't know if the constructor of GenFSR does something
      // nasty but the vector pushback should not happen concurrently
      // as that might end up very badly.
      std::lock_guard<std::mutex> lockguard{ init_lock };
      inst._fsr[name] = new LHCb::GenFSR{};
      inst._fsr[name]->initializeInfos();
      if ( inst._store.find( name ) == std::end( inst._store ) ) { inst._store[name] = {}; }
      inst._store[name].push_back( inst._fsr[name] );
    }
    return inst._fsr[name];
  }
  /*! \brief Get the combination of all currently stored FSR
   *
   * Returns the sum of all used GenFSR instances by the various threads.
   * This method is intended for use during finaliation. And might lead to
   * thread-safety related issues if the stored GenFSR entries are concurrently
   * modified.
   *
   * \return GenFSR pointer owned by user
   */

  static LHCb::GenFSR* GetCombined( const std::string& name ) {
    if ( name == "" ) { return nullptr; }
    static LHCb::GenFSR* _ret{ nullptr };
    if ( !_ret ) {
      _ret       = new LHCb::GenFSR{};
      auto& inst = _inst();
      for ( auto& fsr : inst._store[name] ) { ( *_ret ) += *fsr; }
    }
    return _ret;
  }

private:
  ~GenFSRMTManager() {
    for ( auto& namedstore : _store ) {
      for ( auto& fsr : namedstore.second ) { delete fsr; }
    }
  }
  GenFSRMTManager()                         = default;
  GenFSRMTManager( const GenFSRMTManager& ) = delete;
  GenFSRMTManager( GenFSRMTManager&& )      = delete;
  static inline GenFSRMTManager& _inst() {
    static GenFSRMTManager _instance{};
    return _instance;
  }
  static thread_local std::map<std::string, LHCb::GenFSR*> _fsr;
  std::map<std::string, std::vector<LHCb::GenFSR*>>        _store{};
};

inline thread_local std::map<std::string, LHCb::GenFSR*> GenFSRMTManager::_fsr{};
