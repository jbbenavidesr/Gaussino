#pragma once

//#include "Event/GenFSR.h"
#include <mutex>
#include <vector>
#include "Utils/LocalTL.h"
#include "Event/GenFSR.h"

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
  static LHCb::GenFSR* GetGenFSR() {
    static std::mutex init_lock;
    auto& inst = _inst();
    if (!inst._fsr()) {
      // Lock this initialization part.
      // Don't know if the constructor of GenFSR does something
      // nasty but the vector pushback should not happen concurrently
      // as that might end up very badly.
      std::lock_guard<std::mutex> lockguard{init_lock};
      inst._fsr = new LHCb::GenFSR{};
      inst._fsr()->initializeInfos();
      inst._store.push_back(inst._fsr());
    }
    return inst._fsr();
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

  static LHCb::GenFSR* GetCombined() {
    auto& inst = _inst();
    auto _ret = new LHCb::GenFSR{};
    for (auto& fsr : inst._store) {
      (*_ret) += *fsr;
    }
    return _ret;
  }

  private:
  ~GenFSRMTManager(){
    for (auto& fsr : _store) {
      delete fsr;
    }
  };
  GenFSRMTManager() = default;
  GenFSRMTManager(const GenFSRMTManager&) = delete;
  GenFSRMTManager(GenFSRMTManager&&) = delete;
  static inline GenFSRMTManager& _inst() {
    static GenFSRMTManager _instance{};
    return _instance;
  }
  LocalTL<LHCb::GenFSR*> _fsr{nullptr};
  std::vector<LHCb::GenFSR*> _store{};
};
