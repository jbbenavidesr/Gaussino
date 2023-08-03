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
#include <condition_variable>
#include <mutex>

class GiGaMTBarrier {
private:
  std::mutex              _mutex;
  std::condition_variable _cv;
  std::size_t             m_n_waiting;

public:
  explicit GiGaMTBarrier( std::size_t count ) : m_n_waiting( count ) {}
  void wait() {
    std::unique_lock<std::mutex> lock{ _mutex };
    if ( --m_n_waiting == 0 ) {
      _cv.notify_all();
    } else {
      _cv.wait( lock, [this] { return m_n_waiting == 0; } );
    }
  }
  GiGaMTBarrier()                       = delete;
  GiGaMTBarrier( const GiGaMTBarrier& ) = delete;
  GiGaMTBarrier( GiGaMTBarrier&& )      = delete;
};
